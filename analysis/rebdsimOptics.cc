/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
 * @file rebdsimOptics.cc
 */
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "AnalysisUtilities.hh"
#include "Beam.hh"
#include "Config.hh"
#include "DataLoader.hh"
#include "EventAnalysis.hh"
#include "Options.hh"
#include "RBDSException.hh"

#include "BDSOutputROOTEventBeam.hh"
#include "BDSOutputROOTEventHeader.hh"
#include "BDSOutputROOTEventOptions.hh"

#include "TFile.h"
#include "TChain.h"

void usage()
{ 
  std::cout << "usage: rebdsimOptics <datafile> (<outputfile>) (--emittanceOnFly)"                 << std::endl;
  std::cout << " <datafile>   - root file to operate on ie run1.root"                              << std::endl;
  std::cout << " <outputfile> - name of output file ie optics.root. Must be different to datafile" << std::endl;
  std::cout << " --emittanceOnTheFly - calculate emittance per sampler (optional)"                 << std::endl;
  std::cout << " Quotes should be used if * is used in the input file name."                       << std::endl;
  std::cout << " <outputfile> is optional - default is <datafile>_optics.root"                     << std::endl;
}

int main(int argc, char* argv[])
{
  if (argc < 2 || argc > 4)
    {
      std::cout << "Incorrect number of arguments." << std::endl;
      usage();
      return 1;
    }
  
  // parse arguments
  std::vector<std::string> arguments;
  for (int i = 1; i < argc; i++) // skip name of the program
    {arguments.emplace_back(std::string(argv[i]));}
  std::set<std::string> argumentsSet = {arguments.begin(), arguments.end()};
  
  // emittance on the fly
  bool emittanceOnFly = argumentsSet.count("--emittanceOnTheFly") > 0 || argumentsSet.count("--emittanceOnFly") > 0;
  if (emittanceOnFly)
    {std::cout << "Calculating emittance per sampler" << std::endl;}
  // remove this option from vector of arguments
  arguments.erase(std::remove_if(arguments.begin(),
                                 arguments.end(),
                                 [](const std::string& s){ return s == "--emittanceOnTheFly" || s == "--emittanceOnFly";}),
                  arguments.end());

  std::string inputFileName = arguments[0];
  std::string outputFileName;
  if (arguments.size() > 1)
    {outputFileName = arguments[1];}
  if (arguments.size() > 2)
    {
      for (int i = 2; i < (int)arguments.size(); i++)
        {std::cout << "Unknown option \"" << arguments[i] << "\"" << std::endl;}
      usage();
      return 1;
    }

  if (inputFileName == outputFileName)
    {
      std::cout << "outputfile same as datafile" << std::endl;
      usage();
      return 1;
    }
  
  if (outputFileName.empty())
    {
      outputFileName = RBDS::DefaultOutputName(inputFileName, "_optics");
      std::cout << "Using default output file name with \"_optics\" suffix  : " << outputFileName << std::endl;
    }

  DataLoader* dl = nullptr;
  try
    {dl = new DataLoader(inputFileName, false, true);}
  catch (const RBDSException& error)
    {std::cerr << error.what() << std::endl; return 1;}
  catch (const std::exception& error)
    {std::cerr << error.what() << std::endl; return 1;}

  // beam required to get the mass of the primary particle in EventAnalysis
  Beam*   beam     = dl->GetBeam();
  TChain* beamTree = dl->GetBeamTree();
  BDSOutputROOTEventBeam* outputBeam = beam->beam;
  beamTree->GetEntry(0);
  const std::string& particleName = outputBeam->particle;
  
  TChain* modelTree = dl->GetModelTree();
  if (modelTree->GetEntries() == 0)
    {
      std::cout << "Warning: data file written without Model tree that is required to know the sampler names" << std::endl;
      std::cout << "         only the primary sampler will be analysed if available" << std::endl;
    }

  EventAnalysis* evtAnalysis;
  try
    {
      evtAnalysis = new EventAnalysis(dl->GetEvent(), dl->GetEventTree(),
                                      false, true, false, false, true, -1,
                                      emittanceOnFly, 0, -1, particleName);
      evtAnalysis->Execute();
    }
  catch (const RBDSException& error)
    {std::cerr << error.what() << std::endl; return 1;}

  TFile* outputFile = new TFile(outputFileName.c_str(), "RECREATE");

  // add header for file type and version details
  outputFile->cd();
  BDSOutputROOTEventHeader* headerOut = new BDSOutputROOTEventHeader();
  headerOut->Fill(dl->GetFileNames()); // updates time stamp
  headerOut->SetFileType("REBDSIM");
  TTree* headerTree = new TTree("Header", "REBDSIM Header");
  headerTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);
  headerTree->Fill();
  headerTree->Write("", TObject::kOverwrite);

  // write merged histograms and optics
  evtAnalysis->Write(outputFile);

  // Don't clone the model tree if only primaries are generated - model not created in BDSIM
  Options* options = dl->GetOptions();
  TChain*  optionsTree = dl->GetOptionsTree();
  BDSOutputROOTEventOptions* ob = options->options;
  optionsTree->GetEntry(0);
  if (!ob->generatePrimariesOnly)
    {
      // clone model tree for nice built in optics plotting
      auto newTree = modelTree->CloneTree();
      newTree->Write("", TObject::kOverwrite);
    }
  
  outputFile->Close();
  delete outputFile;
  std::cout << "Result written to: " << outputFileName << std::endl;
  delete dl;
  delete evtAnalysis;
  
  return 0;
}
