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
 * @file rebdsim.cc
 */

#include <iostream>
#include <string>
#include <vector>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

#include "BDSOutputROOTEventHeader.hh"
#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventModel.hh"

#include "Analysis.hh"
#include "BeamAnalysis.hh"
#include "Config.hh"
#include "DataLoader.hh"
#include "EventAnalysis.hh"
#include "HeaderAnalysis.hh"
#include "ModelAnalysis.hh"
#include "OptionsAnalysis.hh"
#include "RBDSException.hh"
#include "RebdsimTypes.hh"
#include "RunAnalysis.hh"

int main(int argc, char *argv[])
{
  // check input
  if (argc < 2 || argc > 4)
    {
      std::cout << "usage: rebdsim <analysisConfig> (<dataFile>) (<outputFile>)" << std::endl;
      std::cout << " <datafile> (optional) - root file to operate on" << std::endl;
      std::cout << " <outputfile> (optional) - output file name for analysis" << std::endl;
      std::cout << " if no <datafile> and <outputfile> are specified, those from <analysisConfig> are used." << std::endl;
      exit(1);
    }

  std::string configFilePath = std::string(argv[1]); //create a string from arguments so able to use find_last_of and substr  methods
  std::string configFileExtension = configFilePath.substr(configFilePath.find_last_of('.') + 1) ;
  if (configFileExtension != "txt")
    {
      std::cerr << "Unrecognised extension for file: " << configFilePath << ".  Extension: " << configFileExtension << std::endl;
      std::cerr << "Make sure the config file is plain text with the .txt extension!" << std::endl;
      exit(1);
    }

  std::cout << "rebdsim> configuration file name : " << configFilePath << std::endl;

  std::string inputFilePath = "";  // default of "" means use ones specified in analysisConfig.txt
  std::string outputFileName = "";

  if (argc > 2)
    {inputFilePath = std::string(argv[2]);}
  if (argc > 3)
    {outputFileName = std::string(argv[3]);}

  // parse input file with options and histogram definitions
  Config* config = nullptr;
  try
    {
      Config::Instance(configFilePath, inputFilePath, outputFileName);
      config = Config::Instance();
      
      bool allBranches = config->AllBranchesToBeActivated();
      const RBDS::BranchMap* branchesToActivate = &(config->BranchesToBeActivated());
      
      bool debug = config->Debug();
      DataLoader* dl = new DataLoader(config->InputFilePath(),
                                      debug,
                                      config->ProcessSamplers(),
                                      allBranches,
                                      branchesToActivate,
                                      config->GetOptionBool("backwardscompatible"));

      config->FixCylindricalAndSphericalSamplerVariablesInSets(dl->GetAllCylindricalSamplerNames(),
                                                               dl->GetAllSphericalSamplerNames());

      auto filenames = dl->GetFileNames();
      HeaderAnalysis* ha = new HeaderAnalysis(filenames,
                                              dl->GetHeader(),
                                              dl->GetHeaderTree());
      unsigned long long int nEventsRequested = 0;
      unsigned long long int nEventsInFileTotal = 0;
      unsigned long long int nEventsInFileSkippedTotal = 0;
      unsigned int distrFileLoopNTimes = 0;
      unsigned long long int nOriginalEvents = ha->CountNOriginalEvents(nEventsInFileTotal,
                                                                        nEventsInFileSkippedTotal,
                                                                        nEventsRequested,
                                                                        distrFileLoopNTimes);
      delete ha;
      
      BeamAnalysis* beaAnalysis = new BeamAnalysis(dl->GetBeam(),
                                                   dl->GetBeamTree(),
                                                   config->PerEntryBeam(),
                                                   debug);
      EventAnalysis* evtAnalysis;
      evtAnalysis = new EventAnalysis(dl->GetEvent(),
                                      dl->GetEventTree(),
                                      config->PerEntryEvent(),
                                      config->ProcessSamplers(),
                                      dl->DataVersion() < 10,
                                      debug,
                                      config->PrintOut(),
                                      config->PrintModuloFraction(),
                                      config->EmittanceOnTheFly(),
                                      (long int) config->GetOptionNumber("eventstart"),
                                      (long int) config->GetOptionNumber("eventend"));
      
      RunAnalysis* runAnalysis = new RunAnalysis(dl->GetRun(),
                                                 dl->GetRunTree(),
                                                 config->PerEntryRun(),
                                                 debug);
      OptionsAnalysis* optAnalysis = new OptionsAnalysis(dl->GetOptions(),
                                                         dl->GetOptionsTree(),
                                                         config->PerEntryOption(),
                                                         debug);
      ModelAnalysis* modAnalysis = new ModelAnalysis(dl->GetModel(),
                                                     dl->GetModelTree(),
                                                     config->PerEntryModel(),
                                                     debug);
      
      std::vector<Analysis*> analyses = {beaAnalysis,
                                         evtAnalysis,
                                         runAnalysis,
                                         optAnalysis,
                                         modAnalysis};
      
      for (auto &analysis: analyses)
        {analysis->Execute();}
      
      // write output
      TFile* outputFile = new TFile(config->OutputFileName().c_str(),"RECREATE");
      
      // add header for file type and version details
      outputFile->cd();
      BDSOutputROOTEventHeader* headerOut = new BDSOutputROOTEventHeader();
      headerOut->Fill(dl->GetFileNames()); // updates time stamp
      headerOut->SetFileType("REBDSIM");
      headerOut->nOriginalEvents = nOriginalEvents;
      headerOut->nEventsInFile = nEventsInFileTotal;
      headerOut->nEventsInFileSkipped = nEventsInFileSkippedTotal;
      headerOut->nEventsRequested = nEventsRequested;
      headerOut->distrFileLoopNTimes = distrFileLoopNTimes;
      TTree* headerTree = new TTree("Header", "REBDSIM Header");
      headerTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);
      headerTree->Fill();
      headerTree->Write("", TObject::kOverwrite);
      
      for (auto& analysis : analyses)
        {analysis->Write(outputFile);}

      // copy the model over and rename to avoid conflicts with Model directory
      TChain* modelTree = dl->GetModelTree();
      TTree* treeTest = modelTree->GetTree();
      if (treeTest)
        {// TChain can be valid but TTree might not be in corrupt / bad file
          auto newTree = modelTree->CloneTree();
          // unfortunately we have a folder called Model in histogram output files
          // avoid conflict when copying the model for plotting
          newTree->SetName("ModelTree");
          newTree->Write("", TObject::kOverwrite);
        }

      outputFile->Close();
      delete outputFile;
      std::cout << "Result written to: " << config->OutputFileName() << std::endl;

      delete dl;
      for (auto analysis : analyses)
        {delete analysis;}
    }
  catch (const RBDSException& error)
    {std::cerr << error.what() << std::endl; exit(1);}
  catch (const std::exception& error)
    {std::cerr << error.what() << std::endl; exit(1);}
  
  return 0;
}
