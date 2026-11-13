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
#ifndef BDSINTERPOLATOR1DNEAREST_H
#define BDSINTERPOLATOR1DNEAREST_H

#include "BDSFieldValue.hh"
#include "BDSInterpolator1D.hh"

#include "G4Types.hh"

class BDSArray1DCoords;

/**
 * @brief Interpolated field array that gives the nearest neighbour value.
 *
 * Does not own array - so multiple interpolators could be used on same data.
 *
 * @author Laurie Nevay
 */

class BDSInterpolator1DNearest: public BDSInterpolator1D
{
public:
  explicit BDSInterpolator1DNearest(BDSArray1DCoords* arrayIn);
  virtual ~BDSInterpolator1DNearest();

protected:
  virtual BDSFieldValue GetInterpolatedValueT(G4double x) const;
  
private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator1DNearest() = delete;
};

#endif
