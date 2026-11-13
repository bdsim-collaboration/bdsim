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
#ifndef BDSRANDOM_H
#define BDSRANDOM_H

#include "BDSTypeSafeEnum.hh"

#include "G4String.hh"

#include <sstream>


/**
 * @brief Type definition for all random engine names.
 */
struct randomenginetypes_def
{
  enum type {hepjames, mixmax};
};

typedef BDSTypeSafeEnum<randomenginetypes_def,int> BDSRandomEngineType;

namespace BDSRandom
{
  /// Function that gives corresponding enum value for string (case-insensitive).
  BDSRandomEngineType DetermineRandomEngineType(G4String engineType);
  
  /// Construct and set the random engine.
  void CreateRandomNumberGenerator(const G4String& engineName = "hepjames");

  /// Set the initial seed. Takes the seed number from
  /// BDSGlobalConstants - if negative uses the time.
  void SetSeed();

  /// Print out seed state to G4cout.
  void PrintFullSeedState();

  /// Write the seed state out to suffix + 'seedstate.txt' in cwd.
  void WriteSeedState(const G4String& suffix = "");

  /// Get the current full seed state as a string.
  G4String GetSeedState();

  /// Load a seedstate.txt file and restore the engine to this status.
  void LoadSeedState(const G4String& inSeedFilename);

  /// Set the seed state from a string.
  void SetSeedState(const G4String& seedState);
  void SetSeedState(std::stringstream& seedState);
}

#endif
