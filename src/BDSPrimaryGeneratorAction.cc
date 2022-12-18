/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
                                                     BDSRunAction*     runActionIn):
  bunch(bunchIn),
  runAction(runActionIn),
  recreateFile(nullptr),
  eventOffset(0),
  ionPrimary(bunchIn->BeamParticleIsAnIon()),
  distrFileMatchLength(false),
  ionCached(false),
  oneTurnMap(nullptr),
  generatorFromFile(nullptr)
{
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
  
  generatorFromFile = BDSPrimaryGeneratorFile::ConstructGenerator(beam, bunch, recreate, eventOffset);
}

BDSPrimaryGeneratorAction::~BDSPrimaryGeneratorAction()
{
  delete particleGun;
  delete recreateFile;
  delete generatorFromFile;
}

void BDSPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // load seed state if recreating.
  if (recreate)
    {
      G4cout << __METHOD_NAME__ << "setting seed state from file" << G4endl;
      BDSRandom::SetSeedState(recreateFile->SeedState(anEvent->GetEventID() + eventOffset));
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
  anEvent->SetUserInformation(eventInfo);
  eventInfo->SetSeedStateAtStart(BDSRandom::GetSeedState());

  // events from external file
  if (generatorFromFile)
    {
      G4bool distributionFinished = generatorFromFile->DistributionIsFinished(); // only happens if no looping
      G4int nGenerateRequested = BDSGlobalConstants::Instance()->NGenerate();
      if (distributionFinished && distrFileMatchLength)
	{
	  runAction->NotifyOfCompletionOfInputDistrFile(generatorFromFile->NEventsInFile(),
                                                  generatorFromFile->NEventsReadThatPassedFilters());
	  G4RunManager::GetRunManager()->AbortRun();
	  return;
	}
      else if (distributionFinished && (generatorFromFile->NEventsReadThatPassedFilters() < nGenerateRequested) )
	{
	  G4int currentEventIndex = G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEvent();
	  G4cerr << __METHOD_NAME__ << "unable to generate " << nGenerateRequested
		 << " events as fewer events passed the filters in the file." << G4endl;
	  G4cerr << currentEventIndex << " events generated" << G4endl;
	  anEvent->SetEventAborted();
	  G4EventManager::GetEventManager()->AbortCurrentEvent();
	  G4RunManager::GetRunManager()->AbortRun();
	  return;
	}
      G4bool generatedVertexOK = generatorFromFile->GeneratePrimaryVertexSafe(anEvent);
      if (!generatedVertexOK)
	{	
	  anEvent->SetEventAborted();
	  G4EventManager::GetEventManager()->AbortCurrentEvent();
	}
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
      G4cout << "Aborting this event (#" << anEvent->GetEventID() << ")" << G4endl;
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
      G4cout << __METHOD_NAME__ << "Event #" << anEvent->GetEventID()
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
  vertex->SetUserInformation(new BDSPrimaryVertexInformation(coords,
							     bunch->ParticleDefinition()));

#ifdef BDSDEBUG
  vertex->Print();
#endif
}
