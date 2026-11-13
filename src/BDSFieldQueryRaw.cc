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
#include "BDSFieldQueryInfo.hh"
#include "BDSFieldQueryRaw.hh"
#include "BDSWarning.hh"

#include "G4Field.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"

BDSFieldQueryRaw::BDSFieldQueryRaw():
  field(nullptr)
{;}

BDSFieldQueryRaw::~BDSFieldQueryRaw()
{;}

void BDSFieldQueryRaw::QueryFieldRaw(G4Field* fieldIn,
				     const BDSFieldQueryInfo* query)
{
  field = fieldIn;
  QueryField(query);
}

void BDSFieldQueryRaw::GetFieldValue(const G4ThreeVector& globalXYZ,
				     const G4ThreeVector& /*globalDirection*/,
				     G4double tGlobal,
				     G4double fieldValue[6])
{
  for (G4int i = 0; i < 6; i++)
    {fieldValue[i] = 0;}
  if (!field)
    {return;}
  G4double position[4] = {globalXYZ.x(), globalXYZ.y(),globalXYZ.z(), tGlobal};
  field->GetFieldValue(position, fieldValue);
}

void BDSFieldQueryRaw::CheckIfFieldObjectSpecified(const BDSFieldQueryInfo* query) const
{
  if (query->fieldObject.empty())
  {
    G4String msg = "\"fieldObject\" variable is empty in query definition \"" + query->name;
    msg += "\" - it must have a value";
    BDS::Warning(msg);
  }
}