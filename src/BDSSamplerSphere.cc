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
#include "BDSSamplerSphere.hh"
#include "BDSSDManager.hh"
#include "BDSSDSamplerSphere.hh"

#include "G4LogicalVolume.hh"
#include "G4Sphere.hh"
#include "G4String.hh"
#include "G4Types.hh"

#include "CLHEP/Units/SystemOfUnits.h"


BDSSamplerSphere::BDSSamplerSphere(const G4String& nameIn,
                                   G4double        radiusIn,
                                   G4double        startAnglePhi,
                                   G4double        sweepAnglePhi,
                                   G4double        startAngleTheta,
                                   G4double        sweepAngleTheta,
                                   G4int           filterSetIDIn):
  BDSSampler(nameIn, filterSetIDIn)
{
  G4double thickness = 1e-6 * radiusIn;
  containerSolid = new G4Sphere(nameIn + "_solid",      // name
                                radiusIn,               // inner radius
                                radiusIn + thickness,   // outer radius
                                startAnglePhi,
                                sweepAnglePhi,
                                startAngleTheta,
                                sweepAngleTheta);

  SetExtent(BDSExtent(radiusIn, radiusIn, radiusIn));
  CommonConstruction();
}

void BDSSamplerSphere::SetSensitivity()
{
  auto sdMan = BDSSDManager::Instance();
  BDSSDSamplerSphere* sd = filterSetID > -1 ? sdMan->SamplerSphereWithFilter(filterSetID) : sdMan->SamplerSphere();
  containerLogicalVolume->SetSensitiveDetector(sd);
}