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
#ifndef BDSEVENTGENERATORFILETYPE_H
#define BDSEVENTGENERATORFILETYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Type definition for event generator file types.
 * 
 * @author Laurie Nevay
 */

struct eventgeneratorfiletypes_def
{
  enum type {hepmc2, hepmc3, hpe, root, treeroot, lhef};
};

typedef BDSTypeSafeEnum<eventgeneratorfiletypes_def,int> BDSEventGeneratorFileType;

namespace BDS
{
  /// Function that gives corresponding enum value for string (case-insensitive).
  BDSEventGeneratorFileType DetermineEventGeneratorFileType(G4String distrType);
}

#endif
