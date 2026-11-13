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
#include "Beam.hh"
#include "BDSOutputROOTEventBeam.hh"

#include "Rtypes.h" // for classdef
#include "TTree.h"

#include <iostream>

ClassImp(Beam)

Beam::Beam():
  Beam(false)
{;}

Beam::Beam(bool debugIn):
  debug(debugIn)
{
  beam = new BDSOutputROOTEventBeam();
}

Beam::~Beam()
{
  delete beam;
}

void Beam::SetBranchAddress(TTree *t,
			    bool                      allBranchesOn,
			    const RBDS::VectorString* branchesToTurnOn)
{
  // turn off all branches by default.
  t->SetBranchStatus("*", false);

  if (allBranchesOn)
    {t->SetBranchStatus("*", true);}
  else if (branchesToTurnOn)
    {
      for (auto name : *branchesToTurnOn)
	{
	  std::string nameStar = name + ".*"; // necessary because of the splitting
	  if (debug)
	    {std::cout << "Turning on branch \"" << nameStar << "\"" << std::endl;}
	  t->SetBranchStatus(nameStar.c_str(), true);
	}
    }
  
  t->SetBranchAddress("Beam.",&beam);
}
