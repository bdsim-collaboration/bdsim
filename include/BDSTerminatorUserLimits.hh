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
#ifndef BDSTERMINATORUSERLIMITS_H
#define BDSTERMINATORUSERLIMITS_H

#include "G4UserLimits.hh"
#include "globals.hh"  // geant4 basic stl types

#include <limits>

/**
 * @brief Dynamic user limits for a volume that changed based
 * on the parameter turnstaken in BDSGlobalConstants().
 *
 * Used to control the number of turns a particle completes around
 * a circular accelerator. An instance of these user limits is
 * typically attached to a volume near the end of the lattice.
 * 
 * @author Laurie Nevay
 */

class BDSTerminatorUserLimits: public G4UserLimits
{
public:
  /// Basic inheritance - just use everything normally from G4UserLimits but 
  /// replace one function in inherited class. Default values are defined
  /// in G4UserLimits so all particles continue.
  BDSTerminatorUserLimits(G4double ustepMax = std::numeric_limits<double>::max(),
			  G4double utrakMax = std::numeric_limits<double>::max(),
			  G4double utimeMax = std::numeric_limits<double>::max(),
			  G4double uekinMin = 0.,     
			  G4double urangMin = 0.);    
  
  BDSTerminatorUserLimits(const G4String& type,
			  G4double ustepMax = std::numeric_limits<double>::max(),
			  G4double utrakMax = std::numeric_limits<double>::max(),
			  G4double utimeMax = std::numeric_limits<double>::max(),
			  G4double uekinMin = 0.,
			  G4double urangMin = 0.);
  
  virtual ~BDSTerminatorUserLimits(){};

  /// Replace this function alone in G4UserLimits
  /// where we only set the limit to E=0 eV on the
  /// last turn.
  virtual G4double GetUserMinEkine(const G4Track&);

protected:
  G4double keeprunningEK; ///< Minimum energy particle must have to keep going
  G4double stoprunningEK; ///< Same, so everything < DBL_MAX so everything stopped

private:
  /// Number of turns in total that simulation will progress for - not changed during
  /// run or after construction.
  const G4int turnsToTake;
};

#endif
