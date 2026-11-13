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
#include "BDSFieldEM.hh"
#include "BDSFieldEMGlobalPlacement.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <utility>

BDSFieldEMGlobalPlacement::BDSFieldEMGlobalPlacement(BDSFieldEM* fieldIn):
  field(fieldIn)
{
  finiteStrength = field->FiniteStrength();
}

BDSFieldEMGlobalPlacement::~BDSFieldEMGlobalPlacement()
{
  delete field;
}

std::pair<G4ThreeVector,G4ThreeVector> BDSFieldEMGlobalPlacement::GetFieldTransformed(const G4ThreeVector& position,
							     const G4double       t) const
{
  if (!finiteStrength)
    {return std::make_pair(G4ThreeVector(),G4ThreeVector());} // quicker than query
  else
    {return GetField(position, t);}
}

std::pair<G4ThreeVector,G4ThreeVector> BDSFieldEMGlobalPlacement::GetField(const G4ThreeVector& position,
								  const G4double       t) const
{
  G4bool foundAPlacementVolume = false;
  G4ThreeVector localPosition = ConvertToLocal(position, foundAPlacementVolume);
  if (!foundAPlacementVolume)
    {return std::make_pair(G4ThreeVector(),G4ThreeVector());} // force to 0 outside range of coordinate transforms
  auto          localField    = field->GetFieldTransformed(localPosition, t);
  auto          globalField   = ConvertAxisToGlobal(localField);
  return globalField;
}
