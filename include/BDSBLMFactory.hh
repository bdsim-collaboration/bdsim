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
#ifndef BDSBLMFACTORY_H
#define BDSBLMFACTORY_H

#include "globals.hh"

class BDSBLM;
class BDSExtent;
class G4VSensitiveDetector;

namespace GMAD
{
  class BLMPlacement;
}

/**
 * @brief Factory for building BLMs. 
 * 
 * @author Laurie Nevay
 */

class BDSBLMFactory
{
public:
  BDSBLMFactory();
  ~BDSBLMFactory();

  BDSBLM* CreateBLM(const GMAD::BLMPlacement& bp,
                    G4VSensitiveDetector* sd);

  /// Main public interface to the factory. Each of the 1,2,3,4 blm parameters
  /// has a different meaning depending on the geometry chosen in geometryType.
  BDSBLM* CreateBLM(const G4String& name,
		   const G4String& geometryFile,
		   const G4String& geometryType,
		   const G4String& material,
		   G4double blm1,
		   G4double blm2,
		   G4double blm3,
		   G4double blm4,
		   G4VSensitiveDetector* sd,
		   const G4String& bias = "");

private:
  void PositiveFinite(G4double value,
  const G4String& parameterName,
  const G4String& blmName) const;

  /// Build the geometry for a cylinder.
  BDSBLM* BuildBLMCylinder(const G4String& name,
			   const G4String& material,
			   G4double        halfLength,
			   G4double        radius);

  /// Build the geometry for a cube.
  BDSBLM* BuildBLMCube(const G4String& name,
		       const G4String& material,
		       G4double        halfLengthX,
		       G4double        halfLengthY,
		       G4double        halfLengthZ);

  /// Build the geometry for a sphere.
  BDSBLM* BuildBLMSphere(const G4String& name,
			 const G4String& material,
			 G4double        radius);

  /// Construct the logical volume and final BDSBLM object from a shape
  /// and extent.
  BDSBLM* CommonConstruction(const G4String&  name,
			     const G4String&  material,
			     G4VSolid*        shape,
			     const BDSExtent& extent);
};

#endif
