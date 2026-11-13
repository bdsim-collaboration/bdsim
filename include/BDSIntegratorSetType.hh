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
#ifndef BDSINTEGRATORSETTYPE_H
#define BDSINTEGRATORSETTYPE_H

#include "BDSTypeSafeEnum.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Version.hh"

/**
 * @brief Type definition for integrator sets.
 *
 * @author Laurie Nevay
 */

struct integratorsettype_def
{
  enum type {geant4, bdsimone, bdsimtwo, bdsimmatrix, bdsimmatrixfringescaling
#if G4VERSION_NUMBER > 1039
	     ,
	     geant4dp
#endif
  };
  G4bool isMatrixIntegrator;
};

typedef BDSTypeSafeEnum<integratorsettype_def, int> BDSIntegratorSetType;

namespace BDS
{
  /// Function that gives corresponding enum value for string (case-insensitive)
  BDSIntegratorSetType DetermineIntegratorSetType(G4String integratorSet);
}

#endif
