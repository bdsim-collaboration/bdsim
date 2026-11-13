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
#ifndef BDSMAGNETGEOMETRYTYPE_H
#define BDSMAGNETGEOMETRYTYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Geometry type definition for magnets - used for comparison
 * in factory methods. 
 * 
 * @author Laurie Nevay
 */

struct magnetgeometrytypes_def
{
  enum type {none, cylindrical, polescircular, polessquare, polesfacet,
	      polesfacetcrop, lhcleft, lhcright, external};
};

typedef BDSTypeSafeEnum<magnetgeometrytypes_def, G4int> BDSMagnetGeometryType;

namespace BDS
{
  /// function to determine the enum type of the magnet geometry (case-insensitive)
  BDSMagnetGeometryType DetermineMagnetGeometryType(G4String geometryType);
}

#endif
