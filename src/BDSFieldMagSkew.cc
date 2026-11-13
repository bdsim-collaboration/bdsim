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
#include "BDSFieldMagSkew.hh"

#include "globals.hh"
#include "G4RotationMatrix.hh"

BDSFieldMagSkew::BDSFieldMagSkew(BDSFieldMag* fieldIn,
				 G4double     angle):
  field(fieldIn)
{
  rotation     = new G4RotationMatrix();
  antiRotation = new G4RotationMatrix();
  rotation->rotateZ(angle);
  antiRotation->rotateZ(-angle);

  finiteStrength = field->FiniteStrength();
}

BDSFieldMagSkew::~BDSFieldMagSkew()
{
  delete rotation;
  delete antiRotation;
}

G4ThreeVector BDSFieldMagSkew::GetField(const G4ThreeVector &position,
					const G4double       t) const
{
  G4ThreeVector rotatedPosition(position);
  rotatedPosition           = rotatedPosition.transform(*rotation);
  G4ThreeVector normalField = field->GetField(rotatedPosition, t);
  return (*antiRotation)*normalField;
}
