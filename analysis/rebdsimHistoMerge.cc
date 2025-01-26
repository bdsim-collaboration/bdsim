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

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

#include "BDSOutputROOTEventHeader.hh"
#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventModel.hh"

#include "Analysis.hh"
#include "Config.hh"
#include "DataLoader.hh"
#include "BeamAnalysis.hh"
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
  if (argc < 2 || argc > 3)
    {
      std::cout << "usage: rebdsim <datafile> (<outputFile>)" << std::endl;
      std::cout << " <datafile> - root file to operate on" << std::endl;
      std::cout << " <outputfile> - output file name for analysis" << std::endl;
      std::cout << " <outputfile> is optional - default is <datafile>_histos.root" << std::endl;
      return 1;
    }
  
  std::string inputFilePath = std::string(argv[1]);
  std::string outputFileName;
  if (argc == 3) // optional
    {outputFileName = std::string(argv[2]);}

  try
    {
      // Setup config
      Config* config = Config::Instance("", inputFilePath, outputFileName, "_histos");
      
      bool allBranches = config->AllBranchesToBeActivated();
      const RBDS::BranchMap* branchesToActivate = &(config->BranchesToBeActivated());
      
      bool debug = config->Debug();
      DataLoader* dl = new DataLoader(config->InputFilePath(),
                                      debug,
                                      config->ProcessSamplers(),
                                      allBranches,
                                      branchesToActivate,
                                      config->GetOptionBool("backwardscompatible"));
      
      BeamAnalysis* beaAnalysis = new BeamAnalysis(dl->GetBeam(),
                                                   dl->GetBeamTree(),
                                                   config->PerEntryBeam(),
                                                   debug);
      
      EventAnalysis* evtAnalysis = new EventAnalysis(dl->GetEvent(),
                                                     dl->GetEventTree(),
                                                     config->PerEntryEvent(),
                                                     config->ProcessSamplers(),
                                                     dl->DataVersion() < 10,
                                                     debug,
                                                     config->PrintOut(),
                                                     config->PrintModuloFraction(),
                                                     config->GetOptionBool("emittanceonthefly"));
      
      RunAnalysis* runAnalysis = new RunAnalysis(dl->GetRun(),
                                                 dl->GetRunTree(),
                                                 config->PerEntryRun(),
                                                 debug);

      OptionsAnalysis* optAnalysis = new OptionsAnalysis(dl->GetOptions(),
                                                         dl->GetOptionsTree(),
                                                         config->PerEntryOption(),
                                                         debug);

      ModelAnalysis*   modAnalysis = new ModelAnalysis(dl->GetModel(),
                                                       dl->GetModelTree(),
                                                       config->PerEntryModel(),
                                                       debug);
      
      auto filenames = dl->GetFileNames();
      HeaderAnalysis* ha = new HeaderAnalysis(filenames,
                                              dl->GetHeader(),
                                              dl->GetHeaderTree());
      unsigned long long int nEventsInFileTotal;
      unsigned long long int nEventsInFileSkippedTotal;
      unsigned long long int nEventsRequested;
      unsigned int distrFileLoopNTimes;
      unsigned long long int nOriginalEvents = ha->CountNOriginalEvents(nEventsInFileTotal,
                                                                        nEventsInFileSkippedTotal,
                                                                        nEventsRequested,
                                                                        distrFileLoopNTimes);
      
      std::vector<Analysis*> analyses = {beaAnalysis,
                                         evtAnalysis,
                                         runAnalysis,
                                         optAnalysis,
                                         modAnalysis};
      
      for (auto& analysis : analyses)
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
      TTree* headerTree = new TTree("Header", "REBDSIM Header");
      headerTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);
      headerTree->Fill();
      headerTree->Write("", TObject::kOverwrite);
      
      for (auto& analysis : analyses)
	{analysis->Write(outputFile);}

      // copy the model over and rename to avoid conflicts with Model directory
      auto modelTree = dl->GetModelTree();
      auto newTree = modelTree->CloneTree();
      // unfortunately we have a folder called Model in histogram output files
      // avoid conflict when copying the model for plotting
      newTree->SetName("ModelTree");
      newTree->Write("", TObject::kOverwrite);

      outputFile->Close();
      delete outputFile;
      std::cout << "Result written to: " << config->OutputFileName() << std::endl;
      delete dl;
      for (auto analysis : analyses)
        {delete analysis;}
    }
  catch (const RBDSException& error)
    {std::cerr << error.what() << std::endl; return 1;}
  catch (const std::exception& error)
    {std::cerr << error.what() << std::endl; return 1;}
  return 0;
}
