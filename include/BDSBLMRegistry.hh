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
#ifndef BDSBLMREGISTRY_H
#define BDSBLMREGISTRY_H

#include "globals.hh" // geant4 types / globals

#include <set>
#include <vector>

class BDSBLM;

/**
 * @brief Registry of all BLMs so we know each one uniquely.
 * 
 * @author Laurie Nevay
 */

class BDSBLMRegistry
{  
public:
  /// Accessor for registry.
  static BDSBLMRegistry* Instance();

  ~BDSBLMRegistry();
  
  G4int RegisterBLM(G4String name,
		    BDSBLM*  blm,
		    G4double SIn = -1000);
  
  inline size_t size()  const {return (size_t)numberOfEntries;}
  inline G4int  NBLMs() const {return numberOfEntries;}
  inline const std::set<BDSBLM*>& BLMs() const {return blms;}
  
private:
  /// Private constructor to enforce singleton pattern
  BDSBLMRegistry();

  /// Singleton instance.
  static BDSBLMRegistry* instance;

  /// Counter for easy checking of out of bounds and incrementing.
  /// Also the index in the member vectors, so zero counting.
  G4int numberOfEntries;

  /// Cache of unique sampler objects for memory management.
  std::set<BDSBLM*> blms;

  std::vector<BDSBLM*>  blmsVector;
  std::vector<G4String> names;
  std::vector<G4double> S;
};

#endif
