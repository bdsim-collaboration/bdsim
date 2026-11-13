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
#ifndef BDSSCORERTYPE_H
#define BDSSCORERTYPE_H

#include "G4String.hh"
#include "G4Types.hh"

#include "BDSTypeSafeEnum.hh"

/**
 * @brief Type definition for scorers
 *
 * @author Robin Tesse
 */

struct scorertype_def
{
  enum type {cellcharge, cellcharge3d,
	     depositeddose, depositeddose3d,
	     depositedenergy, depositedenergyblm, depositedenergy3d,
	     population, population3d, populationscaled,
	     cellflux, cellflux3d,
	     cellfluxscaledperparticle, cellfluxscaledperparticle3d,
	     cellfluxscaled, cellfluxscaled3d, cellflux4d};
};

typedef BDSTypeSafeEnum<scorertype_def,int> BDSScorerType;

namespace BDS
{
  /// function to determine the enum type of the cavity geometry (case-insensitive)
  BDSScorerType DetermineScorerType(G4String scorerType);
}

#endif
