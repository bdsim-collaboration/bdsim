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
#ifndef BDSBEAMLINESET_H
#define BDSBEAMLINESET_H

#include "BDSExtentGlobal.hh"
#include "G4ThreeVector.hh"
#include <vector>

class BDSBeamline;

/**
 * @brief Simple struct to return a beamline plus associated beam lines.
 *
 * @author L. Nevay
 */

class BDSBeamlineSet
{
public:
  BDSBeamlineSet(){;}
  ~BDSBeamlineSet(){;}

  /// Destroy objects pointed to by this instance.
  void DeleteContents();

  BDSBeamline* massWorld              = nullptr;
  BDSBeamline* curvilinearWorld       = nullptr;
  BDSBeamline* curvilinearBridgeWorld = nullptr;
  BDSBeamline* endPieces              = nullptr;

  /// Get maximum extents
  const std::vector<G4ThreeVector> GetMaximumExtentAbsolute() const;

  /// Append maximum extents to a supplied vector of extents
  void GetMaximumExtentAbsolute(std::vector<G4ThreeVector>& extents) const;

  /// Append global extents of all beamlines to supplied vector
  void GetExtentGlobals(std::vector<BDSExtentGlobal>& extents) const;
};

#endif
