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
#ifndef BDSCAVITY_H
#define BDSCAVITY_H

#include "BDSGeometryComponent.hh"

#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

class BDSExtent;

class G4LogicalVolume;
class G4VSolid;

/**
 * @brief A holder class for an RF cavity piece of geometry.
 * 
 * @author Laurie Nevay
 */

class BDSCavity: public BDSGeometryComponent
{
public:
  BDSCavity(G4VSolid*         containerSolidIn,
	    G4LogicalVolume*  containerLVIn,
	    BDSExtent         extentIn,
	    G4LogicalVolume*  vacuumLVIn,
	    G4ThreeVector     placementOffsetIn   = G4ThreeVector(0,0,0),
	    G4RotationMatrix* placementRotationIn = nullptr);

  virtual ~BDSCavity() {;}

  /// Access the vacuum volume to set fields and limits
  inline G4LogicalVolume* GetVacuumLogicalVolume() const {return vacuumLogicalVolume;};

protected:
  G4LogicalVolume* vacuumLogicalVolume; ///< Cache of which volume is the vacuum one.
};

#endif
