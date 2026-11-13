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
#ifndef BDSFIELDMAGINTERPOLATED2D_H
#define BDSFIELDMAGINTERPOLATED2D_H

#include "BDSFieldMagInterpolated.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

class BDSInterpolator2D;

/**
 * @brief A 2D field from an interpolated array with any interpolation.
 * 
 * Owns interpolator.
 *
 * This provides a simple interface for magnetic fields to use a 2D
 * interpolator irrespective of which type of interpolator it is.
 *
 * @author Laurie Nevay
 */

class BDSFieldMagInterpolated2D: public BDSFieldMagInterpolated
{
public:
  BDSFieldMagInterpolated2D() = delete;
  explicit BDSFieldMagInterpolated2D(BDSInterpolator2D*   interpolatorIn,
				     const G4Transform3D& offset    = G4Transform3D::Identity,
				     G4double             scalingIn = 1.0);

  virtual ~BDSFieldMagInterpolated2D();

  /// Return the interpolated field value at a given point. Note this doesn't depend
  /// on the z position, only x & y.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const G4double       t = 0) const;

  inline const BDSInterpolator2D* Interpolator() const {return interpolator;}

private:
  BDSInterpolator2D* interpolator;   ///< Interpolator the field is based on.
  const G4int  firstDimensionIndex;  ///< Integer index to dimension to use.
  const G4bool firstTime;            ///< Cache of whether to use time coordinate.
  const G4int  secondDimensionIndex; ///< Integer index to dimension to use.
  const G4bool secondTime;           ///< Cache of whether to use time coordinate.
};

#endif
