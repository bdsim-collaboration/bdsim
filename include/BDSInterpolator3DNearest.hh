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
#ifndef BDSINTERPOLATOR3DNEAREST_H
#define BDSINTERPOLATOR3DNEAREST_H

#include "BDSFieldValue.hh"
#include "BDSInterpolator3D.hh"

#include "G4Types.hh"

class BDSArray3DCoords;

/**
 * @brief Interpolated field array that gives the nearest neighbour value.
 *
 * Does not own array - so multiple interpolators could be used on same data.
 *
 * @author Laurie Nevay
 */

class BDSInterpolator3DNearest: public BDSInterpolator3D
{
public:
  explicit BDSInterpolator3DNearest(BDSArray3DCoords* arrayIn);
  virtual ~BDSInterpolator3DNearest();

protected:
  virtual BDSFieldValue GetInterpolatedValueT(G4double x, G4double y, G4double z) const;

private:
  /// Private default constructor to force the use of the provided one.
  BDSInterpolator3DNearest() = delete;
};

#endif
