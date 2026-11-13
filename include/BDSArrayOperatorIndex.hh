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
#ifndef BDSARRAYOPERATORINDEX_H
#define BDSARRAYOPERATORINDEX_H
#include "G4String.hh"
#include "G4Types.hh"

#include <array>

/**
 * @brief Interface for modifying by reference array indices.
 *
 * This transforms coordinates in array index space via some mapping.
 *
 * Apply() method is pure virtual.
 * 
 * @author Laurie Nevay
 */

class BDSArrayOperatorIndex
{
public:
  BDSArrayOperatorIndex() = delete;
  explicit BDSArrayOperatorIndex(const G4String& nameIn = "None"):
    name(nameIn),
    operatesOn{false, false, false, false}
  {;}
  virtual ~BDSArrayOperatorIndex(){;}
  
  /// Supply a name of this operator for feedback to the user in print out.
  virtual G4String Name() const {return name;}

  /// Operation to modify the coordinates by reference. As we typically have
  /// more than one return value, we therefore use references as a way of doing this.
  virtual void Apply(G4int& /*x*/,
                     G4int& /*y*/,
                     G4int& /*z*/,
                     G4int& /*t*/) const {;}
  virtual void ApplyX(G4int& /*x*/) const {;}
  virtual void ApplyY(G4int& /*y*/) const {;}
  virtual void ApplyZ(G4int& /*z*/) const {;}
  virtual void ApplyT(G4int& /*t*/) const {;}
  
  /// Return which axes this object operates on overall.
  virtual std::array<G4bool, 4> OperatesOnXYZT() const {return operatesOn;}
  
  void Apply(G4int& x) const
  {
    G4int v[3] = {0,0,0};
    return Apply(x, v[0], v[1], v[2]);
  }
  
  void Apply(G4int& x, G4int& y) const
  {
    G4int v[2] = {0,0};
    return Apply(x, y, v[0], v[1]);
  }
  
  void Apply(G4int& x, G4int& y, G4int& z) const
  {
    G4int t = 0;
    Apply(x, y, z, t);
  }
  
  virtual void TransformLimits(G4double& xMin, G4double& xMax,
                               G4double& yMin, G4double& yMax,
                               G4double& zMin, G4double& zMax,
                               G4double& tMin, G4double& tMax) const = 0;
  
  void TransformLimits(G4double& xMin, G4double xMax) const
  {
    G4double v[6] = {0,0,0,0,0,0};
    TransformLimits(xMin, xMax, v[0], v[1], v[2], v[3], v[4], v[5]);
  }
  void TransformLimits(G4double& xMin, G4double& xMax,
                       G4double& yMin, G4double& yMax) const
  {
    G4double v[4] = {0,0,0,0};
    TransformLimits(xMin, xMax, yMin, yMax, v[0], v[1], v[2], v[3]);
  }
  void TransformLimits(G4double& xMin, G4double& xMax,
                       G4double& yMin, G4double& yMax,
                       G4double& zMin, G4double& zMax) const
  {
    G4double v[2] = {0, 0};
    TransformLimits(xMin, xMax, yMin, yMax, zMin, zMax, v[0], v[1]);
  }
  
protected:
  G4String name;
  std::array<G4bool, 4> operatesOn;
};

#endif
