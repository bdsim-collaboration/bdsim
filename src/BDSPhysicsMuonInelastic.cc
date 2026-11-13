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
#include "BDSPhysicsMuonInelastic.hh"

#include "G4AutoDelete.hh"
#include "G4BaryonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4MuonMinus.hh"
#include "G4MuonNuclearProcess.hh"
#include "G4MuonPlus.hh"
#include "G4MuonVDNuclearModel.hh"
#include "G4PhysicsListHelper.hh"
#include "G4ShortLivedConstructor.hh"


BDSPhysicsMuonInelastic::BDSPhysicsMuonInelastic():
  G4VPhysicsConstructor("BDSPhysicsMuonInelastic")
{;}

BDSPhysicsMuonInelastic::~BDSPhysicsMuonInelastic()
{;}

void BDSPhysicsMuonInelastic::ConstructParticle()
{ 
  G4LeptonConstructor::ConstructParticle();
  G4MesonConstructor::ConstructParticle();
  G4BaryonConstructor::ConstructParticle();
  G4BosonConstructor::ConstructParticle();
  G4IonConstructor::ConstructParticle();
  G4ShortLivedConstructor::ConstructParticle();
}

void BDSPhysicsMuonInelastic::ConstructProcess()
{
  if (Activated())
    {return;}

  G4MuonNuclearProcess* muNucProcess = new G4MuonNuclearProcess();
  G4MuonVDNuclearModel* muNucModel = new G4MuonVDNuclearModel();
  muNucProcess->RegisterMe(muNucModel);
  G4AutoDelete::Register(muNucProcess);
  G4AutoDelete::Register(muNucModel);

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
  ph->RegisterProcess(muNucProcess, G4MuonPlus::MuonPlus());
  ph->RegisterProcess(muNucProcess, G4MuonMinus::MuonMinus());

  SetActivated();
}
