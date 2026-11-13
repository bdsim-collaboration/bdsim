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
#include "BDSPhysicsEMDissociation.hh"

#include "G4EMDissociation.hh"
#include "G4EMDissociationCrossSection.hh"
#include "G4Gamma.hh"
#include "G4GenericIon.hh"
#include "G4HadronInelasticProcess.hh"
#include "G4IonConstructor.hh"
#include "G4ProcessManager.hh"

#ifdef G4EMDPROCESSID
#include "G4HadronicProcessType.hh"
#endif

BDSPhysicsEMDissociation::BDSPhysicsEMDissociation():
  G4VPhysicsConstructor("BDSPhysicsEMDissociation")
{;}

void BDSPhysicsEMDissociation::ConstructParticle()
{
  G4Gamma::Gamma();
  G4GenericIon::GenericIon();

  G4IonConstructor::ConstructParticle();
}

void BDSPhysicsEMDissociation::ConstructProcess()
{
  if (Activated())
    {return;}

  G4HadronInelasticProcess* inelProcIon = new G4HadronInelasticProcess("EMD", G4GenericIon::GenericIon());
#ifdef G4EMDPROCESSID
  // in our customised Geant4 we explicitly label EMD to allow identification and biasing
  // as opposed to just general fHadronicInelastic
  inelProcIon->SetProcessSubType(G4HadronicProcessType::fEMDissociation);
#endif

  G4EMDissociationCrossSection* crossSectionData = new G4EMDissociationCrossSection();
  inelProcIon->AddDataSet(crossSectionData);

  G4EMDissociation* emdModel = new G4EMDissociation();
  emdModel->SetMaxEnergy(100*CLHEP::TeV);
  emdModel->SetMinEnergy(1*CLHEP::MeV);
  inelProcIon->RegisterMe(emdModel);

  G4ProcessManager* pmanager = G4GenericIon::GenericIon()->GetProcessManager();
  pmanager->AddDiscreteProcess(inelProcIon);
  
  SetActivated();
}
