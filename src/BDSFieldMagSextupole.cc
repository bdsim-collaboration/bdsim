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
#include "BDSFieldMagSextupole.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSFieldMagSextupole::BDSFieldMagSextupole(BDSMagnetStrength const* strength,
					   G4double          const   brho)
{
  // B'' = d^2By/dx^2 = Brho * (1/Brho d^2By/dx^2) = Brho * k2
  bDoublePrime     = brho * (*strength)["k2"] / CLHEP::m3;
  halfBDoublePrime = bDoublePrime*0.5;
  finiteStrength = BDS::IsFinite(bDoublePrime);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B'' = " << bDoublePrime << G4endl;
#endif
}

G4ThreeVector BDSFieldMagSextupole::GetField(const G4ThreeVector& position,
					     const G4double       /*t*/) const
{ 
  // B_x = 2*x*y * (B''/2!)
  // B_y = (x^2 - y^2) * (B''/2!)
  // B_z = 0

  G4ThreeVector localField;
  localField[0] = position.x() * position.y() * bDoublePrime;
  localField[1] = (std::pow(position.x(),2) - std::pow(position.y(),2)) * halfBDoublePrime;
  localField[2] = 0;
  
  return localField;
}
