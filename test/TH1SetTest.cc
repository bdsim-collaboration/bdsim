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
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

#include "TH1D.h"
#include "TH1Set.hh"

#include "DataLoader.hh"

#include <iostream>


int main(int /*argc*/, char** /*argv*/)
{

  //TFile* tf = new TFile("t1.root");
  //TTree* event = (TTree*)tf->Get("Event");

  TH1D* hist0 = new TH1D("histy", "histy", 1, 0, 10000);
  TH1Set* hist = new TH1Set("test1", "test1");
  std::cout << hist0 << " " << hist << std::endl;
  /*
  hist->SetCanExtend(111);
  event->Draw("c1.partID >> test1", "", "goff");
  event->Draw("c1.partID >> histy", "", "goff");
  hist->Dump();
  hist->Fill(11);
  hist->Fill(-11);
  */
  DataLoader* dl = new DataLoader("t2.root");
  TChain* evtT = dl->GetEventTree();
  evtT->Draw("c1.partID>>histy", "", "goff");
  evtT->Draw("c1.partID>>test1", "", "goff");
  
  //evtT->Draw("c1.partID");
  std::cout << evtT->GetEntries() << std::endl;

  
  
  //tf->Close();
  return 0;
}
