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
#include "BDSArray4DCoords.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSFieldValue.hh"
#include "BDSUtilities.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <ostream>
#include <string>

#include "globals.hh"

BDSArray4DCoords::BDSArray4DCoords(G4int nXIn, G4int nYIn, G4int nZIn, G4int nTIn,
				   G4double xMinIn, G4double xMaxIn,
				   G4double yMinIn, G4double yMaxIn,
				   G4double zMinIn, G4double zMaxIn,
				   G4double tMinIn, G4double tMaxIn,
                                   BDSDimensionType xDimensionIn,
                                   BDSDimensionType yDimensionIn,
                                   BDSDimensionType zDimensionIn,
                                   BDSDimensionType tDimensionIn):
  BDSArray4D(nXIn,nYIn,nZIn,nTIn),
  xMin(xMinIn), xMax(xMaxIn),
  yMin(yMinIn), yMax(yMaxIn),
  zMin(zMinIn), zMax(zMaxIn),
  tMin(tMinIn), tMax(tMaxIn),
  smallestSpatialStep(std::numeric_limits<double>::max()),
  xDimension(xDimensionIn),
  yDimension(yDimensionIn),
  zDimension(zDimensionIn),
  tDimension(tDimensionIn),
  dimensions{0,1,2,3},
  timeVarying(true)
{
  // There are 1 fewer differences than the points.
  if (nX > 1)
    {
      xStep = (xMax - xMin) / ((G4double)nX - 1);
      CheckStep(xStep, "x");
      smallestSpatialStep = std::min(smallestSpatialStep, xStep);
    }
  else
    {xStep = 1;}
  if (nY > 1)
    {
      yStep = (yMax - yMin) / ((G4double)nY - 1);
      CheckStep(yStep, "y");
      smallestSpatialStep = std::min(smallestSpatialStep, yStep);
    }
  else
    {yStep = 1;}
  if (nZ > 1)
    {
      zStep = (zMax - zMin) / ((G4double)nZ - 1);
      CheckStep(zStep, "z");
      smallestSpatialStep = std::min(smallestSpatialStep, zStep);
    }
  else
    {zStep = 1;}
  if (nT > 1)
    {
      tStep = (tMax - tMin) / ((G4double)nT - 1);
      CheckStep(tStep, "t");
      G4double lengthScale = CLHEP::c_light * tStep;
      smallestSpatialStep = std::min(smallestSpatialStep, lengthScale);
    }
  else
    {tStep = 1;}
  BuildDimensionIndex();
}

void BDSArray4DCoords::CheckStep(G4double step, const G4String& name)
{
  if (!BDS::IsFinite(step))
    {
      throw BDSException(__METHOD_NAME__, "Invalid " + name + "min and " + name + 
			 "max in array leading to 0 step size between points.");
    }
}

G4bool BDSArray4DCoords::OutsideCoords(G4double x,
                                       G4double y,
                                       G4double z,
                                       G4double t) const
{
  G4bool rx = x < xMin || x > xMax;
  G4bool ry = y < yMin || y > yMax;
  G4bool rz = z < zMin || z > zMax;
  G4bool rt = t < tMin || t > tMax;
  return rx || ry || rz || rt;
}

void BDSArray4DCoords::OutsideCoordsWarn(G4double x,
                                         G4double y,
                                         G4double z,
                                         G4double t) const
{
  if (OutsideCoords(x,y,z,t))
    {
      throw BDSException(__METHOD_NAME__, "(" +
			 std::to_string(x) + ", " +
			 std::to_string(y) + ", " +
			 std::to_string(z) + ", " +
			 std::to_string(t) + ") is outside array");
    }
}

void BDSArray4DCoords::ExtractSection2x2x2x2(G4double x,
                                             G4double y,
                                             G4double z,
                                             G4double t,
                                             BDSFieldValue (&localData)[2][2][2][2],
                                             G4double& xFrac,
                                             G4double& yFrac,
                                             G4double& zFrac,
                                             G4double& tFrac) const
{
  G4double xArrayCoords, yArrayCoords, zArrayCoords, tArrayCoords;
  ArrayCoordsFromXYZT(x, xArrayCoords, y, yArrayCoords, z, zArrayCoords, t, tArrayCoords);
  auto x1 = (G4int)std::floor(xArrayCoords);
  auto y1 = (G4int)std::floor(yArrayCoords);
  auto z1 = (G4int)std::floor(zArrayCoords);
  auto t1 = (G4int)std::floor(tArrayCoords);
  
  xFrac = xArrayCoords - x1;
  yFrac = yArrayCoords - y1;
  zFrac = zArrayCoords - z1;
  tFrac = tArrayCoords - t1;
  
  for (G4int i = 0; i < 2; i++)
    {
      for (G4int j = 0; j < 2; j++)
	{
	  for (G4int k = 0; k < 2; k++)
	    {
	      for (G4int l = 0; l < 2; l++)
		{localData[i][j][k][l] = GetConst(x1+i, y1+j, z1+k, t1+l);}
	    }
	}
    }
}

void BDSArray4DCoords::ExtractSection4x4x4x4(G4double x,
                                             G4double y,
                                             G4double z,
                                             G4double t,
                                             BDSFieldValue (&localData)[4][4][4][4],
                                             G4double& xFrac,
                                             G4double& yFrac,
                                             G4double& zFrac,
                                             G4double& tFrac) const
{
  G4double xArrayCoords, yArrayCoords, zArrayCoords, tArrayCoords;
  ArrayCoordsFromXYZT(x, xArrayCoords, y, yArrayCoords, z, zArrayCoords, t, tArrayCoords);
  auto x1 = (G4int)std::floor(xArrayCoords);
  auto y1 = (G4int)std::floor(yArrayCoords);
  auto z1 = (G4int)std::floor(zArrayCoords);
  auto t1 = (G4int)std::floor(tArrayCoords);
  
  xFrac = xArrayCoords - x1;
  yFrac = yArrayCoords - y1;
  zFrac = zArrayCoords - z1;
  tFrac = tArrayCoords - t1;
  
  for (G4int i = 0; i < 4; i++)
    {
      for (G4int j = 0; j < 4; j++)
	{
	  for (G4int k = 0; k < 4; k++)
	    {
	      for (G4int l = 0; l < 4; l++)
		{localData[i][j][k][l] = GetConst(x1-1+i, y1-1+j, z1-1+k, t1-1+l);}
	    }
	}
    }
}

BDSFieldValue BDSArray4DCoords::ExtractNearest(G4double x,
                                               G4double y,
                                               G4double z,
                                               G4double t) const
{
  G4int xind = NearestX(x);
  G4int yind = NearestY(y);
  G4int zind = NearestZ(z);
  G4int tind = NearestT(t);
  BDSFieldValue result = GetConst(xind, yind, zind, tind); // here we're constructing a copy on purpose
  return result;
}

std::ostream& BDSArray4DCoords::Print(std::ostream& out) const
{
  out << "X: (" << xMin << ", " << xMax << ")" << G4endl;
  out << "Y: (" << yMin << ", " << yMax << ")" << G4endl;
  out << "Z: (" << zMin << ", " << zMax << ")" << G4endl;
  out << "T: (" << tMin << ", " << tMax << ")" << G4endl;
  return BDSArray4D::Print(out);
}

std::ostream& operator<< (std::ostream& out, BDSArray4DCoords const &a)
{
  return a.Print(out);
}

BDSExtent BDSArray4DCoords::Extent() const
{
  return BDSExtent(xMin, xMax, yMin, yMax, zMin, zMax);
}

void BDSArray4DCoords::BuildDimensionIndex()
{
  dimensions[0] = FirstDimension().underlying();
  dimensions[1] = SecondDimension().underlying();
  dimensions[2] = ThirdDimension().underlying();
  dimensions[3] = FourthDimension().underlying();
}

G4int BDSArray4DCoords::DimensionIndex(BDSDimensionType spatialDimension) const
{
  G4int result = -1;
  G4int match = spatialDimension.underlying();
  for (G4int i = 0; i < 4; i++)
    {
      if (dimensions[i] == match)
	{
	  result = i;
	  break;
	}
    }
  return result;
}
