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
#ifndef BDSFACTORYBASE_H
#define BDSFACTORYBASE_H

#include "globals.hh"         // geant4 globals / types
#include "G4RotationMatrix.hh"

#include <set>

class G4LogicalVolume;
class G4Material;
class G4UserLimits;
class G4VisAttributes;
class G4VPhysicalVolume;
class G4VSolid;

/**
 * @brief Common temporary storage for all factories no matter what geometry.
 * 
 * @author Laurie Nevay
 */

class BDSFactoryBase
{
public:
  BDSFactoryBase();
  virtual ~BDSFactoryBase(){;}
  
  /// Empty containers for next use - factories are never deleted so can't rely on scope
  virtual void FactoryBaseCleanUp();
 
protected:
  /// @{ Cache of global constants variable.
  G4double           lengthSafety;
  G4double           lengthSafetyLarge;
  G4bool             checkOverlaps;
  G4bool             visDebug;
  G4int              nSegmentsPerCircle;
  G4VisAttributes*   containerVisAttr;
  G4UserLimits*      defaultUserLimits;
  /// @}

  std::set<G4LogicalVolume*>   allLogicalVolumes;
  std::set<G4VPhysicalVolume*> allPhysicalVolumes;
  std::set<G4RotationMatrix*>  allRotationMatrices;
  std::set<G4UserLimits*>      allUserLimits;
  std::set<G4VSolid*>          allSolids;
  std::set<G4VisAttributes*>   allVisAttributes;
};

#endif
