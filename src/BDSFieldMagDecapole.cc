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
#include "BDSFieldMagDecapole.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSFieldMagDecapole::BDSFieldMagDecapole(BDSMagnetStrength const* strength,
					 G4double          const  brho)
{
  // B'''' = d^4By/dx^4 = Brho * (1/Brho d^4By/dx^4) = Brho * k4
  bQuadruplePrime = brho * (*strength)["k4"]  / (CLHEP::m3*CLHEP::m2);
  bQPNormed       = bQuadruplePrime / 24.;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B'''' = " << bQuadruplePrime << G4endl;
#endif
}

G4ThreeVector BDSFieldMagDecapole::GetField(const G4ThreeVector &position,
					    const G4double       /*t*/) const
{
  // B_x = 4xy(x^2-y^2) * ( B''''/4!)
  // B_y = (x^4 - 6x^2y^2 + y^4) * ( B''''/4!)
  // B_z = 0
  
  //shortcuts
  G4double x = position.x();
  G4double y = position.y();

  G4ThreeVector localField;
  localField[0] = 4 * x * y * (std::pow(x,2)-std::pow(y,2)) * bQPNormed;
  localField[1] = (std::pow(x,4) - 6 * std::pow(x,2) * std::pow(y,2) + std::pow(y,4)) * bQPNormed;
  localField[2] = 0;

  return localField;
}
