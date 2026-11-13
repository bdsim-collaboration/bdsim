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
#ifndef BDSFIELDMAGZERO_H
#define BDSFIELDMAGZERO_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

/**
 * @brief Null B field - for special cases where we need a valid object.
 * 
 * Simply returns 0,0,0,0,0,0 for all field components.  Used for teleporter
 * or specifically written tracking routines that may ignore the 'field' but
 * still need a valid object that can be called by Geant4 in order to fit
 * into its scheme.
 *
 * @author Laurie Nevay.
 */

class BDSFieldMagZero: public BDSFieldMag
{
public:
  BDSFieldMagZero(){finiteStrength = false;}
  
  virtual ~BDSFieldMagZero(){;}

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& /*position*/,
				 const G4double       /*t*/ = 0) const
  {return G4ThreeVector(0,0,0);}
};

#endif
