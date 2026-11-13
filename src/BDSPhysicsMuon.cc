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
#include "BDSGlobalConstants.hh"
#include "BDSPhysicsMuon.hh"

#include "G4AutoDelete.hh"
#include "G4AnnihiToMuPair.hh"
#include "G4eeToHadrons.hh"
#include "G4Gamma.hh"
#include "G4GammaConversionToMuons.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuIonisation.hh"
#include "G4MuMultipleScattering.hh"
#include "G4MuPairProduction.hh"
#include "G4OpticalPhoton.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicsListHelper.hh"
#include "G4Version.hh"


BDSPhysicsMuon::BDSPhysicsMuon():
  BDSPhysicsMuon(false)
{;}

BDSPhysicsMuon::BDSPhysicsMuon(G4bool emWillBeUsedIn):
  G4VPhysicsConstructor("BDSPhysicsMuon"),
  emWillBeUsed(emWillBeUsedIn)
{;}

BDSPhysicsMuon::~BDSPhysicsMuon()
{;}

void BDSPhysicsMuon::ConstructParticle()
{ 
  // leptons
  G4LeptonConstructor::ConstructParticle();

  // mesons, inc. all pions
  G4MesonConstructor::ConstructParticle();

  // photons
  G4Gamma::Gamma();
  G4OpticalPhoton::OpticalPhoton();
}

void BDSPhysicsMuon::ConstructProcess()
{
  if (Activated())
    {return;}
  
  // for gamma
  G4GammaConversionToMuons* gammaToMuPair = new G4GammaConversionToMuons();
  G4AutoDelete::Register(gammaToMuPair);

  // for e+
  G4AnnihiToMuPair* ePlusToMuPair = new G4AnnihiToMuPair();
  G4AutoDelete::Register(ePlusToMuPair);
  G4eeToHadrons*    eeToHadrons   = new G4eeToHadrons();
  G4AutoDelete::Register(eeToHadrons);

  // for muon +-
  G4MuMultipleScattering* mumsc = nullptr;
  G4MuIonisation*         muion = nullptr;
  G4MuBremsstrahlung*     mubrm = nullptr;
  if (!emWillBeUsed)
    {// these are provided by em physics, so don't double register
      mumsc = new G4MuMultipleScattering();
      G4AutoDelete::Register(mumsc);
      muion = new G4MuIonisation();
      G4AutoDelete::Register(muion);
      mubrm = new G4MuBremsstrahlung();
      G4AutoDelete::Register(mubrm);
    }
  G4MuPairProduction* mupar = new G4MuPairProduction();
  G4AutoDelete::Register(mupar);
  
  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
  
#if G4VERSION_NUMBER > 1029
  auto aParticleIterator = GetParticleIterator();
#endif
  aParticleIterator->reset();

  while( (*aParticleIterator)() )
    {
      G4ParticleDefinition* particle = aParticleIterator->value();
      G4String particleName = particle->GetParticleName();
      
      if(particleName == "gamma")
	{
	  ph->RegisterProcess(gammaToMuPair, particle);
	  continue;
	}
      if(particleName == "e+")
	{
	  ph->RegisterProcess(ePlusToMuPair, particle);
	  ph->RegisterProcess(eeToHadrons,   particle);
	  continue;
	}
      if(particleName == "mu+" || particleName == "mu-")
	{
	  if (!emWillBeUsed)
	    {// these are only instantiated if we're not using EM physics
	      ph->RegisterProcess(mumsc, particle);
	      ph->RegisterProcess(muion, particle);
	      ph->RegisterProcess(mubrm, particle);
	      ph->RegisterProcess(mupar, particle);
	    }
	  continue;
	}
    }

  SetActivated();
}
