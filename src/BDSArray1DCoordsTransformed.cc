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
#include "BDSArray1DCoordsTransformed.hh"
#include "BDSArrayOperatorIndex.hh"
#include "BDSArrayOperatorValue.hh"
#include "BDSFieldValue.hh"

#include "globals.hh"

#include <array>
#include <cmath>
#include <ostream>

BDSArray1DCoordsTransformed::BDSArray1DCoordsTransformed(BDSArray1DCoords* arrayIn,
                                                         BDSArrayOperatorIndex* indexOperatorIn,
                                                         BDSArrayOperatorValue* valueOperatorIn):
  BDSArray1DCoords(*arrayIn),
  indexOperator(indexOperatorIn),
  valueOperator(valueOperatorIn),
  returnValue(BDSFieldValue())
{;}

BDSArray1DCoordsTransformed::~BDSArray1DCoordsTransformed()
{
  delete indexOperator;
  delete valueOperator;
}

void BDSArray1DCoordsTransformed::ExtractSection2(G4double x,
                     BDSFieldValue (&localData)[2],
                     G4double& xFrac) const
{
  G4double xArrayCoords = ArrayCoordsFromX(x);
  
  auto x1 = (G4int)std::floor(xArrayCoords);
  
  xFrac = xArrayCoords - x1;
  std::array<G4int, 2> indexArr = {x1, x1+1};
  G4int indexOriginal, indexTransformed;
  for (G4int i = 0; i < 2; i++)
    {
      indexOriginal = indexArr[i];
      indexTransformed = indexOriginal;
      indexOperator->Apply(indexTransformed);
      BDSFieldValue v = GetConst(indexTransformed);
      localData[i] = valueOperator->Apply(v, indexOriginal);
    }
}

void BDSArray1DCoordsTransformed::ExtractSection4(G4double x,
                                                  BDSFieldValue (&localData)[4],
                                                  G4double& xFrac) const
{
  G4double xArrayCoords = ArrayCoordsFromX(x);
  auto x1 = (G4int)std::floor(xArrayCoords);
  xFrac = xArrayCoords - x1;
  
  std::array<G4int, 4> indexArr = {x1-1, x1, x1+1, x1+2};
  G4int indexOriginal, indexTransformed;
  for (G4int i = 0; i < 4; i++)
    {
      indexOriginal = indexArr[i];
      indexTransformed = indexOriginal;
      indexOperator->Apply(indexTransformed);
      BDSFieldValue v = GetConst(indexTransformed);
      localData[i] = valueOperator->Apply(v, indexOriginal);
    }
}

BDSFieldValue BDSArray1DCoordsTransformed::ExtractNearest(G4double x,
                                                          G4double /*y*/,
                                                          G4double /*z*/,
                                                          G4double /*t*/) const
{
  G4int indexOriginal = NearestX(x);
  G4int indexTransformed = indexOriginal;
  indexOperator->Apply(indexTransformed);
  BDSFieldValue v = GetConst(indexTransformed);
  v = valueOperator->Apply(v, indexOriginal);
  return v;
}

std::ostream& BDSArray1DCoordsTransformed::Print(std::ostream& out) const
{
  out << "Spatial limits are the original ones" << G4endl;
  out << "Array index operator: " << indexOperator->Name() << G4endl;
  out << "Array value operator: " << valueOperator->Name() << G4endl;
  return BDSArray1DCoords::Print(out);
}

std::ostream& operator<< (std::ostream& out, BDSArray1DCoordsTransformed const &a)
{
  return a.Print(out);
}
