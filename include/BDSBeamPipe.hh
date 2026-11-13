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
#ifndef BDSBEAMPIPE_H
#define BDSBEAMPIPE_H

#include "BDSGeometryComponent.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include <set>

class BDSExtent;
class G4LogicalVolume;
class G4VSolid;

/**
 * @brief A holder class for a piece of beam pipe geometry.
 * 
 * This does not implement the construction of the beampipe but merely
 * holds all relevant objects and information. 
 *
 * Face normals are ensured to be unit.
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipe: public BDSGeometryComponent
{
public:
  /// constructor has BDSGeometryComponent members first,
  /// then everything extra for this derived class
  BDSBeamPipe(G4VSolid*                 containerSolidIn, ///< Container solid.
	      G4LogicalVolume*          containerLVIn,    ///< Container logical volume.
	      BDSExtent                 extentIn,
	      G4VSolid*                 containerSubtractionSolidIn,
	      G4LogicalVolume*          vacuumLVIn,
	      G4bool                    containerIsCircularIn = false, ///< Whether the container is circular.
	      G4double                  containerRadiusIn     = 0.0,
	      G4ThreeVector             inputFaceNormalIn  = G4ThreeVector(0,0,-1),
	      G4ThreeVector             outputFaceNormalIn = G4ThreeVector(0,0, 1));
  
  virtual ~BDSBeamPipe(); /// default destructor sufficient as G4 manages solids and LVs

  /// Access a solid for beampipe subtraction - note this is typically longer
  /// than the actual beampipe for unambiguous subtraction
  inline G4VSolid* GetContainerSubtractionSolid() const {return containerSubtractionSolid;}
  /// Access the vacuum volume to set fields and limits
  inline G4LogicalVolume* GetVacuumLogicalVolume() const {return vacuumLogicalVolume;};
  /// Flag to tell whether the parent volume needn't use a subtraction
  /// solid and can simply use a G4Tubs for example
  inline G4bool ContainerIsCircular() const {return containerIsCircular;}
  /// If it is circular, we need the radius
  inline G4double GetContainerRadius() const {return containerRadius;}

  /// @{ Accessor
  inline G4ThreeVector InputFaceNormal()  const {return inputFaceNormal;}
  inline G4ThreeVector OutputFaceNormal() const {return outputFaceNormal;}
  /// @}

  /// Get all volumes except the container logical volume as this is the optimal
  /// set of volumes for putting fields on.
  std::set<G4LogicalVolume*> GetVolumesForField() const;
  
protected:
  G4VSolid*        containerSubtractionSolid;
  G4LogicalVolume* vacuumLogicalVolume;
  G4bool           containerIsCircular;
  G4double         containerRadius;
  G4ThreeVector    inputFaceNormal;
  G4ThreeVector    outputFaceNormal;
};

#endif
