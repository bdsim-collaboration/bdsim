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
#include "BDSIntegratorDrift.hh"

#include "G4ThreeVector.hh"

BDSIntegratorDrift::BDSIntegratorDrift()
{;}

BDSIntegratorDrift::~BDSIntegratorDrift()
{;}

void BDSIntegratorDrift::AdvanceDriftMag(const G4double yIn[],
					 const G4double h,
					 G4double       yOut[],
					 G4double       yErr[]) const
{
  G4ThreeVector unitMomentum = G4ThreeVector(yIn[3],yIn[4],yIn[5]).unit();

  AdvanceDriftMag(yIn, unitMomentum, h, yOut, yErr);
}

void BDSIntegratorDrift::AdvanceDriftMag(const G4double       yIn[],
					 const G4ThreeVector& unitMomentum,
					 const G4double       h,
					 G4double             yOut[],
					 G4double             yErr[]) const
{
  for (G4int i = 0; i < 3; i++)
    {
      yOut[i]   = yIn[i] + h * unitMomentum[i]; // update position
      yOut[i+3] = yIn[i+3];                     // momentum doesn't change
      yErr[i]   = 0;
      yErr[i+3] = 0;
    }
}
