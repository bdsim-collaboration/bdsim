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
#ifndef BDSFIELDSKEW_H
#define BDSFIELDSKEW_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

/**
 * @brief A wrapper class for BDSFieldMag that rotates it.
 * 
 * This is a wrapper class for a BDSFieldMag instance that rotates it by applying
 * a rotation to the coordinates before querying the wrapped field object.
 * This is intended to implement skew fields but any arbritary rotation can
 * be applied, although this should be considered carefully.
 * 
 * This class does not own the field it wraps.
 *
 * @author Laurie Nevay
 */

class BDSFieldMagSkew: public BDSFieldMag
{
public:
  /// Wrap a field and rotate it by angle (rad) in the x-y plane (about z axis).
  BDSFieldMagSkew(BDSFieldMag* fieldIn, G4double angle = 0);

  virtual ~BDSFieldMagSkew();

  /// Get the field - local coordinates, and rotated.
  virtual G4ThreeVector GetField(const G4ThreeVector &position,
				 const G4double       t = 0) const;
  
private:
  /// Private default constructor to force use of supplied ones.
  BDSFieldMagSkew() = delete;

  /// The nominal non-skewed field.  Just storage of a pointer to it. This class
  /// doesn't own it.
  BDSFieldMag* const field;

  /// The rotation matrix used to rotate the coordinates.
  G4RotationMatrix* rotation;

  /// The opposite rotation matrix used to transform the resultant field vector.
  G4RotationMatrix* antiRotation;
};

#endif
