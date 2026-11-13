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
#include "BDSPhysicsRadioactivation.hh"

#include "G4GenericIon.hh"
#include "G4EmParameters.hh"
#include "G4LossTableManager.hh"
#include "G4NuclideTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicsListHelper.hh"
#include "G4Radioactivation.hh"
#include "G4Types.hh"
#include "G4UAtomicDeexcitation.hh"
#include "G4VAtomDeexcitation.hh"

BDSPhysicsRadioactivation::BDSPhysicsRadioactivation(G4bool atomicRearrangementIn):
  G4VPhysicsConstructor("BDSPhysicsRadioactivation"),
  atomicRearrangement(atomicRearrangementIn)
{;}

BDSPhysicsRadioactivation::~BDSPhysicsRadioactivation()
{;}

void BDSPhysicsRadioactivation::ConstructParticle()
{ 
  G4GenericIon::GenericIon();
}

void BDSPhysicsRadioactivation::ConstructProcess()
{
  if (Activated())
    {return;}

  G4Radioactivation* ra = new G4Radioactivation();
  //G4AutoDelete::Register(ra); // this cause a double deletion error somehow
  
  // atomic rearrangement
  ra->SetARM(atomicRearrangement);

  // initialise atomic deexcitation
  G4LossTableManager* man = G4LossTableManager::Instance();
  G4VAtomDeexcitation* ad = man->AtomDeexcitation();
  if (!ad)
    {
      G4EmParameters::Instance()->SetAuger(true);
      ad = new G4UAtomicDeexcitation();
      ad->InitialiseAtomicDeexcitation();
      man->SetAtomDeexcitation(ad);
    }
  
  G4PhysicsListHelper::GetPhysicsListHelper()->RegisterProcess(ra, G4GenericIon::GenericIon());

  SetActivated();
}

#endif
