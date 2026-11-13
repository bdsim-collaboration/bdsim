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
#ifndef BDSARRAY1DCOORDS_H
#define BDSARRAY1DCOORDS_H

#include "BDSArray2DCoords.hh"
#include "BDSDimensionType.hh"

#include "G4Types.hh"

#include <ostream>

class BDSExtent;

/**
 * @brief 1D array with spatial mapping derived from BDSArray4DCoords.
 *
 * @author Laurie Nevay
 */

class BDSArray1DCoords: public BDSArray2DCoords
{
public:
  /// No default constructor as the array is not adjustable after construction and
  /// therefore the size must be known at construction time.
  BDSArray1DCoords() = delete;
  BDSArray1DCoords(G4int            nX,
                   G4double         xMinIn,
                   G4double         xMaxIn,
                   BDSDimensionType dimensionIn = BDSDimensionType::x);
  virtual ~BDSArray1DCoords(){;}
  
  /// Extract 2 points lying around coordinate x.
  virtual void ExtractSection2(G4double x,
                               BDSFieldValue (&localData)[2],
                               G4double& xFrac) const;
  
  /// Extract 4 points lying around coordinate x, where x will be between points 2 and 3.
  virtual void ExtractSection4(G4double x,
                               BDSFieldValue (&localData)[4],
                               G4double& xFrac) const;
  
  /// Extract nearest field value from array. y,z,t ignored but required for overload.
  virtual BDSFieldValue ExtractNearest(G4double x,
                                       G4double y = 0,
                                       G4double z = 0,
                                       G4double t = 0) const;

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray1DCoords const &a);
  
  /// Return the SPATIAL (only) extent of this field without any offset. Ignores time.
  /// This override gives infinite limit in y,z, but accurate in x. TODO - check for
  /// if not in x,y,z order.
  virtual BDSExtent Extent() const;
};

#endif
