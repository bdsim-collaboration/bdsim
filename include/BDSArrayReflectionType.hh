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
#ifndef BDSARRAYREFLECTIONTYPE_H
#define BDSARRAYREFLECTIONTYPE_H
#include "BDSTypeSafeEnum.hh"
#include "G4String.hh"

#include <ostream>
#include <set>

/**
 * @brief Type definition for array reflections
 *
 * @author Laurie Nevay
 */

struct arrayreflectiontypes_def
{
  enum type {flipsimple,    // for internal use
             flipx, flipy, flipz, flipt,
             reflectsimple, // for internal use
             reflectx, reflecty, reflectz, reflectt,
             reflectadvanced, // for internal use
             reflectxydipole,
             reflectxzdipole,
             reflectyzdipole,
             reflectzsolenoid,
             reflectxyquadrupole};
};

typedef BDSTypeSafeEnum<arrayreflectiontypes_def,int> BDSArrayReflectionType;

// We will commonly use a set of possible reflections, so we provide a convenient typedef.
typedef std::set<BDSArrayReflectionType> BDSArrayReflectionTypeSet;

namespace BDS
{
  /// Function that gives corresponding enum value for string (case-insensitive)
  BDSArrayReflectionType DetermineArrayReflectionType(G4String arrayReflectionType);
  
  /// Return a std::set of reflection types. Split string on white space.
  BDSArrayReflectionTypeSet DetermineArrayReflectionTypeSet(const G4String& arrayReflectionType);
  
  /// Return true if there's a conceptual conflict with the set of field reflections requested.
  G4bool ProblemWithArrayReflectionCombination(const BDSArrayReflectionTypeSet& setIn, G4String* details=nullptr);
}

std::ostream& operator<< (std::ostream &out, BDSArrayReflectionTypeSet const& t);

#endif


