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
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSTerminatorUserLimits.hh"

#include "globals.hh" // geant4 types / globals
#include "G4UserLimits.hh"

#include <limits>

BDSTerminatorUserLimits::BDSTerminatorUserLimits(G4double ustepMax,
						 G4double utrakMax,
						 G4double utimeMax,
						 G4double uekinMin,
						 G4double urangMin):
  G4UserLimits(ustepMax,
	       utrakMax,
	       utimeMax,
	       uekinMin,
	       urangMin),
  keeprunningEK(0.0),
  stoprunningEK(std::numeric_limits<double>::max()),
  turnsToTake(BDSGlobalConstants::Instance()->TurnsToTake())
{}

BDSTerminatorUserLimits::BDSTerminatorUserLimits(const G4String& type,
						 G4double ustepMax,
						 G4double utrakMax,
						 G4double utimeMax,
						 G4double uekinMin,
						 G4double urangMin):
  G4UserLimits(type,
	       ustepMax,
	       utrakMax,
	       utimeMax,
	       uekinMin,
	       urangMin),
  keeprunningEK(0.0),
  stoprunningEK(std::numeric_limits<double>::max()),
  turnsToTake(BDSGlobalConstants::Instance()->TurnsToTake())
{}

inline G4double BDSTerminatorUserLimits::GetUserMinEkine(const G4Track& /*trk*/)
{
  // does the number of turns passed == number of turns to take
  G4int turnsTaken = BDSGlobalConstants::Instance()->TurnsTaken();
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "turns taken : " << turnsTaken << G4endl;
#endif
  if (turnsTaken >= turnsToTake)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Requested number of turns completed - stopping all particles" << G4endl;
#endif
      return stoprunningEK;
    } // yes - stop: return DBL_MAX eV so no particles will be tracked
  else
    {
      return keeprunningEK;
    } // no  - keep going: return 0 eV so all are tracked
}


