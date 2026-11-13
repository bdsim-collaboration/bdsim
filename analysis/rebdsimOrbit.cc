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
 * @file rebdsimOrbit.cc
 */

#include <iostream>
#include <string>

#include "DataLoader.hh"
#include "EventAnalysisOrbit.hh"
#include "RBDSException.hh"

#include "BDSOutputROOTEventHeader.hh"

#include "TFile.h"
#include "TTree.h"

void usage()
{ 
  std::cout << "usage: rebdsimOrbit <dataFile> <outputfile> <index>"  << std::endl;
  std::cout << " <datafile>   - root file to operate on ie run1.root" << std::endl;
  std::cout << " <outputfile> - name of output file ie optics.root"   << std::endl;
  std::cout << " <index>      - index of orbit to pull from file (default 1)" << std::endl;
  std::cout << "This only works on a single file - no wildcards."     << std::endl;
}

int main(int argc, char* argv[])
{
  if (argc < 3 || argc >= 5)
    {
      usage();
      return 1;
    }

  std::string inputFileName  = std::string(argv[1]);
  std::string outputFileName = std::string(argv[2]);
  int index = 0;
  try
    {
      if (argc == 4)
        {index = std::stoi(argv[3]);}
      
      DataLoader dl = DataLoader(inputFileName, false, true, true);
      EventAnalysisOrbit* evtAnalysis = new EventAnalysisOrbit(dl.GetEvent(),
                                                               dl.GetEventTree(),
                                                               true, true);
      evtAnalysis->ExtractOrbit(index);
      
      TFile* outputFile = new TFile(outputFileName.c_str(), "RECREATE");
      
      // add header for file type and version details
      outputFile->cd();
      BDSOutputROOTEventHeader* headerOut = new BDSOutputROOTEventHeader();
      headerOut->Fill(dl.GetFileNames()); // updates time stamp
      headerOut->SetFileType("REBDSIM");
      TTree* headerTree = new TTree("Header", "REBDSIM Header");
      headerTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);
      headerTree->Fill();
      headerTree->Write();
      
      evtAnalysis->WriteOrbit(outputFile);
      outputFile->Close();
      std::cout << "Result written to: " << outputFileName << std::endl;
      delete outputFile;
      delete evtAnalysis;
    }
  catch (const RBDSException& error)
    {std::cerr << error.what() << std::endl; return 1;}
  catch (const std::exception& error)
    {std::cerr << error.what() << std::endl; return 1;}
      
  return 0;
}
