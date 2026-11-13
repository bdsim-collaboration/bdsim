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
#ifndef BDSARRAYOPERATORVALUEV_H
#define BDSARRAYOPERATORVALUEV_H
#include "BDSArrayOperatorValue.hh"

#include "G4Types.hh"

#include <vector>

/**
 * @brief A vectorised version of BDSArrayOperatorValueV.
 * 
 * @author Laurie Nevay
 */

class BDSArrayOperatorValueV: public BDSArrayOperatorValue
{
public:
  BDSArrayOperatorValueV():
    BDSArrayOperatorValue("Vector( ")
  {;}
  virtual ~BDSArrayOperatorValueV()
  {
    for (auto op : operators)
      {delete op;}
  }
  
  /// Append an operator to the vector.
  void push_back(BDSArrayOperatorValue* operatorIn) {operators.push_back(operatorIn);}
  
  /// Return a name of the operator for feedback to the user in print out.
  virtual G4String Name() const
  {
    G4String result = name;
    for (auto op : operators)
      {result += op->Name() + " ";}
    result += " )";
    return result;
  }
  
  /// Index arguments are original coordinate space indices, i.e. allowed
  /// to be negative. No action by default.
  virtual BDSFieldValue Apply(BDSFieldValue v,
                              G4int xInd,
                              G4int yInd = 0,
                              G4int zInd = 0,
                              G4int tInd = 0) const
  {
    BDSFieldValue tempV = v;
    for (auto op : operators)
      {tempV = op->Apply(tempV, xInd, yInd, zInd, tInd);}
    return tempV;
  }

  std::vector<BDSArrayOperatorValue*> operators;
};

#endif
