/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#ifndef BDSMODULATORTYPE_H
#define BDSMODULATORTYPE_H

#include "BDSTypeSafeEnum.hh"

#include "G4String.hh"

/**
 * @brief Type definition for field modulators.
 * 
 * @author Laurie Nevay
 */

struct modulatortypes_def
{
  enum type {lineart,
         none,
	     sint,
	     singlobalt,
	     tophatt};
};

typedef BDSTypeSafeEnum<modulatortypes_def,int> BDSModulatorType;

namespace BDS
{
  /// Function that gives corresponding enum value for string (case-insensitive)
  BDSModulatorType DetermineModulatorType(G4String mType);
}

#endif


