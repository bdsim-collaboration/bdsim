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
#ifndef BDSFIELDEMINTERPOLATED1D_H
#define BDSFIELDEMINTERPOLATED1D_H
#include "BDSFieldEMInterpolated.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4Types.hh"

#include <utility>

class BDSInterpolator1D;

/**
 * @brief A 1D field from an interpolated array with any interpolation.
 *
 * Owns interpolators.
 *
 * This provides a simple interface for em fields to use a 1D
 * interpolator irrespective of which type of interpolator it is.
 *
 * @author Laurie Nevay
 */

class BDSFieldEMInterpolated1D: public BDSFieldEMInterpolated
{
public:
  BDSFieldEMInterpolated1D() = delete;
  BDSFieldEMInterpolated1D(BDSInterpolator1D*   eInterpolatorIn,
			   BDSInterpolator1D*   bInterpolatorIn,
  			   const G4Transform3D& offset     = G4Transform3D::Identity,
			   G4double             eScalingIn = 1.0,
			   G4double             bScalingIn = 1.0);

  virtual ~BDSFieldEMInterpolated1D();

  /// Return the interpolated field value at a given point.
  virtual std::pair<G4ThreeVector,G4ThreeVector> GetField(const G4ThreeVector& position,
							  const G4double       t = 0) const;

  inline const BDSInterpolator1D* EInterpolator() const {return eInterpolator;}
  inline const BDSInterpolator1D* BInterpolator() const {return bInterpolator;}

private:
  BDSInterpolator1D* eInterpolator; ///< E Inteprolator the field is based on.
  BDSInterpolator1D* bInterpolator; ///< B Interpolator the field is based on.
  const G4int  eDimensionIndex;     ///< E Integer index to dimension to use.
  const G4bool eTime;               ///< E Cache of whether to use time coordinate.
  const G4int  bDimensionIndex;     ///< B Integer index to dimension to use.
  const G4bool bTime;               ///< B Cache of whether to use time coordinate.
};

#endif
