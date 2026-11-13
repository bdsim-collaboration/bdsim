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
#ifndef BDSFIELDMAGDETECTORSOLENOID_H
#define BDSFIELDMAGDETECTORSOLENOID_H

#include "globals.hh" // geant4 types / globals

#include "BDSFieldMag.hh"

/**
 * @brief Field with different inner and an outer solenoid field. 
 */

class BDSFieldMagDetectorSolenoid: public BDSFieldMag
{
public: 
  BDSFieldMagDetectorSolenoid(G4double BIn,
			      G4double Bout,
			      G4double radiusIn,
			      G4double radiusOut,
			      G4double zMin,
			      G4double zMax);
  
  virtual ~BDSFieldMagDetectorSolenoid(){;}

  /// Get the magnetic field vector in local coordinates.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const G4double       t = 0) const;
  
private:
  G4double itsBIn;
  G4double itsBOut;
  G4double itsRadiusIn;
  G4double itsRadiusOut;
  G4double itsZMin;
  G4double itsZMax;
};

#endif
