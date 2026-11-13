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
#ifndef RUN_H
#define RUN_H

#include "TChain.h"

#include "RebdsimTypes.hh"

class BDSOutputROOTEventHistograms;
class BDSOutputROOTEventRunInfo;

/**
 * @brief Class to overlay a run from a ROOT file with.
 * 
 * @author Stewart Boogert
 */

class Run
{
public:
  Run();
  explicit Run(bool debugIn);
  Run(bool debugIn,
      int  dataVersionIn);
  virtual ~Run();

  /// Map the ROOT file to members in this class.
  void SetBranchAddress(TTree* t,
			bool                      allBranchesOn    = true,
			const RBDS::VectorString* branchesToTurnOn = nullptr);

  // These variables are named to match the naems in the output file we would find.
  /// @{ Member that ROOT can map file data to locally.
  BDSOutputROOTEventRunInfo*    Summary;
  BDSOutputROOTEventHistograms* Histos;
  /// @}

  BDSOutputROOTEventRunInfo*    Info; ///< For backwards compatibiliy.
  
private:  
  bool debug;
  int  dataVersion;
  
  ClassDef(Run, 2);
};

#endif
