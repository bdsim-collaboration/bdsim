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
#include "BDSException.hh"
#include "BDSPhysicsMuonSplitting.hh"
#include "BDSWrapperMuonSplitting.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4PhysicsListHelper.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4String.hh"
#include "G4TrackFastVector.hh"
#include "G4Types.hh"
#include "G4Version.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <map>
#include <set>

BDSPhysicsMuonSplitting::BDSPhysicsMuonSplitting(G4int splittingFactorIn,
                                                 G4double splittingThresholdEKIn,
                                                 G4int splittingFactor2In,
                                                 G4double splittingThresholdEK2In,
                                                 G4bool excludeWeight1ParticlesIn,
                                                 G4double muonSplittingExclusionWeightIn):
  G4VPhysicsConstructor("BDSPhysicsMuonSplitting"),
  splittingFactor(splittingFactorIn),
  splittingThresholdEK(splittingThresholdEKIn),
  splittingFactor2(splittingFactor2In),
  splittingThresholdEK2(splittingThresholdEK2In),
  excludeWeight1Particles(excludeWeight1ParticlesIn),
  muonSplittingExclusionWeight(muonSplittingExclusionWeightIn)
{
  if (splittingFactorIn < 1)
    {throw BDSException(__METHOD_NAME__, "the splitting factor must be an integer 1 or greater.");}
  if (splittingFactor2In < 1)
    {throw BDSException(__METHOD_NAME__, "the splitting factor #2 must be an integer 1 or greater.");}
  G4int maxSize = G4TrackFastVectorSize/2;
  if (splittingFactorIn > maxSize)
    {
      G4String msg = "the maximum safe splitting factor is " + std::to_string(maxSize);
      msg += " based on the G4TrackFastVectorSize in Geant4";
      throw BDSException(__METHOD_NAME__, msg);
    }
  if (splittingFactor2In > maxSize)
    {
      G4String msg = "(for factor #2) the maximum safe splitting factor is " + std::to_string(maxSize);
      msg += " based on the G4TrackFastVectorSize in Geant4";
      throw BDSException(__METHOD_NAME__, msg);
    }
}

BDSPhysicsMuonSplitting::~BDSPhysicsMuonSplitting()
{;}

void BDSPhysicsMuonSplitting::ConstructParticle()
{;}

void BDSPhysicsMuonSplitting::ConstructProcess()
{
  if (Activated())
    {return;}
  
  if (excludeWeight1Particles)
    {G4cout << "Bias> muon splitting> excluding weight=1 parents from splitting." << G4endl;}

  // "hPairProduction" includes "MuPairProduction" but this is in fact nothing to do with producing
  // muons. It's for producing e+e- pairs by modelling the interaction of high energy muons. This
  // model is used to make e+e- pairs also with protons, pions, kaons. But has nothing really to
  // do with producing muons. Red herring by name.
  // I designed this to be able to find more than one process per particle that produces muons
  // but in the end it looks like only 1 really. Leave for future. Micro-overhead only at setup.
  
  // These can also potentially produce muons (from inspection of the Geant4 source code). However,
  // this is practically quite rare and (in BDSIM at least) we don't have any final state biasing
  // so keeping this in could invoke a very long loop to find more muons and likely unsuccessfully,
  // so ignore these two.
  //   {"proton", {"protonInelastic"}},
  //   {"anti_proton", {"anti_protonInelastic"}},
  std::map<G4String, std::set<G4String> > particleProcesses = {{"e+", {"AnnihiToMuPair"}},
                                                               {"pi+", {"Decay"}},
                                                               {"pi-", {"Decay"}},
                                                               {"kaon+", {"Decay"}},
                                                               {"kaon-", {"Decay"}},
                                                               {"kaon0L", {"Decay"}} };

#if G4VERSION_NUMBER > 1029
  auto aParticleIterator =  G4ParticleTable::GetParticleTable()->GetIterator();
#endif
  aParticleIterator->reset();
  
  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
  
  while( (*aParticleIterator)() )
    {
      G4ParticleDefinition* particle = aParticleIterator->value();
      G4String particleName = particle->GetParticleName();
      const auto& search = particleProcesses.find(particleName);
      if (search == particleProcesses.end())
        {continue;}
      
      // else it's in the set
      G4ProcessManager* pManager = particle->GetProcessManager();
      G4ProcessVector* processVector = pManager->GetProcessList();
      const std::set<G4String>& processNamesToLookFor = search->second;
      for (G4int i=0; i < (G4int)processVector->entries(); ++i)
        {
          G4VProcess* process = (*processVector)[i];
          G4String processName = process->GetProcessName();
          if (processNamesToLookFor.count(processName) == 0)
            {continue;}
          
          auto wrappedProcess = new BDSWrapperMuonSplitting(process,
                                                            splittingFactor,
                                                            splittingThresholdEK,
                                                            splittingFactor2,
                                                            splittingThresholdEK2,
                                                            excludeWeight1Particles,
                                                            muonSplittingExclusionWeight);
          pManager->RemoveProcess(process);
          ph->RegisterProcess(wrappedProcess, particle);
          G4cout << "Bias> muon splitting> wrapping \"" << process->GetProcessName()
                 << "\" for particle \"" << particle->GetParticleName() << "\": x" << splittingFactor
                 << " for parent Ek > " << splittingThresholdEK / CLHEP::GeV << " GeV" << G4endl;
          if (splittingFactor2 > 1)
            {
              G4cout << "Bias> muon splitting> wrapping \"" << process->GetProcessName()
                     << "\" for particle \"" << particle->GetParticleName() << "\": (2nd band) x" << splittingFactor2
                     << " for parent Ek > " << splittingThresholdEK2 / CLHEP::GeV << " GeV" << G4endl;
            }
        }
    }
  
  SetActivated();
}
