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
#ifndef BDSMAGNETTYPE_H
#define BDSMAGNETTYPE_H

#include "BDSTypeSafeEnum.hh"
#include <map>

/**
 * @brief Type definition for everything that inherits
 * BDSMagnet - ie is a magnet
 * 
 * @author Laurie Nevay
 */

struct magnettypes_def
{
  enum type { decapole, hkicker, vkicker, muonspoiler, octupole, quadrupole,
	      sectorbend, rectangularbend, rfcavity, sextupole, undulator,
	      solenoid, multipole, thinmultipole, dipolefringe, rmatrix, paralleltransporter};
};

typedef BDSTypeSafeEnum<magnettypes_def,int> BDSMagnetType;

#endif
