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
#include "BDSPhysicsSynchRad.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AutoDelete.hh"
#include "G4Gamma.hh"
#include "G4LeptonConstructor.hh"
#include "G4OpticalPhoton.hh"
#include "G4ParticleDefinition.hh"
#include "G4SynchrotronRadiation.hh"
#include "G4Version.hh"

BDSPhysicsSynchRad::BDSPhysicsSynchRad():
  G4VPhysicsConstructor("BDSPhysicsSynchRad")
{;}

BDSPhysicsSynchRad::~BDSPhysicsSynchRad()
{;}

void BDSPhysicsSynchRad::ConstructParticle()
{
  G4LeptonConstructor::ConstructParticle();
  G4Gamma::Gamma();
  G4OpticalPhoton::OpticalPhotonDefinition();
}

void BDSPhysicsSynchRad::ConstructProcess()
{
  if (Activated())
    {return;}
  
  G4SynchrotronRadiation* synchrotron = new G4SynchrotronRadiation();
  G4AutoDelete::Register(synchrotron);
  
  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
  
#if G4VERSION_NUMBER > 1029
  auto aParticleIterator = GetParticleIterator();
#endif
  aParticleIterator->reset();
  while( (*aParticleIterator)() )
    {
      G4ParticleDefinition* particle = aParticleIterator->value(); 
      // add to charged particles
      if (particle->GetPDGCharge() != 0)
	{ph->RegisterProcess(synchrotron, particle);}
    }

  SetActivated();
}


