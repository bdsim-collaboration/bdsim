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
#ifndef BDSFIELDMAGOCTUPOLE_H
#define BDSFIELDMAGOCTUPOLE_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic strength in an octupole.
 * 
 * The magnetic field is calculated from the octuupole strength parameter
 * "k3" and a design rigidity (brho).
 */

class BDSFieldMagOctupole: public BDSFieldMag
{
public:
  BDSFieldMagOctupole(BDSMagnetStrength const* strength,
		      G4double          const  brho);

  virtual ~BDSFieldMagOctupole(){;}

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector &position,
				 const G4double       t = 0) const;
  
private:
  /// Private default constructor to force use of supplied constructor.
  BDSFieldMagOctupole();

  /// 3rd derivative of the magnetic field
  G4double bTriplePrime;

  /// bTriplePrime / 3! cached for simplicity
  G4double bTPNormed; 
};

#endif
