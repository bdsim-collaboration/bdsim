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
#include "BDSRunManager.hh"
#include "BDSDebug.hh"
#include "BDSDetectorConstruction.hh"
#include "BDSExceptionHandler.hh"
#include "BDSExtent.hh"
#include "BDSFieldQuery.hh"
#include "BDSPrimaryGeneratorAction.hh"

#include "G4UImanager.hh"

#include "CLHEP/Random/Random.h"

BDSRunManager::BDSRunManager()
{
  // Construct an exception handler to catch Geant4 aborts.
  // This has to be done after G4RunManager::G4RunManager() which constructs
  // its own default exception handler which overwrites the one in G4StateManager
  exceptionHandler = new BDSExceptionHandler();
}

BDSRunManager::~BDSRunManager()
{
  delete exceptionHandler;
}

void BDSRunManager::Initialize()
{
  G4RunManager::Initialize();

  BDSExtent worldExtent;
  if (const auto detectorConstruction = dynamic_cast<BDSDetectorConstruction*>(userDetector))
    {
      worldExtent = detectorConstruction->WorldExtent();

      /// Check for any 3D field queries of the model and carry them out
      const auto& fieldQueries = detectorConstruction->FieldQueries();
      if (!fieldQueries.empty())
        {
          BDSFieldQuery querier;
          querier.QueryFields(fieldQueries);
        }
    }
  if (const auto primaryGeneratorAction = dynamic_cast<BDSPrimaryGeneratorAction*>(userPrimaryGeneratorAction))
    {primaryGeneratorAction->SetWorldExtent(worldExtent);}
}

void BDSRunManager::BeamOn(G4int n_event,const char* macroFile,G4int n_select)
{
  G4RunManager::BeamOn(n_event,macroFile,n_select);
}

void BDSRunManager::DoEventLoop(G4int n_event,const char* macroFile,G4int n_select)
{
  // save event loop state
  if (verboseLevel > 0)
    {
      // Print seed to try and recreate an event in a run 
      G4cout << __METHOD_NAME__ << "Random number generator's seed=" 
             << CLHEP::HepRandom::getTheSeed() << G4endl;
      // Print generator full state to output 
      G4cout << __METHOD_NAME__ << "Random number generator's state: " << G4endl;
      CLHEP::HepRandom::saveFullState(G4cout);
    }
  
  G4RunManager::DoEventLoop(n_event, macroFile, n_select);
}

void BDSRunManager::ProcessOneEvent(G4int i_event)
{
  // additional output
  if (verboseLevel > 3)
    {
      G4cout << __METHOD_NAME__ << "Event="<<i_event<<G4endl;
      // Print seed to try and recreate an event in a run
      G4cout << __METHOD_NAME__ << "Random number generator's seed=" 
             << CLHEP::HepRandom::getTheSeed() << G4endl;
      // Print generator full state to output 
      G4cout << __METHOD_NAME__ << "Random number generator's state: " << G4endl;
      CLHEP::HepRandom::saveFullState(G4cout);
    }
  
  //G4RunManager::ProcessOneEvent(i_event);
  
  // This is the same as in G4RunManager, but we check the aborted event after the primary generator action
  currentEvent = GenerateEvent(i_event);
  if (currentEvent->IsAborted())
    {return;}
  eventManager->ProcessOneEvent(currentEvent);
  AnalyzeEvent(currentEvent);
  UpdateScoring();
  if (i_event < n_select_msg)
    {G4UImanager::GetUIpointer()->ApplyCommand(msgText);}
}

void BDSRunManager::AbortRun(G4bool)
{
  G4cout << "Terminate run - trying to write and close output file" << G4endl;
  G4RunManager::AbortRun();
}
