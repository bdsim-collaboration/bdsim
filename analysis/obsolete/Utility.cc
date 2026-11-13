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
#include "Utility.hh"

#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TH1D.h"

#include "Event.hh"

TVector3 LocalToGlobal(const TVector3 &vLocal, const TRotation &r, const TVector3 &d)
{
  return r*vLocal+d;
}

std::vector<TH1D*>& GetRun1DHistograms(TString fileName)
{
  TFile *f = new TFile(fileName.Data());
  TTree *t = (TTree*)f->Get("Run");
  BDSOutputROOTEventHistograms *h = 0;
  t->SetBranchAddress("Histos.",&h);
  t->GetEntry(0);
  return h->Get1DHistograms();
}

BDSOutputROOTEventOptions* GetOptions(TString fileName)
{
  TFile *f = new TFile(fileName.Data());
  TTree *t = (TTree*)f->Get("Options");
  BDSOutputROOTEventOptions *h = 0;
  t->SetBranchAddress("Options.",&h);
  t->GetEntry(0);
  return h;
}

BDSOutputROOTEventModel* GetModel(TString fileName)
{
  TFile *f = new TFile(fileName.Data());
  TTree *t = (TTree*)f->Get("Model");
  BDSOutputROOTEventModel *h = 0;
  t->SetBranchAddress("Model.",&h);
  t->GetEntry(0);
  return h;
}

void GetEvent(TString fileName, TChain *c, Event *e)
{
  c->Add(fileName.Data());
  std::vector<std::string> samplerNames;
  e->SetBranchAddress(c,samplerNames);
  return;
}
