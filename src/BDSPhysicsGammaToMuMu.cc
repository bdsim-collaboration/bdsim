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
#include "BDSPhysicsGammaToMuMu.hh"

#include "G4AutoDelete.hh"
#include "G4GammaConversionToMuons.hh"
#include "G4LeptonConstructor.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicsListHelper.hh"
#include "G4String.hh"
#include "G4Version.hh"


BDSPhysicsGammaToMuMu::BDSPhysicsGammaToMuMu():
  G4VPhysicsConstructor("BDSPhysicsGammaToMuMu")
{;}

BDSPhysicsGammaToMuMu::~BDSPhysicsGammaToMuMu()
{;}

void BDSPhysicsGammaToMuMu::ConstructParticle()
{ 
  G4LeptonConstructor::ConstructParticle();
}

void BDSPhysicsGammaToMuMu::ConstructProcess()
{
  if (Activated())
    {return;}
  
  G4GammaConversionToMuons* gammaToMuPair = new G4GammaConversionToMuons();
  G4AutoDelete::Register(gammaToMuPair);
  
  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
  
#if G4VERSION_NUMBER > 1029
  auto aParticleIterator = GetParticleIterator();
#endif
  aParticleIterator->reset();

  while( (*aParticleIterator)() )
    {
      G4ParticleDefinition* particle = aParticleIterator->value();
      G4String particleName = particle->GetParticleName();
      
      if (particleName == "gamma")
	{ph->RegisterProcess(gammaToMuPair, particle);}
    }

  SetActivated();
}
