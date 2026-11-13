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
#include "BDSColours.hh"
#include "BDSVisManager.hh"

#include "G4UIterminal.hh"
#ifdef G4UI_USE_TCSH
#include "G4UItcsh.hh"
#endif

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#ifdef G4VIS_USE
#include "G4UImanager.hh"        // G4 session managers
#endif
#include "G4UIExecutive.hh"
#endif

#include "G4String.hh"
#include "G4Types.hh"

#include "G4TrajectoryDrawByCharge.hh"
#include "G4Version.hh"

#include "BDSDebug.hh"
#include "BDSDetectorConstruction.hh"
#include "BDSMessenger.hh"
#include "BDSUtilities.hh"
#include "BDSVisCommandSceneAddQueryMagneticField.hh"

#include <string>


BDSVisManager::BDSVisManager(const G4String& visMacroFileNameIn,
                             const G4String& geant4MacroFileNameIn,
                             const BDSDetectorConstruction* realWorldIn,
                             G4int visVerbosity):
  uiSession(nullptr),
  visManager(nullptr),
  bdsMessenger(nullptr),
  visMacroFileName(visMacroFileNameIn),
  geant4MacroFileName(geant4MacroFileNameIn)
{
#ifdef G4UI_USE_TCSH
  uiSession = new G4UIterminal(new G4UItcsh);
#else
  uiSession = new G4UIterminal();
#endif
  visManager = new G4VisExecutive(std::to_string(visVerbosity));
  bdsMessenger = new BDSMessenger();
  if (realWorldIn)
    {visManager->RegisterMessenger(new BDSVisCommandSceneAddQueryMagneticField(realWorldIn));}
}

BDSVisManager::~BDSVisManager()
{
  delete uiSession;
  delete visManager;
  delete bdsMessenger;
}

void BDSVisManager::StartSession(int argc, char** argv)
{
#ifdef G4VIS_USE
  visManager->Initialize();
  
  // setup trajectory colouring
  G4TrajectoryDrawByCharge* trajModel1 = new G4TrajectoryDrawByCharge("bdsim_traj_by_charge");
  const auto colours = BDSColours::Instance();
  trajModel1->Set(G4TrajectoryDrawByCharge::Charge::Neutral,  *(colours->GetColour("traj_neutral")));
  trajModel1->Set(G4TrajectoryDrawByCharge::Charge::Positive, *(colours->GetColour("traj_positive")));
  trajModel1->Set(G4TrajectoryDrawByCharge::Charge::Negative, *(colours->GetColour("traj_negative")));
  visManager->RegisterModel(trajModel1);
  visManager->SelectTrajectoryModel(trajModel1->Name());
#endif
 
#ifdef G4UI_USE
  G4UIExecutive* uiExecutive = new G4UIExecutive(argc, argv);
#ifdef G4VIS_USE

  G4UImanager* UIManager = G4UImanager::GetUIpointer();
  // setup paths to look for macros for the installation then the build directory
  G4String bdsimExecPath = G4String(BDS::GetBDSIMExecPath());
  G4String macroPaths    = bdsimExecPath + "../share/bdsim/vis:@CMAKE_BINARY_DIR@/vis:./";
  G4cout << __METHOD_NAME__ << "Setting macro path to: " << macroPaths << G4endl;
  UIManager->ApplyCommand("/control/macroPath "+macroPaths);

  G4String visMacName = visMacroFileName;
  G4String visMacPath = visMacName; // by default just copy it
  if (visMacName.empty()) // none specified - use default in BDSIM
    {
#if G4VERSION_NUMBER > 1119
      visMacName = "bdsim_default_vis_11p2.mac";
#else
#ifdef G4VIS_USE_OPENGLQT
      visMacName = "bdsim_default_vis.mac";
#else
      visMacName = "bdsim_default_dawnfile.mac";
#endif
#endif
      // check if we find the file to at least let the user know what's being executed
      visMacPath = UIManager->FindMacroPath(visMacName);
      G4cout << __METHOD_NAME__ << "Visualisation macro name: " << visMacName << G4endl;
      G4cout << __METHOD_NAME__ << "Visualisation macro path: " << visMacPath << G4endl;
      if (visMacPath == visMacName) // this happens when the geant4 ui manager doesn't find the file in any directory
        {// behaviour found from geant4 source code inspection...
          G4cout << __METHOD_NAME__ << "bdsim_default_vis.mac missing from BDSIM installation directory." << G4endl;
          return;
        }
    }
  else
    {// user specified visualisation macro - check if it exists
      if (!BDS::FileExists(visMacPath))
        {
          std::cout << __METHOD_NAME__ << "ERROR: visualisation file " << visMacPath << " not present!" << G4endl;
          return;
        }
      G4cout << __METHOD_NAME__ << "Using visualisation macro: " << visMacName << G4endl;
    }
  // execute the macro
  UIManager->ApplyCommand("/control/execute " + visMacPath);

  // apply optional macro if file name not empty
  if (!geant4MacroFileName.empty())
    {
      G4cout << __METHOD_NAME__ << "Applying geant4MacroFileName: " << geant4MacroFileName << G4endl;
      UIManager->ApplyCommand("/control/execute " + geant4MacroFileName);
    }
  
#if G4VERSION_NUMBER < 1030
  if (uiExecutive->IsGUI())
    {// these were added by default in Geant4.10.3 onwards
      UIManager->ApplyCommand("/control/execute bdsim_default_icons.mac"); // add icons
      UIManager->ApplyCommand("/gui/addIcon \"Run beam on\" user_icon \"/run/beamOn 1\" run.png"); // add run icon
      UIManager->ApplyCommand("/control/execute bdsim_default_gui.mac");   // add menus
    }
#endif
#endif
  // run gui
  uiExecutive->SessionStart();
  delete uiExecutive;
#endif
}
