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
#ifndef BDSARRAYOPERATORINDEXV_H
#define BDSARRAYOPERATORINDEXV_H
#include "BDSArrayOperatorIndex.hh"

#include "G4Types.hh"

#include <array>
#include <vector>

/**
 * @brief Vectorised version of BDSArrayOperatorIndex.
 * 
 * @author Laurie Nevay
 */

class BDSArrayOperatorIndexV: public BDSArrayOperatorIndex
{
public:
  BDSArrayOperatorIndexV():
    BDSArrayOperatorIndex("Vector( "){;}
  virtual ~BDSArrayOperatorIndexV()
  {
    for (auto op : operators)
      {delete op;}
  }
  
  /// Append an operator to the vector.
  void push_back(BDSArrayOperatorIndex* operatorIn)
  {
    operators.push_back(operatorIn);
    // update base class record of what this oeprates on
    const std::array<G4bool, 4> ops = operatorIn->OperatesOnXYZT();
    for (G4int i = 0; i < (G4int)ops.size(); i++)
      {operatesOn[i] = operatesOn[i] || ops[i];}
  }
  
  /// Supply a name of this operator for feedback to the user in print out.
  virtual G4String Name() const
  {
    G4String result = "Vector(";
    for (auto op : operators)
      {result += op->Name() + " ";}
    result += ")";
    return result;
  }

  /// Operation to modify the coordinates by reference. As we typically have
  /// more than one return value, we therefore use references as a way of doing this.
  virtual void Apply(G4int& x,
                     G4int& y,
                     G4int& z,
                     G4int& t) const
  {
    for (auto op : operators)
      {op->Apply(x,y,z,t);}
  }
  virtual void ApplyX(G4int& x) const
  {
    for (auto op : operators)
      {op->ApplyX(x);}
  }
  virtual void ApplyY(G4int& y) const
  {
    for (auto op : operators)
      {op->ApplyY(y);}
  }
  virtual void ApplyZ(G4int& z) const
  {
    for (auto op : operators)
      {op->ApplyZ(z);}
  }
  virtual void ApplyT(G4int& t) const
  {
    for (auto op : operators)
      {op->ApplyT(t);}
  }
  
  virtual void TransformLimits(G4double& xMin, G4double& xMax,
                               G4double& yMin, G4double& yMax,
                               G4double& zMin, G4double& zMax,
                               G4double& tMin, G4double& tMax) const
  {
    for (auto op : operators)
      {op->TransformLimits(xMin, xMax, yMin, yMax, zMin, zMax, tMin, tMax);}
  }

  std::vector<BDSArrayOperatorIndex*> operators;
};

#endif
