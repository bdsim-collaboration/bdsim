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
#include "BDSFieldMagDipole.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

BDSFieldMagDipole::BDSFieldMagDipole(const G4ThreeVector& field):
  localField(field)
{
  finiteStrength = BDS::IsFinite(localField.mag());
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B (local) = " << localField << G4endl;
#endif
}

BDSFieldMagDipole::BDSFieldMagDipole(const BDSMagnetStrength* strength)
{
  G4double field = (*strength)["field"];
  G4double bx    = (*strength)["bx"];
  G4double by    = (*strength)["by"];
  G4double bz    = (*strength)["bz"];
  
  if (!BDS::IsFinite(bx) && !BDS::IsFinite(by) && !BDS::IsFinite(bz) && BDS::IsFinite(field))
    {localField = G4ThreeVector(0,1,0) * field;}
  else
    {localField = G4ThreeVector(bx,by,bz).unit() * field;}

  finiteStrength = BDS::IsFinite(localField.mag());
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B (local) (T) = " << localField / CLHEP::tesla << G4endl;
#endif
}

G4ThreeVector BDSFieldMagDipole::GetField(const G4ThreeVector& /*position*/,
					  const G4double       /*t*/) const
{
  return localField;
}
