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
#include "RebdsimTypes.hh"
#include "Run.hh"

#include "BDSOutputROOTEventRunInfo.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSVersionData.hh"

#include <iostream>
#include <vector>

ClassImp(Run)

Run::Run():
  Run(false)
{;}

Run::Run(bool debugIn):
  Summary(nullptr),
  Histos(nullptr),
  Info(nullptr),
  debug(debugIn),
  dataVersion(BDSIM_DATA_VERSION)
{
  Summary = new BDSOutputROOTEventRunInfo();
  Histos  = new BDSOutputROOTEventHistograms();
  Info    = new BDSOutputROOTEventRunInfo();
}

Run::Run(bool debugIn,
	 int  dataVersionIn):
  Summary(nullptr),
  Histos(nullptr),
  Info(nullptr),
  debug(debugIn),
  dataVersion(dataVersionIn)
{
  Summary = new BDSOutputROOTEventRunInfo();
  Histos  = new BDSOutputROOTEventHistograms();
  Info    = new BDSOutputROOTEventRunInfo();
} 

Run::~Run()
{
  delete Summary;
  delete Histos;
  delete Info;
}

void Run::SetBranchAddress(TTree *t,
			   bool                      allBranchesOn,
			   const RBDS::VectorString* branchesToTurnOn)
{
  // turn off all branches by default.
  t->SetBranchStatus("*", false);

  if (allBranchesOn)
    {t->SetBranchStatus("*", true);}
  else if (branchesToTurnOn)
    {
      for (const auto& name : *branchesToTurnOn)
	{
	  std::string nameStar = name + "*";
	  if (debug)
	    {std::cout << "Run::SetBranchAddress> Turning on branch \"" << nameStar << "\"" << std::endl;}
	  t->SetBranchStatus(nameStar.c_str(), true);
	}
    }

  if (dataVersion < 4)
    {t->SetBranchAddress("Info.", &Info);}
  else
    {t->SetBranchAddress("Summary.", &Summary);}
  t->SetBranchAddress("Histos.", &Histos);
}
