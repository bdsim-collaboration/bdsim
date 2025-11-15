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
#include "BDSPhysicsPionExtendedDecays.hh"

#include "G4DecayTable.hh"
#include "G4PhaseSpaceDecayChannel.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4Version.hh"


BDSPhysicsPionExtendedDecays::BDSPhysicsPionExtendedDecays():
  G4VPhysicsConstructor("BDSPhysicsPionExtendedDecays")
{;}

BDSPhysicsPionExtendedDecays::~BDSPhysicsPionExtendedDecays()
{;}

void BDSPhysicsPionExtendedDecays::ConstructParticle()
{
  G4PionPlus::Definition();
  G4PionMinus::Definition();
}

void BDSPhysicsPionExtendedDecays::ConstructProcess()
{
  if (Activated())
    {return;}

  G4ParticleTable* pTable = G4ParticleTable::GetParticleTable();

  // pi+
  G4ParticleDefinition* piplus = pTable->FindParticle("pi+");
  if (piplus)
    {
      G4DecayTable* ppdt = piplus->GetDecayTable();
      // pi+ -> gamma, mu+, nu_mu
      auto* ppmode2 = new G4PhaseSpaceDecayChannel("pi+", 2.00e-4, 3, "gamma", "mu+", "nu_mu");
      // pi+ -> e+, nu_e
      auto* ppmode3 = new G4PhaseSpaceDecayChannel("pi+", 1.23e-4, 2, "e+", "nu_e");
      G4double ppnewBR = ppdt->GetDecayChannel(0)->GetBR() - 2.00e-4 - 1.23e-4; // they're ordered internally biggest first
      ppdt->GetDecayChannel(0)->SetBR(ppnewBR);
      ppdt->Insert(ppmode2); // takes ownership
      ppdt->Insert(ppmode3);
    }

  // pi-
  G4ParticleDefinition* piminus = pTable->FindParticle("pi-");
  if (piminus)
    {
      G4DecayTable* pmdt = piminus->GetDecayTable();
      // pi- -> gamma, mu-, nu_mu
      auto* pnmode2 = new G4PhaseSpaceDecayChannel("pi-", 2.00e-4, 3, "gamma", "mu-", "anti_nu_mu");
      // pi+ -> e+, nu_e
      auto* pnmode3 = new G4PhaseSpaceDecayChannel("pi-", 1.23e-4, 2, "e-", "anti_nu_e");
      G4double pnnewBR = pmdt->GetDecayChannel(0)->GetBR() - 2.00e-4 - 1.23e-4;
      pmdt->GetDecayChannel(0)->SetBR(pnnewBR);
      pmdt->Insert(pnmode2);
      pmdt->Insert(pnmode3);
    }

  SetActivated();
}


