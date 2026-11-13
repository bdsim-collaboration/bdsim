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
#include "BDSPhysicsIonisation.hh"

#include "G4AntiProton.hh"
#include "G4AutoDelete.hh"
#include "G4eIonisation.hh"
#include "G4Electron.hh"
#include "G4GenericIon.hh"
#include "G4hIonisation.hh"
#include "G4ionIonisation.hh"
#include "G4KaonMinus.hh"
#include "G4KaonPlus.hh"
#include "G4MuIonisation.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"
#include "G4PhysicsListHelper.hh"
#include "G4PionMinus.hh"
#include "G4PionPlus.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"


BDSPhysicsIonisation::BDSPhysicsIonisation():
  G4VPhysicsConstructor("BDSPhysicsIonisation")
{;}

BDSPhysicsIonisation::~BDSPhysicsIonisation()
{;}

void BDSPhysicsIonisation::ConstructParticle()
{
  G4Electron::Electron();
  G4Positron::Positron();
  G4MuonMinus::MuonMinus();
  G4MuonPlus::MuonPlus();
  G4Proton::Proton();
  G4AntiProton::AntiProton();
  G4PionMinus::PionMinus();
  G4PionPlus::PionPlus();
  G4KaonMinus::KaonMinus();
  G4KaonPlus::KaonPlus();
  G4GenericIon::GenericIon();
}

void BDSPhysicsIonisation::ConstructProcess()
{
  if (Activated())
    {return;}

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

  // e+-
  auto eIonisation = new G4eIonisation();
  G4AutoDelete::Register(eIonisation);
  ph->RegisterProcess(eIonisation, G4Electron::Electron());
  ph->RegisterProcess(eIonisation, G4Positron::Positron());

  // mu+-
  auto muIonisation = new G4MuIonisation();
  G4AutoDelete::Register(muIonisation);
  ph->RegisterProcess(muIonisation, G4MuonPlus::MuonPlus());
  ph->RegisterProcess(muIonisation, G4MuonMinus::MuonMinus());

  // p pbar, pi+-, k+-
  auto hIonisation = new G4hIonisation();
  G4AutoDelete::Register(hIonisation);
  ph->RegisterProcess(hIonisation, G4Proton::Proton());
  ph->RegisterProcess(hIonisation, G4AntiProton::AntiProton());
  ph->RegisterProcess(hIonisation, G4PionMinus::PionMinus());
  ph->RegisterProcess(hIonisation, G4PionPlus::PionPlus());
  ph->RegisterProcess(hIonisation, G4KaonMinus::KaonMinus());
  ph->RegisterProcess(hIonisation, G4KaonPlus::KaonPlus());

  // ions
  G4GenericIon::GenericIon();
  auto ionIonisation = new G4ionIonisation();
  G4AutoDelete::Register(ionIonisation);
  ph->RegisterProcess(ionIonisation, G4GenericIon::GenericIon());

  SetActivated();
}
