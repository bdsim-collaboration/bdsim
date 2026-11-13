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
#include "BDSFieldEM.hh"
#include "BDSFieldEMGlobal.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <utility>

BDSFieldEMGlobal::BDSFieldEMGlobal(BDSFieldEM* fieldIn):
  field(fieldIn)
{
  finiteStrength = field->FiniteStrength();
}

BDSFieldEMGlobal::~BDSFieldEMGlobal()
{
  delete field;
}

std::pair<G4ThreeVector,G4ThreeVector> BDSFieldEMGlobal::GetFieldTransformed(const G4ThreeVector& position,
									     const G4double       t) const
{
  if (!finiteStrength)
    {return std::make_pair(G4ThreeVector(),G4ThreeVector());} // quicker than query
  else
    {return GetField(position, t);}
}

std::pair<G4ThreeVector,G4ThreeVector> BDSFieldEMGlobal::GetField(const G4ThreeVector& position,
								  const G4double       t) const
{
  G4ThreeVector localPosition = ConvertToLocal(position);
  auto          localField    = field->GetFieldTransformed(localPosition, t);
  auto          globalField   = ConvertAxisToGlobal(localField);
  return globalField;
}
