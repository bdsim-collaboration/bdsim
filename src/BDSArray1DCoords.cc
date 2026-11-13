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
#include "BDSArray1DCoords.hh"
#include "BDSExtent.hh"

#include "globals.hh"

#include <array>
#include <cmath>
#include <ostream>
#include <limits>
#include <set>
#include <vector>

BDSArray1DCoords::BDSArray1DCoords(G4int            nXIn,
				   G4double         xMinIn,
				   G4double         xMaxIn,
				   BDSDimensionType dimensionIn):
  BDSArray2DCoords(nXIn,1,
		   xMinIn,xMaxIn,
		   0,   1,
		   dimensionIn)
{
  std::set<BDSDimensionType> allDims = {BDSDimensionType::x,
                                        BDSDimensionType::y,
                                        BDSDimensionType::z,
                                        BDSDimensionType::t};
  allDims.erase(dimensionIn);
  std::array<BDSDimensionType*, 3> vars = {&yDimension,
                                           &zDimension,
                                           &tDimension};
  std::vector<BDSDimensionType> unusedDims(allDims.begin(), allDims.end());
  for (G4int i = 0; i < 3; i++)
    {*(vars[i]) = unusedDims[i];}
  BuildDimensionIndex();
  timeVarying = dimensionIn == BDSDimensionType::t;
}

void BDSArray1DCoords::ExtractSection2(G4double x,
                                       BDSFieldValue (&localData)[2],
                                       G4double& xFrac) const
{
  G4double xArrayCoords = ArrayCoordsFromX(x);
  auto x1 = (G4int)std::floor(xArrayCoords);
  xFrac = xArrayCoords - x1;
  for (G4int i = 0; i < 2; i++)
    {localData[i] = GetConst(x1+i);}
}

void BDSArray1DCoords::ExtractSection4(G4double x,
                                       BDSFieldValue (&localData)[4],
                                       G4double& xFrac) const
{
  G4double xArrayCoords = ArrayCoordsFromX(x);
  auto x1 = (G4int)std::floor(xArrayCoords);
  xFrac = xArrayCoords - x1;
  for (G4int i = 0; i < 4; i++)
    {localData[i] = GetConst(x1-1+i);}
}

BDSFieldValue BDSArray1DCoords::ExtractNearest(G4double x,
                                               G4double /*y*/,
                                               G4double /*z*/,
                                               G4double /*t*/) const
{
  return GetConst(NearestX(x));
}

std::ostream& operator<< (std::ostream& out, BDSArray1DCoords const &a)
{
  return a.Print(out);
}

BDSExtent BDSArray1DCoords::Extent() const
{
  G4double limitMax = std::numeric_limits<double>::max();
  G4double limitMin = std::numeric_limits<double>::lowest();
  return BDSExtent(xMin, xMax, limitMin, limitMax, limitMin, limitMax);
}
