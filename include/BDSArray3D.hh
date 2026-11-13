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
#ifndef BDSARRAY3D_H
#define BDSARRAY3D_H

#include "BDSArray4D.hh"

#include "G4Types.hh"

#include <ostream>

/**
 * @brief 3D array class.
 * 
 * All functionality is provided through BDSArray4D.  A 3D array is a 
 * subset of a 4D array.
 * 
 * @author Laurie Nevay
 */

class BDSArray3D: public BDSArray4D
{
public:
  /// No default constructor as the array is not adjustable after construction and
  /// therefore the size must be known at construction time.
  BDSArray3D() = delete;
  BDSArray3D(G4int nX, G4int nY, G4int nZ);
  virtual ~BDSArray3D(){;}

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray3D const &a);
};

#endif
