/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSPhysicsCutsAndLimits.hh"
#include "BDSProcessUserSpecialCutsPDG.hh"

#include "globals.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"
#include "G4StepLimiter.hh"
#include "G4String.hh"
#include "G4Types.hh"
#include "G4UserSpecialCuts.hh"
#include "G4Version.hh"

#include <set>

BDSPhysicsCutsAndLimits::BDSPhysicsCutsAndLimits(const std::set<G4int>& pdgsToExcludeFromCuts):
  G4VPhysicsConstructor("BDSPhysicsCutsAndLimits"),
  useParticleExclusionFromCuts(!pdgsToExcludeFromCuts.empty())
{
  stepLimiter = new G4StepLimiter();
  specialCuts = new G4UserSpecialCuts();
  bdsSpecialCuts = new BDSProcessUserSpecialCutsPDG(pdgsToExcludeFromCuts);
  if (useParticleExclusionFromCuts)
    {
      G4cout << __METHOD_NAME__ << "Excluding the following particle IDs from minimumKineticEnergy, minimumRange, maximumTrackLength cuts\n{ ";
      for (const auto& pdgID : pdgsToExcludeFromCuts)
        {G4cout << pdgID << " ";}
      G4cout << "}" << G4endl;
    }
}

BDSPhysicsCutsAndLimits::~BDSPhysicsCutsAndLimits()
{
  delete stepLimiter;
  delete specialCuts;
  delete bdsSpecialCuts;
}

void BDSPhysicsCutsAndLimits::ConstructParticle()
{
  G4Gamma::Gamma();
  G4Electron::Electron();
  G4Positron::Positron();
  G4Proton::Proton();
}

void BDSPhysicsCutsAndLimits::ConstructProcess()
{
  if (Activated())
    {return;}

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

  auto cutsProcess = useParticleExclusionFromCuts ? bdsSpecialCuts : specialCuts;
  
#if G4VERSION_NUMBER > 1029
  auto aParticleIterator = GetParticleIterator();
#endif
  aParticleIterator->reset();
  while( (*aParticleIterator)())
    {
      G4ParticleDefinition* particle = aParticleIterator->value();

      // Flag as applying range production cuts. These only ever
      // apply in geant4 to gamma, e+- and proton
      std::set<G4String> rangeApplicableParticles = {"gamma", "e-", "e+", "proton"};
      if (rangeApplicableParticles.count(particle->GetParticleName()) == 1) // if in the set
        {particle->SetApplyCutsFlag(true);}
      
      // apply general cuts processes to all particles
      ph->RegisterProcess(stepLimiter, particle); // this is for MaxAllowedStep
      ph->RegisterProcess(cutsProcess, particle); // this is for all other limits
    }

  SetActivated();
}
