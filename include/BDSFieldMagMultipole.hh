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
#ifndef BDSFIELDMAGMULTIPOLE_H
#define BDSFIELDMAGMULTIPOLE_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include <vector>

class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic strength in a quadrupole.
 * 
 * The magnetic field is calculated from the strength parameters
 * "kn" up to a specified order and a design rigidity (brho).
 */

class BDSFieldMagMultipole: public BDSFieldMag
{
public:
  BDSFieldMagMultipole(BDSMagnetStrength const* strength,
		       G4double          const  brho,
		       G4int             const  orderIn = 12);
  
  virtual ~BDSFieldMagMultipole(){;}

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector &position,
				 const G4double       t = 0) const;

private:
  /// Private default constructor to force use of supplied constructor.
  BDSFieldMagMultipole();

  /// Order up to which field components are considered.
  G4int order;

  /// Identify which is the maximum non-zero order in either normal or
  /// skew components and only sum up to this in the field calculation.
  /// Just an optimisation to save addition.
  G4int maximumNonZeroOrder;

  /// Normal field components (normal - ie not skew) = kn * brho
  std::vector<G4double> normalComponents;

  /// Skew field components = kns * brho
  std::vector<G4double> skewComponents;
};

#endif 
