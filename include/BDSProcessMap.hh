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
#ifndef BDSPROCESSMAP_H
#define BDSPROCESSMAP_H

#include "globals.hh" // geant4 types / globals

#include <unordered_map>

/**
 * @brief Map of all process names to IDs.
 *
 * @author L. Nevay / S. Boogert.
 */

class BDSProcessMap
{
public:
  /// Singleton accessor.
  static BDSProcessMap* Instance();

  ~BDSProcessMap();

  /// Main access method for getting the name of processes.
  G4String operator()(const G4int& type, const G4int& subType = -1) const;

  /// Despatched function to operator() for getting the name of processes.
  G4String GetProcessName(const G4int& type, const G4int& subType = -1) const
  {return operator()(type, subType);}

private:
  /// Private default constructor as singleton pattern.
  BDSProcessMap();

  /// Singleton instance
  static BDSProcessMap* instance;

  /// Typedef to make syntax more readable.
  typedef std::unordered_map<G4int, G4String> processMap;

  /// Map of main process categories
  processMap processes;

  /// Map of subprocesses mapped (again) to main processes categories. Safer
  /// than using a vector as there's not guarantee there in order by geant or
  /// contiguous.
  std::unordered_map<G4int, processMap> subProcesses;
};

#endif
