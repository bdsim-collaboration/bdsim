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
#include "BDSFieldMagOctupole.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSFieldMagOctupole::BDSFieldMagOctupole(BDSMagnetStrength const* strength,
					 G4double          const   brho)
{
  // B''' = d^3By/dx^3 = Brho * (1/Brho d^3By/dx^3) = Brho * k3
  bTriplePrime = brho * (*strength)["k3"]  / (CLHEP::m3*CLHEP::m);
  bTPNormed    = bTriplePrime / 6.;
  finiteStrength = BDS::IsFinite(bTriplePrime);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B''' = " << bTriplePrime << G4endl;
#endif
}

G4ThreeVector BDSFieldMagOctupole::GetField(const G4ThreeVector &position,
					    const G4double       /*t*/) const
{
  // B_x = (3x^2y - y^3) * (B'''/3!)
  // B_y = (x^3-3xy^2) * (B'''/3!)
  // B_z = 0

  //shortcuts
  G4double x = position.x();
  G4double y = position.y();
  
  G4ThreeVector localField;
  localField[0] = (3 * std::pow(x,2) * y - std::pow(y,3)) * bTPNormed;
  localField[1] = (std::pow(x,3) - 3* x * std::pow(y,2)) * bTPNormed;
  localField[2] = 0;

  return localField;
}
