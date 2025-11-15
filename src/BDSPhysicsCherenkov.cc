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
#include "BDSException.hh"
#include "BDSPhysicsCherenkov.hh"

#include "G4AutoDelete.hh"
#include "G4Cerenkov.hh"
#include "G4Gamma.hh"
#include "G4LeptonConstructor.hh"
#include "G4OpticalPhoton.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicsListHelper.hh"
#include "G4Version.hh"


BDSPhysicsCherenkov::BDSPhysicsCherenkov(G4int    maxPhotonsPerStepIn,
                                         G4double maxBetaChangePerStepIn):
  G4VPhysicsConstructor("BDSPhysicsCherenkov"),
  maxPhotonsPerStep(maxPhotonsPerStepIn),
  maxBetaChangePerStep(maxBetaChangePerStepIn)
{
  if (maxBetaChangePerStep > 100.0)
    {throw BDSException(__METHOD_NAME__, "the option 'maxBetaChangePerStep' must be less than 100 %");}
  if (maxBetaChangePerStep < 0)
    {throw BDSException(__METHOD_NAME__, "the option 'maxBetaChangePerStep' must be >= 0 %");}
}

BDSPhysicsCherenkov::~BDSPhysicsCherenkov()
{;}

void BDSPhysicsCherenkov::ConstructParticle()
{
  G4LeptonConstructor leptons;
  leptons.ConstructParticle();

  G4Gamma::Gamma();
  G4OpticalPhoton::OpticalPhotonDefinition();
}

void BDSPhysicsCherenkov::ConstructProcess()
{
  if (Activated())
    {return;}
  
  G4Cerenkov* cherenkov = new G4Cerenkov();
  G4AutoDelete::Register(cherenkov);

  // reduce memory profile
  cherenkov->SetTrackSecondariesFirst(true);

  // common settings (similar to optical physics)
  if (maxPhotonsPerStep > 0)
    {cherenkov->SetMaxNumPhotonsPerStep(maxPhotonsPerStep);}
  cherenkov->SetMaxBetaChangePerStep(maxBetaChangePerStep);

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
    
#if G4VERSION_NUMBER > 1029
  auto aParticleIterator = GetParticleIterator();
#endif
  aParticleIterator->reset();
  while( (*aParticleIterator)() )
    {
      G4ParticleDefinition* particle = aParticleIterator->value();
      
      if (cherenkov->IsApplicable(*particle))
	{ph->RegisterProcess(cherenkov, particle);}
    }

  SetActivated();
}
