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
#ifndef ANALYSISOPTIONS_H
#define ANALYSISOPTIONS_H

#include "Rtypes.h" // for classdef

#include "RebdsimTypes.hh"

class BDSOutputROOTEventOptions;
class TTree;

/**
 * @brief Options loader.
 *
 * @author Stuart Walker.
 */

class Options
{
public:
  Options();
  Options(bool debugIn);
  virtual ~Options();

  /// Set the branch addresses to address the contents of the file.
  void SetBranchAddress(TTree* t,
			bool                      allBranchesOn    = true,
			const RBDS::VectorString* branchesToTurnOn = nullptr);

  /// Member that ROOT can map file data to locally.
  BDSOutputROOTEventOptions* options;

private:
  bool debug;
  
  ClassDef(Options,1);
};

#endif
