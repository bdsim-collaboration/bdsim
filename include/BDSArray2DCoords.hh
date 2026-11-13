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
#ifndef BDSARRAY2DCOORDS_H
#define BDSARRAY2DCOORDS_H

#include "BDSArray3DCoords.hh"
#include "BDSDimensionType.hh"

#include "G4Types.hh"

#include <ostream>

class BDSExtent;

/**
 * @brief 2D array with spatial mapping derived from BDSArray4DCoords.
 *
 * @author Laurie Nevay
 */

class BDSArray2DCoords: public BDSArray3DCoords
{
public:
  /// No default constructor as the array is not adjustable after construction and
  /// therefore the size must be known at construction time.
  BDSArray2DCoords() = delete;
  
  BDSArray2DCoords(G4int nX, G4int nY,
		   G4double xMinIn, G4double xMaxIn,
		   G4double yMinIn, G4double yMaxIn,
		   BDSDimensionType xDimensionIn = BDSDimensionType::x,
		   BDSDimensionType yDimensionIn = BDSDimensionType::y);
  virtual ~BDSArray2DCoords(){;}
  
  /// Extract 2x2 points lying around coordinate x.
  virtual void ExtractSection2x2(G4double x,
                                 G4double y,
                                 BDSFieldValue (&localData)[2][2],
                                 G4double& xFrac,
                                 G4double& yFrac) const;
  
  /// Extract 4x4 points lying around coordinate x.
  virtual void ExtractSection4x4(G4double x,
                                 G4double y,
                                 BDSFieldValue (&localData)[4][4],
                                 G4double& xFrac,
                                 G4double& yFrac) const;
  
  /// Extract nearest field value from array. z,t ignored but required for overload.
  virtual BDSFieldValue ExtractNearest(G4double x,
                                       G4double y = 0,
                                       G4double z = 0,
                                       G4double t = 0) const ;

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray2DCoords const &a);
  
  /// Return the SPATIAL (only) extent of this field without any offset. Ignores time.
  /// This override gives infinite limit in z, but accurate in x,y. TODO - check for
  /// if not in x,y,z order.
  virtual BDSExtent Extent() const;
};

#endif
