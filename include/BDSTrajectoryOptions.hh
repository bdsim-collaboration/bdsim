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
#ifndef BDSTRAJECTORYOPTIONS_H
#define BDSTRAJECTORYOPTIONS_H

#include "G4Types.hh"

/**
 * @brief Holder for trajectory storage options.
 * 
 * @author Laurie Nevay
 */

namespace BDS
{
  struct TrajectoryOptions
  {
    // no default values so we can do aggregate initialisation in C++11 and GCC4.9
    G4bool suppressTransportationSteps;
    G4bool storeKineticEnergy;
    G4bool storeMomentumVector;
    G4bool storeProcesses;
    G4bool storeTime;
    G4bool storeLocal;
    G4bool storeLinks;
    G4bool storeIon;
    G4bool storeMaterial;
  };
}
  
#endif
