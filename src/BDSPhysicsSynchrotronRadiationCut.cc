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
#include "BDSPhysicsSynchrotronRadiationCut.hh"

#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSWrapperSynchrotronRadiationCut.hh"

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

#include <set>

BDSPhysicsSynchrotronRadiationCut::BDSPhysicsSynchrotronRadiationCut(G4double energyCutIn):
  G4VPhysicsConstructor("BDSPhysicsScintillationReduction"),
  energyCut(energyCutIn)
{
}

BDSPhysicsSynchrotronRadiationCut::~BDSPhysicsSynchrotronRadiationCut()
{;}

void BDSPhysicsSynchrotronRadiationCut::ConstructParticle()
{;}

void BDSPhysicsSynchrotronRadiationCut::ConstructProcess()
{
  if (Activated())
    {return;}

#if G4VERSION_NUMBER > 1029
  auto aParticleIterator =  G4ParticleTable::GetParticleTable()->GetIterator();
#endif
  aParticleIterator->reset();

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

  while( (*aParticleIterator)() ) {
    G4ParticleDefinition* particle = aParticleIterator->value();

    G4ProcessManager* pManager = particle->GetProcessManager();
    if (!pManager)
      continue;

    G4ProcessVector* processVector = pManager->GetProcessList();

    G4VProcess* processToWrap = nullptr;

    for (G4int i=0; i < (G4int)processVector->entries(); ++i)
    {
      G4VProcess* process = (*processVector)[i];
      if (process->GetProcessName() == "SynchrotronRadiation")
      {
        processToWrap = process;
        break;
      }
    }

    if (processToWrap)
    {
      auto wrappedProcess =
        new BDSWrapperSynchrotronRadiationCut(processToWrap, energyCut);

      pManager->RemoveProcess(processToWrap);
      ph->RegisterProcess(wrappedProcess, particle);

      G4cout << "Energy cut > synchrotron radiation> wrapping \""
             << processToWrap->GetProcessName()
             << "\" for particle \""
             << particle->GetParticleName()
             << "\" with energy cut "
             << energyCut << G4endl;
    }
  }

  SetActivated();
}

