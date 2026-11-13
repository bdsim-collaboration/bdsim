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
#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include "TH1D.h"
#include "TVector3.h"
#include "TRotation.h"


#include "Event.hh"

#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventModel.hh"
#include "BDSOutputROOTEventHistograms.hh"

/// Utility function to convert local to globa coordinates given a rotation and translation.
TVector3 LocalToGlobal(const TVector3 &vLocal, const TRotation &r, const TVector3 &d);

/// Utility function to get all the 1D run histograms from a ROOT file.
std::vector<TH1D*>& GetRun1DHistograms(TString fileName);

/// Utility function to get the options used for the simulation from a ROOT file.
BDSOutputROOTEventOptions* GetOptions(TString fileName);

/// Utility funciton to get the GDML model from a ROOT file.
BDSOutputROOTEventModel* GetModel(TString fileName);

/// Utility function to add a file to a chain and remap the events to the given event instance.
void GetEvent(TString fileName, TChain *c, Event *e);

#endif
