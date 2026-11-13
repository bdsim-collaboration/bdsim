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
#ifndef BDSARRAY1DCOORDSTRANSFORMED_H
#define BDSARRAY1DCOORDSTRANSFORMED_H

#include "BDSArray1DCoords.hh"
#include "BDSDimensionType.hh"
#include "BDSFieldValue.hh"

#include "G4Types.hh"

#include <ostream>

class BDSArrayOperatorIndex;
class BDSArrayOperatorValue;

/**
 * @brief Wrapped BDSArray1DCoords with possible transformation to extend field.
 *
 * Uses an index operator and a value operator. The index operator will map some
 * array index coordinates to another index to give the array a larger appearance
 * for the same data, e.g. index -5 -> 5 for reflection. The value operator may
 * flip components of the field value stored depending on the original array
 * coordinates supplied, e.g. flip Bx in quadrant 2.
 *
 * @author Laurie Nevay
 */

class BDSArray1DCoordsTransformed: public BDSArray1DCoords
{
public:
  /// No default constructor.
  BDSArray1DCoordsTransformed() = delete;
  /// A valid index and value operator must be supplied.
  BDSArray1DCoordsTransformed(BDSArray1DCoords* arrayIn,
                              BDSArrayOperatorIndex* indexOperatorIn,
                              BDSArrayOperatorValue* valueOperatorIn);
  virtual ~BDSArray1DCoordsTransformed();
  
  /// Extract 2 points lying around coordinate x. Applies index operator to access the
  /// data and applies value operator to the value.
  virtual void ExtractSection2(G4double x,
                               BDSFieldValue (&localData)[2],
                               G4double& xFrac) const;
  
  /// Same as base class but applies index operator to access the data
  /// and applies value operator to the value.
  virtual void ExtractSection4(G4double x,
                               BDSFieldValue (&localData)[4],
                               G4double& xFrac) const;
  
  /// Extract nearest field value from array. y,z,t ignored but required for overload.
  virtual BDSFieldValue ExtractNearest(G4double x,
                                       G4double y = 0,
                                       G4double z = 0,
                                       G4double t = 0) const;
  
  virtual std::ostream& Print(std::ostream& out) const;
  
  /// Delegate function to call polymorphic Print().
  friend std::ostream& operator<< (std::ostream& out, BDSArray1DCoordsTransformed const &a);
  
private:
  BDSArrayOperatorIndex* indexOperator;
  BDSArrayOperatorValue* valueOperator;
  
  /// Must return by reference and the true data value may have to be reflected
  /// in one or more dimensions.  Copy to this and return reference to it.
  mutable BDSFieldValue returnValue;
};

#endif
