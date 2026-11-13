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
#ifndef BDSGEOMETRYTYPE_H
#define BDSGEOMETRYTYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Type definition for external geometry formats - 
 * used for comparison in factory methods.
 * 
 * @author Laurie Nevay
 */

struct geometrytypes_def
{
  enum type {mokka, gdml};
};

typedef BDSTypeSafeEnum<geometrytypes_def,int> BDSGeometryType;

namespace BDS
{
  /// function that gives corresponding enum value for string (case-insensitive)
  BDSGeometryType DetermineGeometryType(G4String geometryType);
}

#endif
