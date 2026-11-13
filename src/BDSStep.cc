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
#include "BDSStep.hh"


BDSStep::BDSStep():
  preStepPoint(G4ThreeVector()),
  postStepPoint(G4ThreeVector()),
  volumeForTransform(nullptr)
{;}

BDSStep::BDSStep(G4ThreeVector preStepPointIn,
		 G4ThreeVector postStepPointIn,
		 G4VPhysicalVolume* volumeForTransformIn):
  preStepPoint(preStepPointIn),
  postStepPoint(postStepPointIn),
  volumeForTransform(volumeForTransformIn)
{;}

BDSStep::~BDSStep()
{;}

BDSStep BDSStep::rotateZ(const G4double& angle)
{
  G4ThreeVector preR = preStepPoint.rotateZ(angle);
  G4ThreeVector posR = postStepPoint.rotateZ(angle);
  return BDSStep(preR, posR, volumeForTransform);
}
