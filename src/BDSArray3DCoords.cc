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
#include "BDSArray3DCoords.hh"

#include <cmath>
#include <ostream>
#include <set>
#include <vector>

#include "globals.hh"

BDSArray3DCoords::BDSArray3DCoords(G4int nXIn, G4int nYIn, G4int nZIn,
				   G4double xMinIn, G4double xMaxIn,
				   G4double yMinIn, G4double yMaxIn,
				   G4double zMinIn, G4double zMaxIn,
				   BDSDimensionType xDimensionIn,
				   BDSDimensionType yDimensionIn,
				   BDSDimensionType zDimensionIn):
  BDSArray4DCoords(nXIn,nYIn,nZIn,1,
		   xMinIn,xMaxIn,
		   yMinIn,yMaxIn,
		   zMinIn,zMaxIn,
		   0,   1,
		   xDimensionIn,
		   yDimensionIn,
		   zDimensionIn)
{
  std::set<BDSDimensionType> allDims = {BDSDimensionType::x,
                                        BDSDimensionType::y,
                                        BDSDimensionType::z,
                                        BDSDimensionType::t};
  allDims.erase(xDimensionIn);
  allDims.erase(yDimensionIn);
  allDims.erase(zDimensionIn);
  std::vector<BDSDimensionType> allDimsV(allDims.begin(), allDims.end());
  tDimension = allDimsV[0];
  BuildDimensionIndex();
  
  BDSDimensionType tt = BDSDimensionType::t;
  timeVarying = xDimensionIn == tt || yDimensionIn == tt || zDimensionIn == tt;
}

void BDSArray3DCoords::ExtractSection2x2x2(G4double x,
                                           G4double y,
                                           G4double z,
                                           BDSFieldValue (&localData)[2][2][2],
                                           G4double& xFrac,
                                           G4double& yFrac,
                                           G4double& zFrac) const
{
  G4double xArrayCoords, yArrayCoords, zArrayCoords;
  ArrayCoordsFromXYZ(x, xArrayCoords, y, yArrayCoords, z, zArrayCoords);
  auto x1 = (G4int)std::floor(xArrayCoords);
  auto y1 = (G4int)std::floor(yArrayCoords);
  auto z1 = (G4int)std::floor(zArrayCoords);
  
  xFrac = xArrayCoords - x1;
  yFrac = yArrayCoords - y1;
  zFrac = zArrayCoords - z1;
  
  for (G4int i = 0; i < 2; i++)
    {
      for (G4int j = 0; j < 2; j++)
	{
	  for (G4int k = 0; k < 2; k++)
	    {localData[i][j][k] = GetConst(x1+i, y1+j, z1+k);}
	}
    }
}

void BDSArray3DCoords::ExtractSection4x4x4(G4double x,
                                           G4double y,
                                           G4double z,
                                           BDSFieldValue (&localData)[4][4][4],
                                           G4double& xFrac,
                                           G4double& yFrac,
                                           G4double& zFrac) const
{
  G4double xArrayCoords, yArrayCoords, zArrayCoords;
  ArrayCoordsFromXYZ(x, xArrayCoords, y, yArrayCoords, z, zArrayCoords);
  auto x1 = (G4int)std::floor(xArrayCoords);
  auto y1 = (G4int)std::floor(yArrayCoords);
  auto z1 = (G4int)std::floor(zArrayCoords);
  xFrac = xArrayCoords - x1;
  yFrac = yArrayCoords - y1;
  zFrac = zArrayCoords - z1;
  
  for (G4int i = 0; i < 4; i++)
    {
      for (G4int j = 0; j < 4; j++)
	{
	  for (G4int k = 0; k < 4; k++)
	    {
	      localData[i][j][k] = GetConst(x1-1+i, y1-1+j, z1-1+k);
	    }
	}
    }
}

BDSFieldValue BDSArray3DCoords::ExtractNearest(G4double x,
                                               G4double y,
                                               G4double z,
                                               G4double /*t*/) const
{
  G4int xind = NearestX(x);
  G4int yind = NearestY(y);
  G4int zind = NearestZ(z);
  BDSFieldValue result = GetConst(xind, yind, zind); // here we're constructing a copy on purpose
  return result;
}

std::ostream& operator<< (std::ostream& out, BDSArray3DCoords const &a)
{
  return a.Print(out);
}
