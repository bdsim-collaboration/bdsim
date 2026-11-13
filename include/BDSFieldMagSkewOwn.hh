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
#ifndef BDSFIELDSKEWOWN_H
#define BDSFIELDSKEWOWN_H

#include "BDSFieldMagSkew.hh"

#include "globals.hh" // geant4 types / globals

class BDSFieldMag;

/**
 * @brief A wrapper class for BDSFieldMagSkew where it owns the field.
 * 
 * @author Laurie Nevay
 */

class BDSFieldMagSkewOwn: public BDSFieldMagSkew
{
public:
  /// Wrap a field and rotate it by angle (rad) in the x-y plane (about z axis).
  BDSFieldMagSkewOwn(BDSFieldMag* fieldIn, G4double angle = 0);

  virtual ~BDSFieldMagSkewOwn();
  
private:
  /// Private default constructor to force use of supplied ones.
  BDSFieldMagSkewOwn() = delete;

  /// The nominal non-skewed field.
  BDSFieldMag* const field;
};

#endif
