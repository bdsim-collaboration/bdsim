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
#ifndef BDSARRAYOPERATORCOORDREFLECT_H
#define BDSARRAYOPERATORCOORDREFLECT_H
#include "BDSArray4DCoords.hh"
#include "BDSArrayInfo.hh"
#include "BDSArrayOperatorIndex.hh"
#include "BDSWarning.hh"

#include "G4String.hh"
#include "G4Types.hh"

#include <array>
#include <cmath>
#include <string>

/**
 * @brief 1D array for completeness in array system.
 * 
 * @author Laurie Nevay
 */

class BDSArrayOperatorIndexReflect: public BDSArrayOperatorIndex
{
public:
  BDSArrayOperatorIndexReflect():
    BDSArrayOperatorIndex("Reflect(None)"),
    xyzt{false, false, false, false},
    zeroInArrayCoords{0,0,0,0},
    dimensionInverted{false, false, false, false}
  {;}
  explicit BDSArrayOperatorIndexReflect(const std::array<G4bool,4>& xyztIn,
                                        const BDSArrayInfo& arrayInfo):
    BDSArrayOperatorIndexReflect()
  {
    operatesOn = xyztIn;
    xyzt = xyztIn;
    G4String newName = "Reflect(";
    for (const auto& v : xyzt)
      {newName += std::to_string(v);}
    newName += ")";
    name = newName;
    
    auto zeroV = arrayInfo.zeroPoint;
    auto nPoints = arrayInfo.nPoints;
    for (G4int i = 0; i < 4; i++)
      {
        G4bool inverted = (zeroV[i] >= nPoints[i] - 1) && (nPoints[i] > 1);
        dimensionInverted[i] = inverted;
        zeroInArrayCoords[i] = inverted ? nPoints[i] - 1 : (G4int)std::floor(zeroV[i]);
      }
  }
  virtual ~BDSArrayOperatorIndexReflect(){;}
  
  virtual void Apply(G4int& x,
                     G4int& y,
                     G4int& z,
                     G4int& t) const
  {
    G4int* values[4] = {&x, &y, &z, &t};
    for (G4int i = 0; i < 4; i++)
      {
	G4int v = *(values[i]);
	if (dimensionInverted[i])
	  { *(values[i]) = xyzt[i] && v > zeroInArrayCoords[i] ? zeroInArrayCoords[i] - (v- zeroInArrayCoords[i]) : v;}
	else
	  { *(values[i]) = xyzt[i] && v < zeroInArrayCoords[i] ? std::abs(v - zeroInArrayCoords[i]) : v; }
      }
  }
  
  virtual void ApplyX(G4int& x) const {x = xyzt[0] ? std::abs(x) : x;}
  virtual void ApplyY(G4int& y) const {y = xyzt[1] ? std::abs(y) : y;}
  virtual void ApplyZ(G4int& z) const {z = xyzt[2] ? std::abs(z) : z;}
  virtual void ApplyT(G4int& t) const {t = xyzt[3] ? std::abs(t) : t;}
  
  virtual void TransformLimits(G4double& xMin, G4double& xMax,
                               G4double& yMin, G4double& yMax,
                               G4double& zMin, G4double& zMax,
                               G4double& tMin, G4double& tMax) const
  {
    xMin = -std::abs(xMax);
    yMin = -std::abs(yMax);
    zMin = -std::abs(zMax);
    tMin = -std::abs(tMax);
  }
  
private:
  std::array<G4bool,4> xyzt;
  std::array<G4int, 4> zeroInArrayCoords;
  std::array<G4bool,4> dimensionInverted;
};

#endif
