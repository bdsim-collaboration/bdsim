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
#include "BDSDebug.hh"
#include "BDSFieldMagDipoleQuadrupole.hh"
#include "BDSFieldMagQuadrupole.hh"
#include "BDSFieldMagDipole.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"


BDSFieldMagDipoleQuadrupole::BDSFieldMagDipoleQuadrupole(BDSMagnetStrength const* strength,
							 G4double          const  brho):
  quad(new BDSFieldMagQuadrupole(strength, brho)),
  dipole(new BDSFieldMagDipole(strength))
{
  finiteStrength = quad->FiniteStrength() || dipole->FiniteStrength();
}

BDSFieldMagDipoleQuadrupole::~BDSFieldMagDipoleQuadrupole()
{
  delete quad;
  delete dipole;
}

G4ThreeVector BDSFieldMagDipoleQuadrupole::GetField(const G4ThreeVector &position,
						    const G4double       t) const
{  
  G4ThreeVector   quadField = quad->GetFieldTransformed(position, t);
  G4ThreeVector dipoleField = dipole->GetFieldTransformed(position, t);
  return quadField + dipoleField;
}

void BDSFieldMagDipoleQuadrupole::SetTransform(const G4Transform3D& transformIn)
{
  quad->SetTransform(transformIn);
  dipole->SetTransform(transformIn);
}
