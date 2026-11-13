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
#ifndef BDSARRAY2D_H
#define BDSARRAY2D_H

#include "BDSArray3D.hh"

#include "G4Types.hh"

#include <ostream>

/**
 * @brief 2D array.
 * 
 * Inherits (ie is a type of) BDSArray3D and in turn BDSArray4D.
 *
 * @author Laurie Nevay
 */

class BDSArray2D: public BDSArray3D
{
public:
  BDSArray2D(G4int nXIn, G4int nYIn);
  virtual ~BDSArray2D(){;}
  
  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray2D const &a);
  
private:
  /// No default constructor as the array is not adjustable after construction and
  /// therefore the size must be known at construction time.
  BDSArray2D() = delete;
};

#endif
