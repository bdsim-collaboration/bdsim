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
#include "BDSFieldMagDipoleHardEdgeMuonCooler.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <cmath>

BDSFieldMagDipoleHardEdgeMuonCooler::BDSFieldMagDipoleHardEdgeMuonCooler(G4double strength,                                             
                                             G4double apertureRadius,
                                             G4double coilLength
                                             ):
  D(2*apertureRadius),
  halfLength(0.5*coilLength),
  B0(strength)
  {;}

G4ThreeVector BDSFieldMagDipoleHardEdgeMuonCooler::GetField(const G4ThreeVector& position,
					                          const G4double       /*t*/) const
{
  G4double z = position.z();
  G4double rho = position.perp();

  G4double By = 0;
    
  if (rho > D*0.5 || std::abs(z) > halfLength)  
    { return G4ThreeVector();}
  else
    { By = B0;}

  G4ThreeVector result = G4ThreeVector(0,By,0);
  return result;
}


