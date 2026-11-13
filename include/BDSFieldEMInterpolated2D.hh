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
#ifndef BDSFIELDEMINTERPOLATED2D_H
#define BDSFIELDEMINTERPOLATED2D_H
#include "BDSFieldEMInterpolated.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4Types.hh"

#include <utility>

class BDSInterpolator2D;

/**
 * @brief A 2D field from an interpolated array with any interpolation.
 *
 * Owns interpolators.
 *
 * This provides a simple interface for em fields to use a 2D
 * interpolator irrespective of which type of interpolator it is.
 *
 * @author Laurie Nevay
 */

class BDSFieldEMInterpolated2D: public BDSFieldEMInterpolated
{
public:
  BDSFieldEMInterpolated2D() = delete;
  BDSFieldEMInterpolated2D(BDSInterpolator2D*   eInterpolatorIn,
			   BDSInterpolator2D*   bInterpolatorIn,
  			   const G4Transform3D& offset     = G4Transform3D::Identity,
			   G4double             eScalingIn = 1.0,
			   G4double             bScalingIn = 1.0);

  virtual ~BDSFieldEMInterpolated2D();

  /// Return the interpolated field value at a given point.
  virtual std::pair<G4ThreeVector,G4ThreeVector> GetField(const G4ThreeVector& position,
							  const G4double       t = 0) const;

  inline const BDSInterpolator2D* EInterpolator() const {return eInterpolator;}
  inline const BDSInterpolator2D* BInterpolator() const {return bInterpolator;}

private:
  BDSInterpolator2D* eInterpolator;   ///< E Interpolator the field is based on.
  BDSInterpolator2D* bInterpolator;   ///< B Interpolator the field is based on.
  const G4int  eFirstDimensionIndex;  ///< Integer index to dimension to use.
  const G4bool eFirstTime;            ///< Cache of whether to use time coordinate.
  const G4int  eSecondDimensionIndex; ///< Integer index to dimension to use.
  const G4bool eSecondTime;           ///< Cache of whether to use time coordinate.
  const G4int  bFirstDimensionIndex;  ///< Integer index to dimension to use.
  const G4bool bFirstTime;            ///< Cache of whether to use time coordinate.
  const G4int  bSecondDimensionIndex; ///< Integer index to dimension to use.
  const G4bool bSecondTime;           ///< Cache of whether to use time coordinate.
};

#endif
