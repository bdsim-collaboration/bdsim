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

#include "BDSIntegratorParallelTransport.hh"
#include "BDSStep.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

BDSIntegratorParallelTransport::BDSIntegratorParallelTransport(G4Mag_EqRhs* eqOfMIn):
  BDSIntegratorMag(eqOfMIn, 6)
{;}

void BDSIntegratorParallelTransport::Stepper(const G4double yIn[],
                                             const G4double /*dydx*/[],
                                             const G4double h,
                                             G4double       yOut[],
                                             G4double       yErr[])
{
  G4ThreeVector pos = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector mom = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  G4double      momMag = mom.mag();
  
  BDSStep       localPosMom  = ConvertToLocal(pos, mom, h, false);
  G4ThreeVector localPos     = localPosMom.PreStepPoint();
  G4ThreeVector localMom     = localPosMom.PostStepPoint();
  G4ThreeVector localMomUnit = localMom.unit();
  
  G4double x0 = localPos.x();
  G4double y0 = localPos.y();
  G4double z0 = localPos.z();
  
  G4double xp0 = localMomUnit.x();
  G4double yp0 = localMomUnit.y();
  G4double zp0 = localMomUnit.z();

  // only proceed with parallal transport if particle is paraxial
  // judged by forward momentum > 1-limit and |transverse| < limit (default limit=0.1)
  if (zp0 < (1.0-backupStepperMomLimit) || std::abs(xp0) > backupStepperMomLimit || std::abs(yp0) > backupStepperMomLimit)
	{
	  AdvanceDriftMag(yIn, h, yOut, yErr);
	  SetDistChord(0);
	  return;
	}

  G4double x1 = x0;
  G4double y1 = y0;
  G4double z1 = z0+h;
  
  for (G4int i = 0; i < 3; i++)
    {
      yErr[i]   = 0;
      yErr[i+3] = 0;
    }
  
  G4ThreeVector localPosOut     = G4ThreeVector(x1, y1, z1);
  G4ThreeVector localMomUnitOut = G4ThreeVector(xp0, yp0, zp0);
  ConvertToGlobal(localPosOut, localMomUnitOut, yOut, yErr, momMag);
}
