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
 * @file bdsimCombine.cc
 */
#include "FileMapper.hh"
#include "Header.hh"

#include "BDSOutputROOTEventHeader.hh"

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

#include <exception>
#include <glob.h>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
  if (argc < 3)
    {
      std::cout << "usage: bdsimCombine result.root file1.root file2.root ..." << std::endl;
      std::cout << "usage: bdsimCombine result.root 2GB file1.root file2.root ..." << std::endl;
      std::cout << "Optional argument (e.g. 2GB) is file chunk size in GB. Must have \"GB\".";
      std::cout << "File globbing can be done internally - use quotes around regex, e.g. \"raw_*.root\"" << std::endl;
      exit(1);
    }

  // build input file list
  std::vector<std::string> inputFiles;
  for (int i = 2; i < argc; ++i)
    {inputFiles.emplace_back(std::string(argv[i]));}
  
  // check for optional file chunks size limit
  Long64_t fileChunkSize = 100000000000LL;
  std::size_t gbFound = inputFiles[0].find("GB");
  std::size_t mbFound = inputFiles[0].find("MB");
  if (gbFound != std::string::npos || mbFound != std::string::npos)
    {
      std::size_t unitPosition;
      double unit = 1.0;
      if (gbFound != std::string::npos)
	{
	  unitPosition = gbFound;
	  unit = 1e9;
	}
      else
	{
	  unitPosition = mbFound;
	  unit = 1e6;
	}
      std::string numberString = inputFiles[0].substr(0, unitPosition);
      double fsize;
      try
	{fsize = std::stod(numberString);}
      catch (const std::logic_error& e)
	{
	  std::cerr << "Invalid number for file chunk size \"" << numberString << "\"" << std::endl;
	  exit(1);
	}
      
      fileChunkSize = (Long64_t)  (fsize * unit);
      if (fileChunkSize == 0)
	{
	  std::cerr << "File chunk size is 0 -> must be > 0" << std::endl;
	  exit(1);
	}
      
      // the file chunk size was specified to remove from list of files
      inputFiles.erase(inputFiles.begin());
      std::cout << "Result file will be in chunks of " << fileChunkSize / 1000000LL << " MB" << std::endl;
      TTree::SetMaxTreeSize(fileChunkSize);
    }
  
  // see if we're globbing files
  if (inputFiles[0].find('*') != std::string::npos)
    {
      std::vector<std::string> fileNamesTemp;
      glob_t glob_result;
      glob(inputFiles[0].c_str(),GLOB_TILDE,nullptr,&glob_result);
      for (unsigned int i=0; i<glob_result.gl_pathc; ++i)
	{fileNamesTemp.emplace_back(glob_result.gl_pathv[i]);}
      globfree(&glob_result);
      inputFiles = fileNamesTemp;
    }

  // checks
  if (inputFiles.size() == 1)
    {
      std::cout << "Only one input file provided \"" << inputFiles[0] << "\" - no point." << std::endl;
      exit(1);
    }
  // check for wrong order of arguments which is common mistake
  std::string outputFile = std::string(argv[1]);
  if (outputFile.find('*') != std::string::npos)
    {
      std::cerr << "First argument for output file \"" << outputFile << "\" contains an *." << std::endl;
      std::cerr << "Should only be a singular file - check order of arguments." << std::endl;
      exit(1);
    }

  // merge event trees - this is done by ROOT and it writes the output file and closes it
  TChain* eventsMerged = new TChain("Event");
  for (const auto& filename : inputFiles)
    {eventsMerged->Add(filename.c_str());}
  std::cout << "Beginning merge of Event Tree" << std::endl;
  Long64_t operationCode = eventsMerged->Merge(outputFile.c_str());
  if (operationCode == 0)
    {// from inspection of ROOT TChain.cxx ~line 1866, it returns 0 if there's a problem
      std::cerr << "Problem in TTree::Merge opening output file \"" << outputFile << "\"" << std::endl;
      return 1;
    }
  else
    {std::cout << "Finished merge of Event Tree" << std::endl;}
  
  // loop over input files loading headers to accumulate number of original events
  unsigned long long int nOriginalEvents = 0;
  bool skimmedFile = false;
  unsigned long int i = 0;
  std::cout << "Counting number of original events from headers of files" << std::endl;
  for (const auto& filename : inputFiles)
    {
      TFile* f = new TFile(filename.c_str(), "READ");
      if (!RBDS::IsBDSIMOutputFile(f))
	{
	  std::cout << "File \"" << filename << "\" skipped as not a valid BDSIM file" << std::endl;
	  if (!f->IsZombie())
	    {f->Close();}
	  delete f;
	  continue;
	}
      std::cout << "Accumulating> " << filename << std::endl;
      TTree* headerTree = dynamic_cast<TTree*>(f->Get("Header")); // should be safe given check we've just done
      if (!headerTree)
	{
	  std::cerr << "Problem getting header from file " << filename << std::endl;
	  f->Close();
	  delete f;
	  continue;
	}
      Header* headerLocal = new Header();
      headerLocal->SetBranchAddress(headerTree);
      headerTree->GetEntry(0);
      skimmedFile = skimmedFile || headerLocal->header->skimmedFile;
      if (headerLocal->header->skimmedFile)
	{nOriginalEvents += headerLocal->header->nOriginalEvents;}
      else
	{// unskimmed file which won't record the number of events in the header, so we inspect the Event Tree
	  TTree* eventTree = dynamic_cast<TTree*>(f->Get("Event"));
	  if (eventTree)
	    {
	      Long64_t nEntries = eventTree->GetEntries();
	      nOriginalEvents += (unsigned long long int)nEntries;
	    }
	  else
	    {std::cerr << "Problem getting Event tree in file " << filename << std::endl;}
	}
      delete headerLocal;
      f->Close();
      delete f;
      i++;
    }

  // checks
  if (i == 0)
    {std::cerr << "No valid files found" << std::endl; return 1;}
  
  // now we produce a new header and update the file as well as copy over the other trees from the first valid
  // input file in the list (i.e. tolerate the odd zombie file from a big run)
  TFile* input = nullptr;
  bool validFile = false;
  i = 0;
  while (!validFile)
    {// assume we have one valid file from check above
      if (i > (unsigned long int)inputFiles.size())
	{return 1;}
      input = new TFile(inputFiles[i].c_str(), "READ");
      validFile = RBDS::IsBDSIMOutputFile(input);
      i++;
    }
  
  TFile* output = new TFile(outputFile.c_str(), "UPDATE");
  if (output->IsZombie())
    {std::cerr << "Could not reopen output file to add other trees"; return 1;}
  output->cd();
  BDSOutputROOTEventHeader* headerOut = new BDSOutputROOTEventHeader();
  headerOut->Fill(std::vector<std::string>(), inputFiles); // updates time stamp
  headerOut->SetFileType("BDSIM");
  headerOut->skimmedFile = skimmedFile;
  headerOut->nOriginalEvents = nOriginalEvents;
  TTree* headerTree = new TTree("Header", "BDSIM Header");
  headerTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);
  headerTree->Fill();
  output->Write(nullptr,TObject::kOverwrite);

  // go over all other trees and copy them (in the original order) from the first file to the output
  std::cout << "Merging rest of file contents" << std::endl;
  std::vector<std::string> treeNames = {"ParticleData", "Beam", "Options", "Model", "Run"};
  for (const auto& tn : treeNames)
    {
      TTree* original = dynamic_cast<TTree*>(input->Get(tn.c_str()));
      if (!original)
	{
	  std::cerr << "Failed to load Tree named " << tn << std::endl;
	  delete output;
	  delete input;
	  return 1;
	}
      auto clone = original->CloneTree();
      clone->AutoSave();
    }
  
  output->Close();
  delete output;
  
  std::cout << "Combined result of " << inputFiles.size() << " files written to: " << outputFile << std::endl;
  std::cout << "Run histograms are not summed" << std::endl; // TODO
  return 0;
}
