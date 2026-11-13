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
#ifndef BDSARRAYOPERATORINDEXFLIP_H
#define BDSARRAYOPERATORINDEXFLIP_H
#include "BDSArrayOperatorIndex.hh"

#include "G4String.hh"
#include "G4Types.hh"

#include <array>
#include <string>

/**
 * @brief 1D array for completeness in array system.
 * 
 * @author Laurie Nevay
 */

class BDSArrayOperatorIndexFlip: public BDSArrayOperatorIndex
{
public:
  BDSArrayOperatorIndexFlip():
    BDSArrayOperatorIndex("Flip(None)"),
    xyzt{1,1,1,1}
  {;}
  explicit BDSArrayOperatorIndexFlip(const std::array<G4bool,4>& xyztIn):
    BDSArrayOperatorIndexFlip()
  {
    operatesOn = xyztIn;
    
    for (G4int i = 0; i < 4; i++)
      {xyzt[0] = xyztIn[i] ? -1 : 1;}
  
    G4String newName = "Flip(";
    for (const auto& v : xyzt)
      {newName += std::to_string(v < 0);}
    newName += ")";
    name = newName;
  }
  virtual ~BDSArrayOperatorIndexFlip(){;}
  
  virtual void Apply(G4int& x,
                     G4int& y,
                     G4int& z,
                     G4int& t) const
  {
    x *= xyzt[0];
    y *= xyzt[1];
    z *= xyzt[2];
    t *= xyzt[3];
  }
  virtual void ApplyX(G4int& x) const {x *= xyzt[0];}
  virtual void ApplyY(G4int& y) const {y *= xyzt[1];}
  virtual void ApplyZ(G4int& z) const {z *= xyzt[2];}
  virtual void ApplyT(G4int& t) const {t *= xyzt[3];}
  
  virtual void TransformLimits(G4double& xMin, G4double& xMax,
                               G4double& yMin, G4double& yMax,
                               G4double& zMin, G4double& zMax,
                               G4double& tMin, G4double& tMax) const
  {
    if (xyzt[0])
      {std::swap(xMin, xMax);}
    if (xyzt[1])
      {std::swap(yMin, yMax);}
    if (xyzt[2])
      {std::swap(zMin, zMax);}
    if (xyzt[3])
      {std::swap(tMin, tMax);}
  }
  
private:
  std::array<G4int,4> xyzt;
};

#endif
