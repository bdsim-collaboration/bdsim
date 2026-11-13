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
#include "BDSFieldMag.hh"
#include "BDSFieldMagGlobalPlacement.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

BDSFieldMagGlobalPlacement::BDSFieldMagGlobalPlacement(BDSFieldMag* fieldIn):
  field(fieldIn)
{
  finiteStrength = field->FiniteStrength();
}

BDSFieldMagGlobalPlacement::~BDSFieldMagGlobalPlacement()
{
  delete field;
}

G4ThreeVector BDSFieldMagGlobalPlacement::GetFieldTransformed(const G4ThreeVector& position,
							      const G4double       t) const
{
  if (!finiteStrength)
    {return G4ThreeVector();} // quicker than query
  else
    {return GetField(position, t);}
}

G4ThreeVector BDSFieldMagGlobalPlacement::GetField(const G4ThreeVector& position,
						   const G4double       t) const
{
  G4bool foundAPlacementVolume = false;
  G4ThreeVector localPosition = ConvertToLocal(position, foundAPlacementVolume);
  if (!foundAPlacementVolume)
    {return G4ThreeVector();} // force to 0 outside range of coordinate transforms
  G4ThreeVector localField    = field->GetFieldTransformed(localPosition,t);
  G4ThreeVector globalField   = ConvertAxisToGlobal(localField);
  return globalField;
}
