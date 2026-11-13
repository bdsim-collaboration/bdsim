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
#include "BDSArray4DCoordsTransformed.hh"
#include "BDSArrayOperatorIndex.hh"
#include "BDSArrayOperatorValue.hh"
#include "BDSFieldValue.hh"

#include <array>
#include <cmath>
#include <ostream>

#include "globals.hh"

BDSArray4DCoordsTransformed::BDSArray4DCoordsTransformed(BDSArray4DCoords* arrayIn,
                                                         BDSArrayOperatorIndex* indexOperatorIn,
                                                         BDSArrayOperatorValue* valueOperatorIn):
  BDSArray4DCoords(*arrayIn),
  indexOperator(indexOperatorIn),
  valueOperator(valueOperatorIn),
  returnValue(BDSFieldValue())
{;}

BDSArray4DCoordsTransformed::~BDSArray4DCoordsTransformed()
{
  delete indexOperator;
  delete valueOperator;
}

void BDSArray4DCoordsTransformed::ExtractSection2x2x2x2(G4double x,
                                                        G4double y,
                                                        G4double z,
                                                        G4double t,
                                                        BDSFieldValue (&localData)[2][2][2][2],
                                                        G4double& xFrac,
                                                        G4double& yFrac,
                                                        G4double& zFrac,
                                                        G4double& tFrac) const
{
  G4double xArrayCoords = ArrayCoordsFromX(x);
  G4double yArrayCoords = ArrayCoordsFromY(y);
  G4double zArrayCoords = ArrayCoordsFromZ(z);
  G4double tArrayCoords = ArrayCoordsFromT(t);
  auto x1 = (G4int)std::floor(xArrayCoords);
  auto y1 = (G4int)std::floor(yArrayCoords);
  auto z1 = (G4int)std::floor(zArrayCoords);
  auto t1 = (G4int)std::floor(tArrayCoords);
  xFrac = xArrayCoords - x1;
  yFrac = yArrayCoords - y1;
  zFrac = zArrayCoords - z1;
  tFrac = tArrayCoords - t1;
  
  std::array<G4int, 2> indexArrX = {x1, x1+1};
  std::array<G4int, 2> indexArrY = {y1, y1+1};
  std::array<G4int, 2> indexArrZ = {z1, z1+1};
  std::array<G4int, 2> indexArrT = {t1, t1+1};
  G4int indexOriginalX, indexOriginalY, indexOriginalZ, indexOriginalT;
  G4int indexTransformedX, indexTransformedY, indexTransformedZ, indexTransformedT;
  for (G4int i = 0; i < 2; i++)
    {
      for (G4int j = 0; j < 2; j++)
	{
	  for (G4int k = 0; k < 2; k++)
	    {
	      for (G4int l = 0; l < 2; l++)
		{
		  indexOriginalX = indexArrX[i];
		  indexTransformedX = indexOriginalX;
		  indexOriginalY = indexArrY[j];
		  indexTransformedY = indexOriginalY;
		  indexOriginalZ = indexArrZ[k];
		  indexTransformedZ = indexOriginalZ;
		  indexOriginalT = indexArrT[l];
		  indexTransformedT = indexOriginalT;
		  indexOperator->Apply(indexTransformedX, indexTransformedY, indexTransformedZ, indexTransformedT);
		  BDSFieldValue v = GetConst(indexTransformedX, indexTransformedY, indexTransformedZ, indexTransformedT);
		  localData[i][j][k][l] = valueOperator->Apply(v, indexOriginalX, indexOriginalY, indexOriginalZ, indexOriginalT);
		}
	    }
	}
    }
}

void BDSArray4DCoordsTransformed::ExtractSection4x4x4x4(G4double x,
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
  
  std::array<G4int, 4> indexArrX = {x1-1, x1, x1+1, x1+2};
  std::array<G4int, 4> indexArrY = {y1-1, y1, y1+1, y1+2};
  std::array<G4int, 4> indexArrZ = {z1-1, z1, z1+1, z1+2};
  std::array<G4int, 4> indexArrT = {t1-1, t1, t1+1, t1+2};
  G4int indexOriginalX, indexOriginalY, indexOriginalZ, indexOriginalT;
  G4int indexTransformedX, indexTransformedY, indexTransformedZ, indexTransformedT;
  for (G4int i = 0; i < 4; i++)
    {
      for (G4int j = 0; j < 4; j++)
	{
	  for (G4int k = 0; k < 4; k++)
	    {
	      for (G4int l = 0; l < 4; l++)
		{
		  indexOriginalX = indexArrX[i];
		  indexTransformedX = indexOriginalX;
		  indexOriginalY = indexArrY[j];
		  indexTransformedY = indexOriginalY;
		  indexOriginalZ = indexArrZ[k];
		  indexTransformedZ = indexOriginalZ;
		  indexOriginalT = indexArrT[l];
		  indexTransformedT = indexOriginalT;
		  indexOperator->Apply(indexTransformedX, indexTransformedY, indexTransformedZ, indexTransformedT);
		  BDSFieldValue v = GetConst(indexTransformedX, indexTransformedY, indexTransformedZ, indexTransformedT);
		  localData[i][j][k][l] = valueOperator->Apply(v, indexOriginalX, indexOriginalY, indexOriginalZ, indexOriginalT);
		}
	    }
	}
    }
}

BDSFieldValue BDSArray4DCoordsTransformed::ExtractNearest(G4double x,
                                                          G4double y,
                                                          G4double z,
                                                          G4double t) const
{
  G4int indexOriginalX = NearestX(x);
  G4int indexOriginalY = NearestY(y);
  G4int indexOriginalZ = NearestY(z);
  G4int indexOriginalT = NearestY(t);
  G4int indexTransformedX = indexOriginalX;
  G4int indexTransformedY = indexOriginalY;
  G4int indexTransformedZ = indexOriginalZ;
  G4int indexTransformedT = indexOriginalT;
  indexOperator->Apply(indexTransformedX, indexTransformedY, indexTransformedZ, indexTransformedT);
  BDSFieldValue v = GetConst(indexTransformedX, indexTransformedY, indexTransformedT, indexTransformedZ);
  v = valueOperator->Apply(v, indexOriginalX, indexOriginalY, indexOriginalZ, indexOriginalT);
  return v;
}

std::ostream& BDSArray4DCoordsTransformed::Print(std::ostream& out) const
{
  out << "Spatial limits are the original ones" << G4endl;
  out << "Array index operator: " << indexOperator->Name() << G4endl;
  out << "Array value operator: " << valueOperator->Name() << G4endl;
  return BDSArray4DCoords::Print(out);
}

std::ostream& operator<< (std::ostream& out, BDSArray4DCoordsTransformed const &a)
{
  return a.Print(out);
}
