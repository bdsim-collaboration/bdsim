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
/* BDSIM code.

*/

#include "globals.hh"
#include "BDSField.hh"

BDSField::BDSField():rotation(NULL)
{
}

BDSField::~BDSField()
{
}

G4bool   BDSField::DoesFieldChangeEnergy() const
{
  //default
  return false;
}



void BDSField::GetFieldValue(const G4double[4],G4double *Bfield ) const
{
  // default 0

  Bfield[0] = 0;
  Bfield[1] = 0;
  Bfield[2] = 0;

  Bfield[3] = 0;
  Bfield[4] = 0;
  Bfield[5] = 0;

}

void BDSField::Prepare(G4VPhysicalVolume*)
{
  // default - do nothing
}

void BDSField::SetOriginRotation(G4RotationMatrix* rot)
{
  rotation = rot;
}

void BDSField::SetOriginTranslation(G4ThreeVector trans)
{
  translation = trans;
}

G4RotationMatrix BDSField::Rotation() const{
  return *rotation;
}
