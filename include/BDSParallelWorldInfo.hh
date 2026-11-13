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
#ifndef BDSPARALLELWORLDINFO_H
#define BDSPARALLELWORLDINFO_H

#include "globals.hh"

/**
 * @brief A simple class to hold what parallel geometry is required for each sequence.
 *
 * @author Laurie Nevay
 */

class BDSParallelWorldInfo
{
public:
  BDSParallelWorldInfo();
  BDSParallelWorldInfo(const G4String& sequenceNameIn,
		       G4bool   curvilinearWorldIn,
		       G4bool   samplerWorldIn,
		       const G4String& placementNameIn = ""):
    sequenceName(sequenceNameIn),
    curvilinearWorld(curvilinearWorldIn),
    samplerWorld(samplerWorldIn),
    placementName(placementNameIn),
    curvilinearWorldName(placementNameIn+"_"+sequenceNameIn)
  {;}
  ~BDSParallelWorldInfo(){;}
  
  G4String sequenceName;         ///< Name of sequence worlds will be associated with.
  G4bool   curvilinearWorld;     ///< Whether a curvilinear world is required.
  G4bool   samplerWorld;         ///< Whether a sampler world is required.
  G4String placementName;        ///< Name of placement.
  G4String curvilinearWorldName; ///< Unique name for curvilinear world.
};

#endif
