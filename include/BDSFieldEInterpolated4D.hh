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
#ifndef BDSFIELDEINTERPOLATED4D_H
#define BDSFIELDEINTERPOLATED4D_H
#include "BDSFieldEInterpolated.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4Types.hh"

class BDSInterpolator4D;

/**
 * @brief A 4D field from an interpolated array with any interpolation.
 *
 * Owns interpolator.
 *
 * This provides a simple interface for electric fields to use a 4D
 * interpolator irrespective of which type of interpolator it is.
 *
 * @author Laurie Nevay
 */

class BDSFieldEInterpolated4D: public BDSFieldEInterpolated
{
public:
  BDSFieldEInterpolated4D() = delete;
  explicit BDSFieldEInterpolated4D(BDSInterpolator4D*   interpolatorIn,
				   const G4Transform3D& offset     = G4Transform3D::Identity,
				   G4double             eScalingIn = 1.0);

  virtual ~BDSFieldEInterpolated4D();

  /// Return the interpolated field value at a given point.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const G4double       t) const;

  inline const BDSInterpolator4D* Interpolator() const {return interpolator;}

private:
  BDSInterpolator4D* interpolator; ///< Interpolator the field is based on.
};

#endif
