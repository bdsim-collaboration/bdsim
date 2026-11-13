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
#ifndef BDSINTERPOLATE4DLINEARMAG_H
#define BDSINTERPOLATE4DLINEARMAG_H

#include "BDSFieldValue.hh"
#include "BDSInterpolator4D.hh"

#include "G4Types.hh"

class BDSArray4DCoords;

/** 
 * @brief Linear interpolation over 4d array including magnitude interpolation.
 * 
 * Does not own array - so multiple interpolators could be used on same data.
 * 
 * This uses a quad-linear algorithm to interpolate a 3d array and return
 * the value at any arbitrary point. If the point lies outside the array
 * the default value for the templated parameter is returned (typically 0).
 * Therefore, the field drops to 0 outside the specified region.
 * 
 * This is based on an extension of the trilinear algorithm described in 
 * BDSInterpolator3DLinear.hh.
 * 
 * @author Laurie Nevay
 */

class BDSInterpolator4DLinearMag: public BDSInterpolator4D
{
public:
  explicit BDSInterpolator4DLinearMag(BDSArray4DCoords* arrayIn);
  virtual ~BDSInterpolator4DLinearMag();

protected:
  virtual BDSFieldValue GetInterpolatedValueT(G4double x,
					      G4double y,
					      G4double z,
					      G4double t) const;

private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator4DLinearMag() = delete;
};

#endif
