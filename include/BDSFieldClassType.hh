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
#ifndef BDSFIELDCLASSTYPE_H
#define BDSFIELDCLASSTYPE_H

#include "BDSFieldType.hh"
#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Type definition for general field class. Maps to a 
 * BDSFieldType enum.
 *
 * 'irregular' fields are typically dummy field objects necessary for
 * certain program mechanics or special cases, such as the teleporter.
 * 
 * @author Laurie Nevay
 */

struct fieldclasstypes_def
{
  enum type {magnetic, electric, electromagnetic, irregular};
};

typedef BDSTypeSafeEnum<fieldclasstypes_def,int> BDSFieldClassType;

namespace BDS
{
  /// Function that gives the corresponding enum value for a field type enum.
  BDSFieldClassType DetermineFieldClassType(BDSFieldType fieldType);
}

#endif


