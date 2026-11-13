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
#include "BDSParameterisationPhysics.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4FastSimulationManagerProcess.hh"
#include "G4Positron.hh"
#include "G4ProcessManager.hh"
#include "G4Version.hh"

BDSParameterisationPhysics::BDSParameterisationPhysics():
  G4VPhysicsConstructor("BDSParameterisationPhysics"),
  activated(false),
  fastSimulationManagerProcess(nullptr)
{;}

void BDSParameterisationPhysics::ConstructParticle()
{
  G4Gamma::Gamma();
  G4Electron::Electron();
  G4Positron::Positron();    
  return;
}

void BDSParameterisationPhysics::ConstructProcess()
{
  if(activated)
    {return;}
  activated=true;
  
  fastSimulationManagerProcess = new G4FastSimulationManagerProcess();
  
#if G4VERSION_NUMBER > 1029
  auto aParticleIterator = GetParticleIterator();
#endif
  aParticleIterator->reset();

  while( (*aParticleIterator)() )
    {
      G4ParticleDefinition* particle = aParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      pmanager->AddDiscreteProcess(fastSimulationManagerProcess);
    }
  return;
}


