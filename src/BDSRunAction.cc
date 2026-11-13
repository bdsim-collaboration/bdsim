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
#include "BDSAcceleratorModel.hh"
#include "BDSAuxiliaryNavigator.hh"
#include "BDSBeamline.hh"
#include "BDSBunch.hh"
#include "BDSBunchFileBased.hh"
#include "BDSDebug.hh"
#include "BDSEventAction.hh"
#include "BDSEventInfo.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSOutput.hh"
#include "BDSParser.hh"
#include "BDSRunAction.hh"
#include "BDSSamplerPlacementRecord.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSWarning.hh"

#include "parser/beamBase.h"
#include "parser/optionsBase.h"

#include "globals.hh"               // geant4 globals / types
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4Run.hh"
#include "G4Version.hh"

#include "CLHEP/Random/Random.h"

#include <chrono>
#include <ctime>
#include <sstream>
#include <string>
#include <vector>

#if G4VERSION_NUMBER > 1049
#include "BDSPhysicsUtilities.hh"
#include "G4Positron.hh"
#include "G4Electron.hh"
#endif

BDSRunAction::BDSRunAction(BDSOutput*      outputIn,
                           BDSBunch*       bunchGeneratorIn,
                           G4bool          usingIonsIn,
                           BDSEventAction* eventActionIn,
                           const G4String& trajectorySamplerIDIn):
  output(outputIn),
  starttime(time(nullptr)),
  info(nullptr),
  bunchGenerator(bunchGeneratorIn),
  usingIons(usingIonsIn),
  cpuStartTime(std::clock_t()),
  eventAction(eventActionIn),
  trajectorySamplerID(trajectorySamplerIDIn),
  nEventsRequested(0)
{;}

BDSRunAction::~BDSRunAction()
{
  delete info;
}

void BDSRunAction::BeginOfRunAction(const G4Run* aRun)
{
  if (BDSGlobalConstants::Instance()->PrintPhysicsProcesses())
    {PrintAllProcessesForAllParticles();}

  BDSAuxiliaryNavigator::ResetNavigatorStates();
  
  // Bunch generator beginning of run action (optional mean subtraction).
  bunchGenerator->BeginOfRunAction(aRun->GetNumberOfEventToBeProcessed(), BDSGlobalConstants::Instance()->Batch());
  nEventsRequested = aRun->GetNumberOfEventToBeProcessed();

  SetTrajectorySamplerIDs();
  CheckTrajectoryOptions();
  
  info = new BDSEventInfo();
  
  // save the random engine state
  std::stringstream ss;
  CLHEP::HepRandom::saveFullState(ss);
  seedStateAtStart = ss.str();
  info->SetSeedStateAtStart(seedStateAtStart);
  
  // get the current time
  starttime = time(nullptr);
  info->SetStartTime(starttime);
  
  // Output feedback
  G4cout << __METHOD_NAME__ << "Run " << aRun->GetRunID()
         << " start. Time is " << asctime(localtime(&starttime)) << G4endl;

  output->InitialiseGeometryDependent();
  output->NewFile();

  // Write options now file open.
  const GMAD::OptionsBase* ob = BDSParser::Instance()->GetOptionsBase();
  output->FillOptions(ob);

  // Write beam
  const GMAD::BeamBase* bb = BDSParser::Instance()->GetBeamBase();
  output->FillBeam(bb);

  // Write model now file open.
  output->FillModel();

  // Write out geant4 data including particle tables.
  output->FillParticleData(usingIons);

#if G4VERSION_NUMBER > 1049
  // this apparently has to be done in the run action and doesn't work if done earlier
  BDS::FixGeant105ThreshholdsForBeamParticle(bunchGenerator->ParticleDefinition());
  BDS::FixGeant105ThreshholdsForParticle(G4Positron::Definition());
  BDS::FixGeant105ThreshholdsForParticle(G4Electron::Definition());
#endif

  cpuStartTime = std::clock();
}

void BDSRunAction::EndOfRunAction(const G4Run* aRun)
{
  // Get the current time
  time_t stoptime = time(nullptr);
  info->SetStopTime(stoptime);
  // Run duration
  G4float duration = static_cast<G4float>(difftime(stoptime, starttime));
  info->SetDurationWall(duration);

  // Calculate the elapsed CPU time for the event.
  auto cpuEndTime = std::clock();
  G4float durationCPU = static_cast<G4float>(cpuEndTime - cpuStartTime) / CLOCKS_PER_SEC;
  info->SetDurationCPU(durationCPU);
  
  // Output feedback
  G4cout << G4endl << __METHOD_NAME__ << "Run " << aRun->GetRunID() << " end. Time is " << asctime(localtime(&stoptime));
  
  // Write output
  // In the case of a file-based bunch generator, it will have cached these numbers - get them.
  unsigned long long int nOriginalEvents = nEventsRequested; // default for normal bunch class
  unsigned long long int nEventsDistrFileSkipped = 0;
  unsigned long long int nEventsInOriginalDistrFile = 0;
  unsigned int distrFileLoopNTimes = 1;
  if (auto beg = dynamic_cast<BDSBunchFileBased*>(bunchGenerator))
    {
      nOriginalEvents = beg->NOriginalEvents();
      nEventsDistrFileSkipped = beg->NEventsInFileSkipped();
      nEventsInOriginalDistrFile = beg->NEventsInFile();
      distrFileLoopNTimes = (unsigned int)beg->DistrFileLoopNTimes();
      if (nEventsDistrFileSkipped > 0)
        {G4cout << __METHOD_NAME__ << nEventsDistrFileSkipped << " events were skipped as no particles passed the filters in them." << G4endl;}
      if (nEventsDistrFileSkipped == nEventsInOriginalDistrFile)
        {
          G4String msg = "no events were simulated at all as none contained any particles that passed the filters.";
          BDS::Warning(__METHOD_NAME__, msg);
        }
    }
  output->FillRun(info, nOriginalEvents, nEventsRequested, nEventsInOriginalDistrFile, nEventsDistrFileSkipped, distrFileLoopNTimes);
  output->CloseFile();
  info->Flush();

  // note difftime only calculates to the integer second
  G4cout << __METHOD_NAME__ << "Run Duration >> " << (int)duration << " s" << G4endl;
}

void BDSRunAction::PrintAllProcessesForAllParticles() const
{
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleTable::G4PTblDicIterator* particleIterator = particleTable->GetIterator();
  particleIterator->reset();
  while ((*particleIterator)())
    {
      G4ParticleDefinition* particle = particleIterator->value();
      G4cout << "Particle: \"" << particle->GetParticleName() << "\", defined processes are: " << G4endl;
      G4ProcessManager* pManager = particle->GetProcessManager();
      if (!pManager)
        {continue;}
      G4ProcessVector* processList = pManager->GetProcessList();
      if (!processList)
        {continue;}
      for (G4int i = 0; i < (G4int)processList->size(); i++)
        {G4cout << "\"" << (*processList)[i]->GetProcessName() << "\"" << G4endl;}
      G4cout << G4endl;
    }
}

void BDSRunAction::SetTrajectorySamplerIDs() const
{
  if (trajectorySamplerID.empty())
    {return;}

  std::vector<G4int> samplerIDs;
  std::istringstream is(trajectorySamplerID);
  G4String tok;
  while (is >> tok)
    {
      BDSSamplerRegistry* samplerRegistry = BDSSamplerRegistry::Instance();
      G4int i = 0;
      G4bool found = false;
      for (const auto& samplerInfo : *samplerRegistry)
        {
          if (samplerInfo.UniqueName() == tok)
            {
              samplerIDs.push_back(i);
              found = true;
              break;
            }
          i++;
        }
      if (!found)
        {throw BDSException(__METHOD_NAME__, "Error: sampler \"" + tok + "\" named in the option storeTrajectorySamplerID was not found.");}
    }

  eventAction->SetSamplerIDsForTrajectories(samplerIDs);
}

void BDSRunAction::CheckTrajectoryOptions() const
{
  // TODO - with multiple beam lines this will have to check for the maximum S coordinate
  G4double maxS = 1*CLHEP::m; // 1m for margin
  const BDSBeamline* blm = BDSAcceleratorModel::Instance()->BeamlineMain();
  if (blm)
    {maxS += blm->GetTotalArcLength();}
  std::vector<std::pair<double,double>> sRangeToStore = BDSGlobalConstants::Instance()->StoreTrajectoryELossSRange();
  for (const auto& range : sRangeToStore)
    {
      if (range.first > maxS)
        {throw BDSException(__METHOD_NAME__, "S coordinate " + std::to_string(range.first / CLHEP::m) + "m in option storeTrajectoryElossSRange is beyond the length of the beam line (2m margin).");}
    }
}
