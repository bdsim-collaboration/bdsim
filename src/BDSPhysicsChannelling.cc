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
#include "G4Version.hh"
#if G4VERSION_NUMBER > 1039
#include "BDSGlobalConstants.hh"
#include "BDSPhysicsChannelling.hh"

#include "G4AutoDelete.hh"
#include "G4BaryonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4Channeling.hh"
#include "G4Gamma.hh"
#include "G4GenericIon.hh"
#include "G4IonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"

BDSPhysicsChannelling::BDSPhysicsChannelling():
  G4VPhysicsConstructor("BDSPhysicsChannelling")
{;}

void BDSPhysicsChannelling::ConstructParticle()
{
  G4BosonConstructor  pBosonConstructor;
  pBosonConstructor.ConstructParticle();
  
  G4LeptonConstructor pLeptonConstructor;
  pLeptonConstructor.ConstructParticle();
  
  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();
  
  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();
  
  G4IonConstructor pIonConstructor;
  pIonConstructor.ConstructParticle();
}

void BDSPhysicsChannelling::ConstructProcess()
{
  if (Activated())
    {return;}

  G4Channeling* channelling = new G4Channeling();
  G4AutoDelete::Register(channelling);
  //#if G4VERSION_NUMBER > 1049 || CUSTOMCHANNELING
#if CUSTOMCHANNELING
  channelling->SetMinimumEnergy(BDSGlobalConstants::Instance()->MinimumKineticEnergy());
#endif

#if G4VERSION_NUMBER > 1029
  auto aParticleIterator = GetParticleIterator();
#endif
  aParticleIterator->reset();
  
  while((*aParticleIterator)())
    {
      G4ParticleDefinition* particle = aParticleIterator->value();
      G4double particleCharge = particle->GetPDGCharge();
      
      if (particleCharge !=0)
	{
	  G4ProcessManager* pManager = particle->GetProcessManager();
	  pManager->AddDiscreteProcess(channelling);
	}
    }
  
  SetActivated();
}

#endif
