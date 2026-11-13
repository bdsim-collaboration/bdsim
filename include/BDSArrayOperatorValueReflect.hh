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
#ifndef BDSARRAYOPERATORVALUEREFLECT_H
#define BDSARRAYOPERATORVALUEREFLECT_H
#include "BDSArrayOperatorValue.hh"
#include "BDSFieldValue.hh"

#include "G4String.hh"
#include "G4Types.hh"

#include <array>
#include <cmath>
#include <string>

/**
 * @brief Reflect field component in individual dimensions.
 * 
 * @author Laurie Nevay
 */

class BDSArrayOperatorValueReflect: public BDSArrayOperatorValue
{
public:
  BDSArrayOperatorValueReflect():
    BDSArrayOperatorValue("Reflect(None)"),
    xyz{false, false, false},
    zeroInArrayCoords{0,0,0},
    dimensionInverted{false, false, false},
    arrayToSpatialIndex{0,1,2,3}
  {;}
  BDSArrayOperatorValueReflect(const std::array<G4bool,4>& xyzt,
			       const BDSArrayInfo& arrayInfo):
    BDSArrayOperatorValueReflect()
  {
    for (G4int i = 0; i < 3; i++)
      {xyz[i] = xyzt[i];}
    
    arrayToSpatialIndex = arrayInfo.arrayToSpatialIndex;
    
    G4String newName = "Reflect(";
    for (const auto& v : xyz)
      {newName += std::to_string(v);}
    newName += ")";
    name = newName;
    
    auto zeroV = arrayInfo.zeroPoint;
    auto nPoints = arrayInfo.nPoints;
    for (G4int i = 0; i < 3; i++)
      {
        G4bool inverted = (zeroV[i] >= nPoints[i] - 1) && (nPoints[i] > 1);
        dimensionInverted[i] = inverted;
        zeroInArrayCoords[i] = inverted ? nPoints[i] - 1 : (G4int)std::floor(zeroV[i]);
      }
  }
  virtual ~BDSArrayOperatorValueReflect(){;}
  
  virtual BDSFieldValue Apply(BDSFieldValue v,
                              G4int xInd,
                              G4int yInd = 0,
                              G4int zInd = 0,
                              G4int tInd = 0) const
  {
    std::array<G4double, 4> m = {1,1,1,1};
    std::array<G4int, 4> inds = {xInd, yInd, zInd, tInd};
    for (G4int i = 0; i < 3; i++)
      {
	if (dimensionInverted[i])
	  {m[arrayToSpatialIndex[i]] = xyz[i] && inds[i] > zeroInArrayCoords[i] ? -1.0 : 1.0;}
	else
	  {m[arrayToSpatialIndex[i]] = xyz[i] && inds[i] < zeroInArrayCoords[i] ? -1.0 : 1.0;}
      }
    tInd = 4;// to retain default values and prevent compiler warnings
    return BDSFieldValue(v.x()*m[0], v.y()*m[1], v.z()*m[2]);
  }
  
private:
  std::array<G4bool,3> xyz;
  std::array<G4int, 4> zeroInArrayCoords;
  std::array<G4bool,4> dimensionInverted;
  std::array<G4int, 4> arrayToSpatialIndex;
};

#endif
