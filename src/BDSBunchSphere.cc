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
#include "BDSBunchSphere.hh"
#include "BDSParticleCoordsFull.hh"

#include "globals.hh"
#include "G4RandomDirection.hh"

BDSBunchSphere::BDSBunchSphere():
  BDSBunch("sphere")
{;}

BDSParticleCoordsFull BDSBunchSphere::GetNextParticleLocal()
{
  G4ThreeVector momentumUnitVector = G4RandomDirection();
  G4double xp = momentumUnitVector.x();
  G4double yp = momentumUnitVector.y();
  G4double zp = momentumUnitVector.z();  
  return BDSParticleCoordsFull(X0,Y0,Z0,xp,yp,zp,T0,S0,E0,/*weight=*/1.0);
}
