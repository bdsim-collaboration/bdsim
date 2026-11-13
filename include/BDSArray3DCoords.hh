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
#ifndef BDSARRAY3DCOORDS_H
#define BDSARRAY3DCOORDS_H

#include "BDSArray4DCoords.hh"

#include "G4Types.hh"

#include <ostream>

/**
 * @brief 3D array with spatial mapping derived from BDSArray4DCoords.
 *
 * Internally dimensions are x,y,z but these can represent different
 * coordinates. Hence the possibility of naming with BDSDimensionType.
 *
 * @author Laurie Nevay
 */

class BDSArray3DCoords: public BDSArray4DCoords
{
public:
  /// No default constructor as the array is not adjustable after construction and
  /// therefore the size must be known at construction time.
  BDSArray3DCoords() = delete;

  BDSArray3DCoords(G4int nX, G4int nY, G4int nZ,
		   G4double xMinIn, G4double xMaxIn,
		   G4double yMinIn, G4double yMaxIn,
		   G4double zMinIn, G4double zMaxIn,
		   BDSDimensionType xDimensionIn = BDSDimensionType::x,
		   BDSDimensionType yDimensionIn = BDSDimensionType::y,
		   BDSDimensionType zDimensionIn = BDSDimensionType::z);
  virtual ~BDSArray3DCoords(){;}
  
  /// Extract 2x2x2 points lying around coordinate x.
  virtual void ExtractSection2x2x2(G4double x,
                                   G4double y,
                                   G4double z,
                                   BDSFieldValue (&localData)[2][2][2],
                                   G4double& xFrac,
                                   G4double& yFrac,
                                   G4double& zFrac) const;
  
  /// Extract 4x4x4 points lying around coordinate x.
  virtual void ExtractSection4x4x4(G4double x,
                                   G4double y,
                                   G4double z,
                                   BDSFieldValue (&localData)[4][4][4],
                                   G4double& xFrac,
                                   G4double& yFrac,
                                   G4double& zFrac) const;
  
  /// Extract nearest field value from array. t ignored but required for overload.
  virtual BDSFieldValue ExtractNearest(G4double x,
                                       G4double y = 0,
                                       G4double z = 0,
                                       G4double t = 0) const;

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray3DCoords const &a);
};

#endif
