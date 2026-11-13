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
#ifndef BDSFIELDMAGDIPOLE_H
#define BDSFIELDMAGDIPOLE_H

#include "BDSFieldMag.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

class BDSMagnetStrength;

/**
 * @brief A uniform dipole field.
 * 
 * This class represents a uniform dipole field. This may optionally be constructed
 * with a unit vector to denote a different direction. This uses the parameters
 * "angle" and "length" (the magnetic field length) from the input 
 * BDSMagnetStrength instance. MADX uses angle rather than k0, so we follow that
 * convention.
 */

class BDSFieldMagDipole: public BDSFieldMag
{
public:
  /// Initialise with field of correct magnitude and direction.
  BDSFieldMagDipole(const G4ThreeVector& field);

  /// This constructor uses bx, by and bz from the strength to form a unit vector,
  /// however if these are all 0 and 'field' is finite (in strength), then a field
  /// along unit y with strength 'field' is provided.
  BDSFieldMagDipole(const BDSMagnetStrength* strength);

  virtual ~BDSFieldMagDipole(){;}

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const G4double       t = 0) const;

  inline G4ThreeVector FieldValue() const {return localField;}

private:
  /// Private default constructor to ensure use of supplied constructor
  BDSFieldMagDipole();
  
  /// The vector of the field. This constructed in local coordinates and the first time
  /// the field is queried, the auxiliary navigator (through inheritance) is used to
  /// transform the vector to global coordinates.
  G4ThreeVector localField;
  
};

#endif
