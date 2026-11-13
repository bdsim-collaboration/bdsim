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
#include "BDSFieldMagUndulator.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSFieldMagUndulator::BDSFieldMagUndulator(BDSMagnetStrength const* strength,
					   G4double beamPipeRadiusIn):
  beamPipeRadius(beamPipeRadiusIn)
{
  wavenumber = (CLHEP::twopi)/(*strength)["length"];
  B = (*strength)["field"];// / CLHEP::tesla;
  finiteStrength = BDS::IsFinite(B);
  limit = 3 * B; // limit for validity of this
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B = " << B << G4endl;
  G4cout << __METHOD_NAME__ << "B_limit = " << limit << G4endl;
#endif
}

G4ThreeVector BDSFieldMagUndulator::GetField(const G4ThreeVector& position,
					     const G4double       /*t*/) const
{
  G4ThreeVector field;

  if (std::abs(position.x()) > beamPipeRadius || std::abs(position.y()) > beamPipeRadius)
    {return G4ThreeVector(0,0,0);}

  G4double yFactor = std::cosh(position.y() * wavenumber);
  if (yFactor > 5)
    {yFactor = 5;}
  G4double zFactor = std::sinh(position.y() * wavenumber);
  if (zFactor > 5)
    {zFactor = 5;}

  field[0] = 0;
  field[1] =  B * std::cos(position.z() * wavenumber) * yFactor;
  field[2] = -B * std::sin(position.z() * wavenumber) * zFactor;
  
  // hyperbolic functions can tend to infinity if we happen to query a
  // very large point limits -> a) the max magnitude and b) no nans
  if (field[1] > limit || std::isnan(field[1]))
    {field[1] = limit;}
  if (field[2] > limit || std::isnan(field[2]))
    {field[2] = limit;}

  return field;
}
