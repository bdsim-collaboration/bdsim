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
#include "BDSArray2DCoordsTransformed.hh"
#include "BDSArrayOperatorIndex.hh"
#include "BDSArrayOperatorValue.hh"
#include "BDSFieldValue.hh"

#include <array>
#include <cmath>
#include <ostream>

#include "globals.hh"

BDSArray2DCoordsTransformed::BDSArray2DCoordsTransformed(BDSArray2DCoords* arrayIn,
                                                         BDSArrayOperatorIndex* indexOperatorIn,
                                                         BDSArrayOperatorValue* valueOperatorIn):
  BDSArray2DCoords(*arrayIn),
  indexOperator(indexOperatorIn),
  valueOperator(valueOperatorIn),
  returnValue(BDSFieldValue())
{;}

BDSArray2DCoordsTransformed::~BDSArray2DCoordsTransformed()
{
  delete indexOperator;
  delete valueOperator;
}

void BDSArray2DCoordsTransformed::ExtractSection2x2(G4double x,
                                         G4double y,
                                         BDSFieldValue (&localData)[2][2],
                                         G4double& xFrac,
                                         G4double& yFrac) const
{
  G4double xArrayCoords, yArrayCoords;
  ArrayCoordsFromXY(x, xArrayCoords, y, yArrayCoords);
  auto x1 = (G4int)std::floor(xArrayCoords);
  auto y1 = (G4int)std::floor(yArrayCoords);
  xFrac = xArrayCoords - x1;
  yFrac = yArrayCoords - y1;
  
  std::array<G4int, 2> indexArrX = {x1, x1+1};
  std::array<G4int, 2> indexArrY = {y1, y1+1};
  G4int indexOriginalX, indexOriginalY, indexTransformedX, indexTransformedY;
  for (G4int i = 0; i < 2; i++)
    {
      for (G4int j = 0; j < 2; j++)
	{
	  indexOriginalX = indexArrX[i];
	  indexTransformedX = indexOriginalX;
	  indexOriginalY = indexArrY[j];
	  indexTransformedY = indexOriginalY;
	  indexOperator->Apply(indexTransformedX, indexTransformedY);
	  BDSFieldValue v = GetConst(indexTransformedX, indexTransformedY);
	  localData[i][j] = valueOperator->Apply(v, indexOriginalX, indexOriginalY);
	}
    }
}

void BDSArray2DCoordsTransformed::ExtractSection4x4(G4double x,
                                                    G4double y,
                                                    BDSFieldValue (&localData)[4][4],
                                                    G4double& xFrac,
                                                    G4double& yFrac) const
{
  G4double xArrayCoords = ArrayCoordsFromX(x);
  G4double yArrayCoords = ArrayCoordsFromY(y);
  auto x1 = (G4int)std::floor(xArrayCoords);
  auto y1 = (G4int)std::floor(yArrayCoords);
  xFrac = xArrayCoords - x1;
  yFrac = yArrayCoords - y1;
  
  std::array<G4int, 4> indexArrX = {x1-1, x1, x1+1, x1+2};
  std::array<G4int, 4> indexArrY = {y1-1, y1, y1+1, y1+2};
  G4int indexOriginalX, indexOriginalY, indexTransformedX, indexTransformedY;
  for (G4int i = 0; i < 4; i++)
    {
      for (G4int j = 0; j < 4; j++)
	{
	  indexOriginalX = indexArrX[i];
	  indexTransformedX = indexOriginalX;
	  indexOriginalY = indexArrY[j];
	  indexTransformedY = indexOriginalY;
	  indexOperator->Apply(indexTransformedX, indexTransformedY);
	  BDSFieldValue v = GetConst(indexTransformedX, indexTransformedY);
	  localData[i][j] = valueOperator->Apply(v, indexOriginalX, indexOriginalY);
	}
    }
}

BDSFieldValue BDSArray2DCoordsTransformed::ExtractNearest(G4double x,
                                                          G4double y,
                                                          G4double /*z*/,
                                                          G4double /*t*/) const
{
  G4int indexOriginalX = NearestX(x);
  G4int indexOriginalY = NearestY(y);
  G4int indexTransformedX = indexOriginalX;
  G4int indexTransformedY = indexOriginalY;
  indexOperator->Apply(indexTransformedX, indexTransformedY);
  BDSFieldValue v = GetConst(indexTransformedX, indexTransformedY);
  v = valueOperator->Apply(v, indexOriginalX, indexOriginalY);
  return v;
}

std::ostream& BDSArray2DCoordsTransformed::Print(std::ostream& out) const
{
  out << "Spatial limits are the original ones" << G4endl;
  out << "Array index operator: " << indexOperator->Name() << G4endl;
  out << "Array value operator: " << valueOperator->Name() << G4endl;
  return BDSArray2DCoords::Print(out);
}

std::ostream& operator<< (std::ostream& out, BDSArray2DCoordsTransformed const &a)
{
  return a.Print(out);
}
