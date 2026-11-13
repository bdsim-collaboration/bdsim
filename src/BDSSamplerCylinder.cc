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
#include "BDSExtent.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSSDManager.hh"
#include "BDSSDSamplerCylinder.hh"
#include "BDSUtilities.hh"

#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4Types.hh"

#include "CLHEP/Units/SystemOfUnits.h"


BDSSamplerCylinder::BDSSamplerCylinder(const G4String& nameIn,
                                       G4double        radiusIn,
                                       G4double        fullLength,
                                       G4double        startAngle,
                                       G4double        sweepAngle,
                                       G4int           filterSetIDIn):
  BDSSampler(nameIn, filterSetIDIn)
{ 
  G4double thickness = 1e-6 * radiusIn;
  containerSolid = new G4Tubs(nameIn + "_solid",      // name
			      radiusIn,               // inner radius
			      radiusIn + thickness,   // outer radius
			      fullLength*0.5,         // half-length
			      startAngle,             // start angle
			      sweepAngle);            // sweep angle

  SetExtent(BDSExtent(radiusIn, radiusIn, fullLength*0.5));
  CommonConstruction();
}

BDSSamplerCylinder::BDSSamplerCylinder(const G4String& nameIn,
                                       G4double        radiusIn,
                                       G4double        fullLength,
                                       const G4ThreeVector& inputFaceNormal,
                                       const G4ThreeVector& outputFaceNormal,
                                       G4double        startAngle,
                                       G4double        sweepAngle,
                                       G4int           filterSetIDIn):
  BDSSampler(nameIn, filterSetIDIn)
{
  G4double thickness = 1e-6 * radiusIn;
  if (!BDS::IsFinite(inputFaceNormal.angle({0,0,-1}),1e-6) && !BDS::IsFinite(outputFaceNormal.angle({0,0,1}),1e-6))
    {
      containerSolid = new G4Tubs(nameIn + "_solid",      // name
				  radiusIn,               // inner radius
				  radiusIn + thickness,   // outer radius
				  fullLength*0.5,         // half-length
				  startAngle,             // start angle
				  sweepAngle);            // sweep angle
    }
  else
    {
      containerSolid = new G4CutTubs(nameIn + "_solid",      // name
				     radiusIn,               // inner radius
				     radiusIn + thickness,   // outer radius
				     fullLength * 0.5,         // half-length
				     startAngle,             // start angle
				     sweepAngle,            // sweep angle
				     inputFaceNormal,
				     outputFaceNormal);
    }
  SetExtent(BDSExtent(radiusIn, radiusIn, fullLength*0.5));
  CommonConstruction();
}

void BDSSamplerCylinder::SetSensitivity()
{
  auto sdMan = BDSSDManager::Instance();
  BDSSDSamplerCylinder* sd = filterSetID > -1 ? sdMan->SamplerCylinderWithFilter(filterSetID) : sdMan->SamplerCylinder();
  containerLogicalVolume->SetSensitiveDetector(sd);
}
