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
#ifndef BDSDIMENSIONTYPE_H
#define BDSDIMENSIONTYPE_H 

#include "BDSTypeSafeEnum.hh"
#include "globals.hh" // geant4 types / globals

#include <map>

/**
 * @brief Type definition for dimensions by name.
 */

struct dimensions_def {
  enum type {x, y, z, t};
};

typedef BDSTypeSafeEnum<dimensions_def, int> BDSDimensionType;

namespace BDS {
  /// Determine the output format to be used from the input string.
  BDSDimensionType DetermineDimensionType(G4String dimensionType);
}


#endif
