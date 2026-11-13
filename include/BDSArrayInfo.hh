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
#ifndef BDSARRAYINFO_H
#define BDSARRAYINFO_H
#include "BDSArray4DCoords.hh"
#include "BDSFourVector.hh"

#include "G4Types.hh"

#include <array>

/**
 * @brief Simple holder of information about an array.
 * 
 * @author Laurie Nevay
 */

class BDSArrayInfo
{
public:
  BDSArrayInfo() = delete;
  explicit BDSArrayInfo(const BDSArray4DCoords* array):
    zeroPoint(std::array<G4double,4>()),
    nPoints(std::array<G4int,4>()),
    arrayToSpatialIndex{0,1,2,3}
  {
    auto zV = array->ArrayCoordsFromXYZT(0, 0, 0, 0);
    auto nP = array->NXYZT();
    for (G4int i = 0; i < 4; i++)
      {
	zeroPoint[i] = zV[i];
	nPoints[i] = nP[i];
      }
    arrayToSpatialIndex = array->ArrayToSpatialDimensionIndices();
  }
  ~BDSArrayInfo(){;}

  std::array<G4double, 4> zeroPoint;
  std::array<G4int, 4> nPoints;
  std::array<G4int, 4> arrayToSpatialIndex;
};

#endif
