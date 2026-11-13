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
#ifndef BDSCRYSTALTYPE_H
#define BDSCRYSTALTYPE_H

#include "globals.hh" // geant4 globals / types

#include "BDSTypeSafeEnum.hh"

#include <map>

/**
 * @brief Type definition for crystal shapes.
 * 
 * @author Laurie Nevay
 */

struct crystaltypes_def
{
  enum type {box, cylinder, torus};
};

typedef BDSTypeSafeEnum<crystaltypes_def,int> BDSCrystalType;

namespace BDS
{
  /// function to determine the enum type of the cavity geometry (case-insensitive)
  BDSCrystalType DetermineCrystalType(G4String crystalType);
}

#endif
