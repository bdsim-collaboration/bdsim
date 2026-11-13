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
#ifndef BDSFIELDFORMAT_H
#define BDSFIELDFORMAT_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"

/**
 * @brief Type definition for field file formats.
 * 
 * @author Laurie Nevay
 */

struct fieldformats_def
{
  enum type
    {
      none,
      bdsim1d, bdsim2d, bdsim3d, bdsim4d,
      poisson2d, poisson2dquad, poisson2ddipole
    };
};

typedef BDSTypeSafeEnum<fieldformats_def,int> BDSFieldFormat;

namespace BDS
{
  /// Function that gives corresponding enum value for string (case-insensitive)
  BDSFieldFormat DetermineFieldFormat(G4String fieldformat);

  /// Report the number of dimensions for that format.
  G4int NDimensionsOfFieldFormat(const BDSFieldFormat& ff);
}

#endif
