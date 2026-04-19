/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSFieldMagMuonSpoiler.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include <cmath>

BDSFieldMagMuonSpoiler::BDSFieldMagMuonSpoiler(BDSMagnetStrength const* strength,
					       G4double           const /*brho*/)
{
  bField = (*strength)["field"];
  finiteStrength = BDS::IsFinite(bField);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B = " << bField << G4endl;
#endif
}

G4ThreeVector BDSFieldMagMuonSpoiler::GetField(const G4ThreeVector &position,
					       const G4double       /*t*/) const
{ 
  G4double x = position.x();
  G4double y = position.y();
  G4double r = std::hypot(std::abs(x), std::abs(y));

  G4ThreeVector localField;
  G4double cyor = y/r;
  if (std::isnan(cyor))
    {cyor = 0;}
  G4double cxor = -x/r;
  if (std::isnan(cxor))
    {cxor = 0;}
  localField[0] = cyor * bField;
  localField[1] = cxor * bField;
  localField[2] = 0;

  return localField;
}
