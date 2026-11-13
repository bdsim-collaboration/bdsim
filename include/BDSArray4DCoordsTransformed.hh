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
#ifndef BDSARRAY4DCOORDSTRANSFORMED_H
#define BDSARRAY4DCOORDSTRANSFORMED_H

#include "BDSArray4DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSFourVector.hh"

#include "globals.hh"

#include <ostream>

class BDSArrayOperatorIndex;
class BDSArrayOperatorValue;
class BDSExtent;

/**
 * @brief Wrapped BDSArray4DCoords with possible transformation to extend field.
 *
 * See BDSArray1DCoordsTransformed for explanation.
 * 
 * @author Laurie Nevay
 */

class BDSArray4DCoordsTransformed: public BDSArray4DCoords
{
public:
  /// No default constructor.
  BDSArray4DCoordsTransformed() = delete;
  /// A valid index and value operator must be supplied.
  BDSArray4DCoordsTransformed(BDSArray4DCoords* arrayIn,
                              BDSArrayOperatorIndex* indexOperatorIn,
                              BDSArrayOperatorValue* valueOperatorIn);
  virtual ~BDSArray4DCoordsTransformed();
  
  /// Same as base class but applies index operator to access the data
  /// and applies value operator to the value.
  virtual void ExtractSection2x2x2x2(G4double x,
                                   G4double y,
                                   G4double z,
                                   G4double t,
                                   BDSFieldValue (&localData)[2][2][2][2],
                                   G4double& xFrac,
                                   G4double& yFrac,
                                   G4double& zFrac,
                                   G4double& tFrac) const;
  
  /// Same as base class but applies index operator to access the data
  /// and applies value operator to the value.
  virtual void ExtractSection4x4x4x4(G4double x,
                                   G4double y,
                                   G4double z,
                                   G4double t,
                                   BDSFieldValue (&localData)[4][4][4][4],
                                   G4double& xFrac,
                                   G4double& yFrac,
                                   G4double& zFrac,
                                   G4double& tFrac) const;
  
  /// Same as base class but applies index operator to access the data
  /// and applies value operator to the value.
  virtual BDSFieldValue ExtractNearest(G4double x,
                                       G4double y = 0,
                                       G4double z = 0,
                                       G4double t = 0) const;
  
  virtual std::ostream& Print(std::ostream& out) const;
  
  /// Delegate function to call polymorphic Print().
  friend std::ostream& operator<< (std::ostream& out, BDSArray4DCoordsTransformed const &a);
  
private:
  BDSArrayOperatorIndex* indexOperator;
  BDSArrayOperatorValue* valueOperator;
  
  /// Must return by reference and the true data value may have to be reflected
  /// in one or more dimensions.  Copy to this and return reference to it.
  mutable BDSFieldValue returnValue;
};

#endif
