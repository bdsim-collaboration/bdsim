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
#include "BDSFieldMagDetectorSolenoid.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include <cmath>

BDSFieldMagDetectorSolenoid::BDSFieldMagDetectorSolenoid(G4double BIn,
							 G4double BOut,
							 G4double radiusIn,
							 G4double radiusOut,
							 G4double zMin,
							 G4double zMax):
  itsBIn(BIn),
  itsBOut(BOut),
  itsRadiusIn(radiusIn),
  itsRadiusOut(radiusOut),
  itsZMin(zMin),
  itsZMax(zMax)
{;}

G4ThreeVector BDSFieldMagDetectorSolenoid::GetField(const G4ThreeVector &position,
						    const G4double       /*t*/) const
{
  G4ThreeVector result(0,0,0);
  G4double zField   = 0;
  G4double localRad = std::hypot(position.y(),position.x());

  if( (position.z() > itsZMin) && (position.z() < itsZMax) )
    {
      if(localRad<itsRadiusIn)
	{zField = itsBIn;}
      else if(localRad<itsRadiusOut)
	{zField = itsBOut;}
      else
	{zField=0;}
    }
  else
    {zField=0;}

  result[2] = zField;
  return result;
}



