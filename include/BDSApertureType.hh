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
#ifndef BDSAPERTURETYPE_H
#define BDSAPERTURETYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Type definition for aperture - used for comparison
 * in factory methods.
 * 
 * @author Laurie Nevay
 */

struct aperturetypes_def
{
  enum type {circular, rectangular, elliptical, lhc,
	     lhcdetailed, rectellipse, racetrack, octagonal,
	     circularvacuum, clicpcl};
};

typedef BDSTypeSafeEnum<aperturetypes_def,int> BDSApertureType;

namespace BDS
{
  /// function that gives corresponding enum value for string (case-insensitive)
  BDSApertureType DetermineApertureType(G4String apertureType);
}

#endif
