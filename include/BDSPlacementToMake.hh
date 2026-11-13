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
#ifndef BDSPLACEMENTTOMAKE_H
#define BDSPLACEMENTTOMAKE_H
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4Types.hh"

class G4LogicalVolume;

/**
 * @brief A cache of a components to make a placement.
 *
 * @author Laurie Nevay
 */

class BDSPlacementToMake
{
public:
  BDSPlacementToMake() = delete;
  BDSPlacementToMake(const G4Transform3D& transformIn,
		     G4LogicalVolume*     lvToPlaceIn,
		     const G4String&      nameIn,
		     G4int                copyNumberIn = 0):
    transform(transformIn),
    lvToPlace(lvToPlaceIn),
    name(nameIn),
    copyNumber(copyNumberIn)
  {;}

  /// This class doesn't own anything so only delete full objects it has a copy of.
  ~BDSPlacementToMake(){;}

  G4Transform3D     transform;
  G4LogicalVolume*  lvToPlace;
  G4String          name;
  G4int             copyNumber;  
};

#endif
