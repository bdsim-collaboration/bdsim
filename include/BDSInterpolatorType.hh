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
#ifndef BDSINTERPOLATORTYPE_H
#define BDSINTERPOLATORTYPE_H

#include "BDSTypeSafeEnum.hh"

#include "globals.hh" // geant4 types / globals

/**
 * @brief Type definition for interpolator.
 *
 * Additionally, some utility functions for automatic behaviours.
 * 
 * @author Laurie Nevay
 */

struct interpolatortypes_def
{
  enum type
    {
      none,
      nearestauto, linearauto, linearmagauto, cubicauto,
      nearest1d, linear1d, linearmag1d, cubic1d,
      nearest2d, linear2d, linearmag2d, cubic2d,
      nearest3d, linear3d, linearmag3d, cubic3d,
      nearest4d, linear4d, linearmag4d, cubic4d
    };
};

typedef BDSTypeSafeEnum<interpolatortypes_def,int> BDSInterpolatorType;

namespace BDS
{
  /// Function that determines enum from string (case-insensitive).
  BDSInterpolatorType DetermineInterpolatorType(G4String interpolatorType);

  /// Report the number of dimensions for that interpolator type.
  G4int NDimensionsOfInterpolatorType(const BDSInterpolatorType& it);

  /// Return true if the type is one containing 'auto'.
  G4bool InterpolatorTypeIsAuto(BDSInterpolatorType typeIn);

  /// Return a specific inteprolator type from the appropriate auto one in combination
  /// with the number of dimensions. e.g. 3, cubicauto -> cubic3d.
  BDSInterpolatorType InterpolatorTypeSpecificFromAuto(G4int               nDimension,
						       BDSInterpolatorType autoType);
}

#endif
