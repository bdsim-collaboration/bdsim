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
#include "BDSBunch.hh"
#include "BDSDebug.hh"
#include "BDSEventInfo.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIonDefinition.hh"
#include "BDSOutputLoader.hh"
#include "BDSParticleDefinition.hh"
#include "BDSPhysicsUtilities.hh"
#include "BDSPrimaryGeneratorAction.hh"
#include "BDSPrimaryGeneratorFile.hh"
#include "BDSPrimaryVertexInformation.hh"
#include "BDSPTCOneTurnMap.hh"
#include "BDSRunAction.hh"
#include "BDSRandom.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "parser/beam.h"

#include "CLHEP/Random/Random.h"

#include "globals.hh" // geant4 types / globals
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HEPEvtInterface.hh"
#include "G4IonTable.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleDefinition.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"

BDSPrimaryGeneratorAction::BDSPrimaryGeneratorAction(BDSBunch*         bunchIn,
                                                     const GMAD::Beam& beam,
                                                     G4bool            batchMode):
  bunch(bunchIn),
  recreateFile(nullptr),
  eventOffset(0),
  ionPrimary(false),
  distrFileMatchLength(beam.distrFileMatchLength),
  ionCached(false),
  oneTurnMap(nullptr),
  generatorFromFile(nullptr)
{
  if (!bunchIn)
    {throw BDSException(__METHOD_NAME__, "valid BDSBunch required");}
  ionPrimary = bunchIn->BeamParticleIsAnIon();
  
  particleGun  = new G4ParticleGun(1); // 1-particle gun

  writeASCIISeedState = BDSGlobalConstants::Instance()->WriteSeedState();
  recreate            = BDSGlobalConstants::Instance()->Recreate();
  useASCIISeedState   = BDSGlobalConstants::Instance()->UseASCIISeedState();

  if (recreate)
    {
      recreateFile = new BDSOutputLoader(BDSGlobalConstants::Instance()->RecreateFileName());
      eventOffset  = BDSGlobalConstants::Instance()->StartFromEvent();
      bunch->RecreateAdvanceToEvent(eventOffset);
    }

  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  particleGun->SetParticlePosition(G4ThreeVector());
  particleGun->SetParticleTime(0);
  
  generatorFromFile = BDSPrimaryGeneratorFile::ConstructGenerator(beam, bunch, recreate, eventOffset, batchMode);
}

BDSPrimaryGeneratorAction::~BDSPrimaryGeneratorAction()
{
  delete particleGun;
  delete recreateFile;
  delete generatorFromFile;
}

void BDSPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  G4int thisEventID = anEvent->GetEventID();
  
  // update the bunch distribution for which event we're on for different bunch timings
  bunch->CalculateBunchIndex(thisEventID);
  
  if (recreate) // load seed state if recreating.
    {
      G4cout << __METHOD_NAME__ << "setting seed state from file" << G4endl;
      BDSRandom::SetSeedState(recreateFile->SeedState(thisEventID + eventOffset));
      bunch->CalculateBunchIndex(thisEventID + eventOffset); // correct bunch index
    }

  // save the seed state in a file to recover potentially unrecoverable events
  if (writeASCIISeedState)
    {BDSRandom::WriteSeedState();}

  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  if (useASCIISeedState)
    {
      G4String fileName = globals->SeedStateFileName();
      BDSRandom::LoadSeedState(fileName);
    }

  // always save seed state in output
  BDSEventInfo* eventInfo = new BDSEventInfo();
  eventInfo->SetBunchIndex(bunch->CurrentBunchIndex());
  anEvent->SetUserInformation(eventInfo);
  eventInfo->SetSeedStateAtStart(BDSRandom::GetSeedState());

  // events from external file
  if (generatorFromFile)
    {
      GeneratePrimariesFromFile(anEvent);
      return; // nothing else to be done here
    }
  
  // update particle definition in the special case of an ion - can only be done here
  // and not before due to Geant4 ion information availability only at run time
  if (ionPrimary && !ionCached)
    {
      bunch->UpdateIonDefinition();
      ionCached = true;
    }

  // generate set of coordinates - internally the bunch may try many times to generate
  // coordinates with total energy above the rest mass and may throw an exception if it can't
  BDSParticleCoordsFullGlobal coords;
  
  // BDSBunch distributions based on files do not (as a principle) have the ability to filter
  // the particles they load so the number of events to generate can be predicted exactly and
  // there is no need to check on whether an event has been successfully generated here.
  try
    {coords = bunch->GetNextParticleValid();}
  catch (const BDSException& exception)
    {// we couldn't safely generate a particle -> abort
      // could be because of user input file
      anEvent->SetEventAborted();
      G4cout << exception.what() << G4endl;
      G4cout << "Aborting this event (#" << thisEventID << ")" << G4endl;
      return;
    }
  
  if (oneTurnMap)
    {
      G4bool offsetSAndOnFirstTurn = bunch->UseCurvilinearTransform();
      oneTurnMap->SetInitialPrimaryCoordinates(coords, offsetSAndOnFirstTurn);
    }
  
  particleGun->SetParticleDefinition(bunch->ParticleDefinition()->ParticleDefinition());
  
  // always update the charge - ok for normal particles; fixes purposively specified ions.
  particleGun->SetParticleCharge(bunch->ParticleDefinition()->Charge());

  // check that kinetic energy is positive and finite anyway and abort if not.
  // get the mass from the beamParticle as this takes into account any electrons
  G4double EK = coords.local.totalEnergy - bunch->ParticleDefinition()->Mass();
  if (EK <= 0)
    {
      G4cout << __METHOD_NAME__ << "Event #" << thisEventID
             << " - Particle kinetic energy smaller than 0! "
             << "This will not be tracked." << G4endl;
      anEvent->SetEventAborted();
      return;
    }

  // check the coordinates are valid
  if (!worldExtent.Encompasses(coords.global))
    {
      G4cerr << __METHOD_NAME__ << "point: " << coords.global
             << "mm lies outside the world volume with extent ("
             << worldExtent << " - event aborted!" << G4endl << G4endl;
      anEvent->SetEventAborted();
    }

#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << coords << G4endl;
#endif

  G4ThreeVector PartMomDir(coords.global.xp,coords.global.yp,coords.global.zp);
  G4ThreeVector PartPosition(coords.global.x,coords.global.y,coords.global.z);

  particleGun->SetParticlePosition(PartPosition);
  particleGun->SetParticleEnergy(EK);
  particleGun->SetParticleMomentumDirection(PartMomDir);
  particleGun->SetParticleTime(coords.global.T);

  particleGun->GeneratePrimaryVertex(anEvent);

  // set the weight
  auto vertex = anEvent->GetPrimaryVertex();
  vertex->SetWeight(coords.local.weight);

  // associate full set of coordinates with vertex for writing to output after event
  vertex->SetUserInformation(new BDSPrimaryVertexInformation(coords, bunch->ParticleDefinition()));

#ifdef BDSDEBUG
  vertex->Print();
#endif
}


void BDSPrimaryGeneratorAction::GeneratePrimariesFromFile(G4Event* anEvent)
{
  G4bool distributionFinished = generatorFromFile->DistributionIsFinished(); // only happens if no looping
  G4int nGenerateRequested = BDSGlobalConstants::Instance()->NGenerate();
  
  G4bool generatedVertexOK = false;
  if (!distributionFinished)
    {generatedVertexOK = generatorFromFile->GeneratePrimaryVertexSafe(anEvent);}

  // file finished (no more events) and we haven't generated a viable event
  if (distributionFinished && !generatedVertexOK)
    {
      G4bool endRunNow = false;
      if (distrFileMatchLength)
        {
          endRunNow = true;
          G4cout << __METHOD_NAME__ << "distribution file finished (matched in length): ending run." << G4endl;
          if (generatorFromFile->NEventsReadThatPassedFilters() == 0)
            {BDS::Warning(__METHOD_NAME__, "no events passed filters and were simulated.");}
        }
      else if (generatorFromFile->NEventsReadThatPassedFilters() < nGenerateRequested)
        {// not matching the file length specifically but requested a certain number of events
          // If the NEventsReadThatPassedFilters == nGenerateRequested then this won't happen as we won't
          // try to generate another new event beyond this and the run will end naturally without intervention here.
          endRunNow = true;
          G4int currentEventIndex = G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEvent();
          G4cerr << __METHOD_NAME__ << "unable to generate " << nGenerateRequested
                 << " events as fewer events passed the filters in the file." << G4endl;
          G4cerr << __METHOD_NAME__ << currentEventIndex << " events generated" << G4endl;
        }
      // common bit to artificially abort the event and then end the run now
      if (endRunNow)
        {
          anEvent->SetEventAborted();
          G4EventManager::GetEventManager()->AbortCurrentEvent();
          G4RunManager::GetRunManager()->AbortRun();
          return; // don't generate anything - just return
        }
    }
  else if (!generatedVertexOK) // file isn't finished, but we didn't successfully generate this event
    {   
      anEvent->SetEventAborted();
      G4EventManager::GetEventManager()->AbortCurrentEvent();
    }
}
