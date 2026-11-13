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
#ifndef BDSVISMANAGER_H
#define BDSVISMANAGER_H 

#include "G4String.hh"
#include "G4Types.hh"

class BDSDetectorConstruction;
class BDSMessenger;
class G4UIsession;
class G4VisManager;

/**
 * @brief The BDS Visualisation Manager
 * 
 * This class manages the visualisation.
 * Chosen not to inherit from G4Manager, but instead a wrapper around it.
 */

class BDSVisManager
{
public:
  BDSVisManager(const G4String& visMacroFileNameIn,
                const G4String& geant4MacroFileNameIn,
                const BDSDetectorConstruction* realWorldIn = nullptr,
                G4int visVerbosity = 0);
  
  ~BDSVisManager();

  /// Start interactive mode.
  void StartSession(int argc, char** argv);

private:
  G4UIsession* uiSession;
  G4VisManager* visManager;
  BDSMessenger* bdsMessenger;
  G4String visMacroFileName;    ///< Full macro file path to run for visualisation.
  G4String geant4MacroFileName; ///< Full macro file path for optional macro to run afterwards.
};

#endif

