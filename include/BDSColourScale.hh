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
#ifndef BDSCOLOURSCALE_H
#define BDSCOLOURSCALE_H

#include "G4Types.hh"

class G4Colour;

/**
 * @brief Base class for a colour scaler class.
 *
 * @author Laurie Nevay
 */

class BDSColourScale
{
public:
  BDSColourScale(){;}
  virtual ~BDSColourScale(){;}

  virtual G4Colour GetValue(G4double numberFromZeroToOne) const = 0;
};

#endif
