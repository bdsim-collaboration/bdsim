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
#ifndef BDSOUTPUTTYPE_H
#define BDSOUTPUTTYPE_H 

#include "BDSTypeSafeEnum.hh"
#include "globals.hh" // geant4 types / globals

/**
 * @brief Type definition for all output formats.
 */

struct outputformats_def {
  enum type {none, rootevent};
};

typedef BDSTypeSafeEnum<outputformats_def, int> BDSOutputType;

namespace BDS {
  /// Determine the output format to be used from the input string.
  BDSOutputType DetermineOutputType(G4String outputType);
}


#endif
