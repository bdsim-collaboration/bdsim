/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#include "BDSFieldMagUndulator.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

BDSFieldMagUndulator::BDSFieldMagUndulator(BDSMagnetStrength const* strength)
{
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  peroid = (*strength)["length"];
  B = (*strength)["field"] / CLHEP::tesla;
  finiteStrength = BDS::IsFinite(B);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B' = " << bPrime << G4endl;
#endif
}

G4ThreeVector BDSFieldMagUndulator::GetField(const G4ThreeVector &position,
					      const G4double       /*t*/) const
{

  G4ThreeVector field;
  field[0] = 0;
  field[1] = B * std::cos(position.z() * ((2*CLHEP::pi)/peroid));
  field[2] = 0;

  return field;
}