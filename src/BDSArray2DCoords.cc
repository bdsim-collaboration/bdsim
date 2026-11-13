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
#include "BDSArray2DCoords.hh"
#include "BDSExtent.hh"

#include <array>
#include <cmath>
#include <limits>
#include <ostream>
#include <set>
#include <vector>

#include "globals.hh"

BDSArray2DCoords::BDSArray2DCoords(G4int nXIn, G4int nYIn,
				   G4double xMinIn, G4double xMaxIn,
				   G4double yMinIn, G4double yMaxIn,
				   BDSDimensionType xDimensionIn,
				   BDSDimensionType yDimensionIn):
  BDSArray3DCoords(nXIn,nYIn,1,
		   xMinIn,xMaxIn,
		   yMinIn,yMaxIn,
		   0,   1,
		   xDimensionIn,
		   yDimensionIn)
{
  std::set<BDSDimensionType> allDims = {BDSDimensionType::x,
                                        BDSDimensionType::y,
                                        BDSDimensionType::z,
                                        BDSDimensionType::t};
  allDims.erase(xDimensionIn);
  allDims.erase(yDimensionIn);
  std::array<BDSDimensionType*, 2> vars = {&zDimension, &tDimension};
  std::vector<BDSDimensionType> unusedDims(allDims.begin(), allDims.end());
  for (G4int i = 0; i < 2; i++)
    {*(vars[i]) = unusedDims[i];}
  BuildDimensionIndex();
  
  BDSDimensionType tt = BDSDimensionType::t;
  timeVarying = xDimensionIn == tt || yDimensionIn == tt;
}

void BDSArray2DCoords::ExtractSection2x2(G4double x,
                                         G4double y,
                                         BDSFieldValue (&localData)[2][2],
                                         G4double& xFrac,
                                         G4double& yFrac) const
{
  G4double xarr, yarr;
  ArrayCoordsFromXY(x, xarr, y, yarr);
  
  auto x1 = (G4int)std::floor(xarr);
  auto y1 = (G4int)std::floor(yarr);
  
  xFrac = xarr - x1;
  yFrac = yarr - y1;
  
  for (G4int i = 0; i < 2; i++)
    {
      for (G4int j = 0; j < 2; j++)
	{localData[i][j] = GetConst(x1+i, y1+j);}
    }
}

void BDSArray2DCoords::ExtractSection4x4(G4double x,
                       G4double y,
                       BDSFieldValue (&localData)[4][4],
                       G4double& xFrac,
                       G4double& yFrac) const
{
  G4double xarr, yarr;
  ArrayCoordsFromXY(x, xarr, y, yarr);
  
  // Array indices will look like this where point lies in unit (in array coords)
  // square between 11,12,22,21.
  
  // 03 13 23 33
  // 02 12 22 32
  // 01 11 21 31
  // 00 10 20 30
  
  auto x1 = (G4int)std::floor(xarr);
  auto y1 = (G4int)std::floor(yarr);
  
  xFrac = xarr - x1;
  yFrac = yarr - y1;
  
  for (G4int i = 0; i < 4; i++)
    {
      for (G4int j = 0; j < 4; j++)
	{localData[i][j] = GetConst(x1-1+i, y1-1+j);}
    }
}

BDSFieldValue BDSArray2DCoords::ExtractNearest(G4double x,
                                               G4double y,
                                               G4double /*z*/,
                                               G4double /*t*/) const
{
  G4int xind = NearestX(x);
  G4int yind = NearestY(y);
  BDSFieldValue result = GetConst(xind, yind); // here we're constructing a copy on purpose
  return result;
}

std::ostream& operator<< (std::ostream& out, BDSArray2DCoords const &a)
{
  return a.Print(out);
}

BDSExtent BDSArray2DCoords::Extent() const
{
  G4double limitMax = std::numeric_limits<double>::max();
  G4double limitMin = std::numeric_limits<double>::lowest();
  return BDSExtent(xMin, xMax, yMin, yMax, limitMin, limitMax);
}
