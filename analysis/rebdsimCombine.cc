/* 
Beam Delivery Simulation (BDSIM) Copyright (C) BDSIM Collaboration, 2001 - 2026.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * @file rebdsimCombine.cc
 */
#include "FileMapper.hh"
#include "Header.hh"
#include "HistogramAccumulatorMerge.hh"
#include "HistogramAccumulatorSum.hh"
#include "RBDSException.hh"

#include "BDSOutputROOTEventHeader.hh"

#include "TDirectory.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "BDSBH4D.hh"
#include "TTree.h"

#include <exception>
#include <iostream>
#include <set>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
  if (argc < 3)
    {
      std::cout << "usage: rebdsimCombine result.root file1.root file2.root ..." << std::endl;
      return 1;
    }

  // build input file list
  std::vector<std::string> inputFiles;
  for (int i = 2; i < argc; ++i)
    {inputFiles.emplace_back(std::string(argv[i]));}

  // checks
  if (inputFiles.size() == 1)
    {
      if (inputFiles[0].find('*') != std::string::npos) // glob didn't expand in shell - infer this
        {std::cout << "Glob with * did not match any files" << std::endl;}
      else
        {std::cout << "Only one input file provided \"" << inputFiles[0] << "\" - no point." << std::endl;}
      return 1;
    }
  
  std::set<std::string> inputFilesSet = {inputFiles.begin(), inputFiles.end()};
  if (inputFiles.size() > inputFilesSet.size())
    {std::cout << "Warning: at least 1 duplicate name in list of files provided to combine." << std::endl;}

  std::string outputFile = std::string(argv[1]);
  if (outputFile.find('*') != std::string::npos)
    {
      std::cerr << "First argument for output file \"" << outputFile << "\" contains an *." << std::endl;
      std::cerr << "Should only be a singular file - check order of arguments." << std::endl;
      return 1;
    }
  
  // output file must be opened before histograms are created because root does
  // everything statically behind the scenes
  TFile* output = new TFile(outputFile.c_str(), "RECREATE");
  
  // add header for file type and version details
  output->cd();
  BDSOutputROOTEventHeader* headerOut = new BDSOutputROOTEventHeader();
  headerOut->Fill(std::vector<std::string>(), inputFiles); // updates time stamp
  headerOut->SetFileType("REBDSIMCOMBINE");
  TTree* headerTree = new TTree("Header", "REBDSIM Header");
  headerTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);

  // ensure new histograms are written to file
  TH1::AddDirectory(true);
  TH2::AddDirectory(true);
  TH3::AddDirectory(true);

  TFile* f = nullptr; // temporary variable

  // initialise file map
  try
    {f = new TFile(inputFiles[0].c_str(), "READ");}
  catch (const std::exception& e)
    {std::cerr << e.what() << std::endl; return 1;}
  HistogramMap* histMap = nullptr;
  try
    {histMap = new HistogramMap(f, output);} // map out first file
  catch (const RBDSException& error)
    {std::cerr << error.what() << std::endl; return 1;}
  catch (const std::exception& error)
    {std::cerr << error.what() << std::endl; return 1;}
  
  // copy the model tree over if it exists - expect the name to be ModelTree
  TTree* oldModelTree = dynamic_cast<TTree*>(f->Get("ModelTree"));
  if (!oldModelTree)
    {oldModelTree = dynamic_cast<TTree*>(f->Get("Model"));}
  if (oldModelTree)
    {// TChain can be valid but TTree might not be in corrupt / bad file
      output->cd();
      auto newTree = oldModelTree->CloneTree();
      newTree->SetName("ModelTree");
      newTree->Write("", TObject::kOverwrite);
    }
  
  f->Close();
  delete f;

  std::vector<RBDS::HistogramPath> histograms = histMap->Histograms();

  unsigned long long int nOriginalEvents = 0;
  unsigned long long int nEventsInFile = 0;
  unsigned long long int nEventsInFileSkipped = 0;
  unsigned long long int nEventsRequested = 0;
  
  std::cout << "Combination of " << inputFiles.size() << " files beginning" << std::endl;
  // loop over files and accumulate
  for (const auto& file : inputFiles)
    {
      f = new TFile(file.c_str());
      if (RBDS::IsREBDSIMOrCombineOutputFile(f))
        {
          std::cout << "Accumulating> " << file << std::endl;
          for (const auto& hist : histograms)
            {
              std::string histPath = hist.path + hist.name; // histPath has trailing '/'

              TH1* h = dynamic_cast<TH1*>(f->Get(histPath.c_str()));

              if (!h)
                {RBDS::WarningMissingHistogram(histPath, file); continue;}
              hist.accumulator->Accumulate(h);
            }
          
          Header* h = new Header();
          TTree* ht = (TTree*)f->Get("Header");
          h->SetBranchAddress(ht);
          ht->GetEntry(0);
          nOriginalEvents += h->header->nOriginalEvents;
          // Here we exploit the fact that the 0th entry of the header tree has no data for these
          // two variables. There may however, only ever be 1 entry for older data. We add it up anyway.
          for (int i = 0; i < (int)ht->GetEntries(); i++)
            {
              ht->GetEntry(i);
              nEventsInFile += h->header->nEventsInFile;
              nEventsInFileSkipped += h->header->nEventsInFileSkipped;
              nEventsRequested += h->header->nEventsRequested;
            }
          delete h;
        }
      else
        {std::cout << "Skipping " << file << " as not a rebdsim output file" << std::endl;}
      f->Close();
      delete f;
    }
  
  // terminate and write output
  for (const auto& hist : histograms)
    {
      TH1* result = hist.accumulator->Terminate();
      result->SetDirectory(hist.outputDir);
      hist.outputDir->Add(result);
      delete hist.accumulator; // this removes temporary histograms from the file
    }

  headerOut->nOriginalEvents = nOriginalEvents;
  headerOut->nEventsInFile = nEventsInFile;
  headerOut->nEventsInFileSkipped = nEventsInFileSkipped;
  headerOut->nEventsRequested = nEventsRequested;
  headerTree->Fill();

  output->Write(nullptr,TObject::kOverwrite);
  output->Close();
  delete output;
  std::cout << "Combined result of " << inputFiles.size() << " files written to: " << outputFile << std::endl;
  
  return 0;
}
