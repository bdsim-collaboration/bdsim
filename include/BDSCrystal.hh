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
#ifndef BDSCRYSTAL_H
#define BDSCRYSTAL_H

#include "BDSGeometryComponent.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

class BDSCrystalInfo;
class G4LogicalVolume;
class G4VSolid;

/**
 * @brief An object for both a crystal from a factory.
 * 
 * Mostly BDSGeometryComponent.
 * 
 * @author Laurie Nevay
 */

class BDSCrystal: public BDSGeometryComponent
{
public:
  BDSCrystal(const BDSCrystalInfo* recipeIn,
	     G4VSolid*             containerSolidIn,
	     G4LogicalVolume*      containerLVIn,
	     const BDSExtent&      extent,
	     const G4ThreeVector&  placementOffsetIn   = G4ThreeVector(0,0,0),
	     G4RotationMatrix*     placementRotationIn = nullptr);
  virtual ~BDSCrystal();
  
  G4double        bendingRadiusHorizontal;
  BDSCrystalInfo* recipe;
};

#endif
