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
#ifndef BDSINTERPOLATE3DLINEAR_H
#define BDSINTERPOLATE3DLINEAR_H

#include "BDSFieldValue.hh"
#include "BDSInterpolator3D.hh"

#include "G4Types.hh"

class BDSArray3DCoords;

/** 
 * @brief Linear interpolation over 3d array.
 * 
 * Does not own array - so multiple interpolators could be used on same data.
 * 
 * This uses a trilinear algorithm to interpolate a 3d array and return
 * the value at any arbitrary point. If the point lies outside the array
 * the default value for the templated parameter is returned (typically 0).
 * Therefore, the field drops to 0 outside the specified region.
 *
 * This is based on the following article:
 * https://en.wikipedia.org/wiki/Trilinear_interpolation
 * 
 * @author Laurie Nevay
 */

class BDSInterpolator3DLinear: public BDSInterpolator3D
{
public:
  explicit BDSInterpolator3DLinear(BDSArray3DCoords* arrayIn);
  virtual ~BDSInterpolator3DLinear();

protected:
  virtual BDSFieldValue GetInterpolatedValueT(G4double x, G4double y, G4double z) const;

private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator3DLinear() = delete;
};

#endif
