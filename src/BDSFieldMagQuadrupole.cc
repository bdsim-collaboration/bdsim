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
#include "BDSFieldMagQuadrupole.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

BDSFieldMagQuadrupole::BDSFieldMagQuadrupole(BDSMagnetStrength const* strength,
					     G4double          const  brho)
{
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  bPrime = brho * (*strength)["k1"] / CLHEP::m2;
  finiteStrength = BDS::IsFinite(bPrime);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B' = " << bPrime << G4endl;
#endif
}

G4ThreeVector BDSFieldMagQuadrupole::GetField(const G4ThreeVector &position,
					      const G4double       /*t*/) const
{  
  G4ThreeVector field;
  field[0] = position.y() * bPrime; // B_x = B' * y;
  field[1] = position.x() * bPrime; // B_y = B' * x;
  field[2] = 0;                     // B_z = 0

  return field;
}
