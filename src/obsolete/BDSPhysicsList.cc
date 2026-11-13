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
#include <iomanip>
#include <vector>
#include <algorithm>

#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSPhysicsList.hh"

#include "globals.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4FastSimulationManagerProcess.hh"
#include "G4LossTableManager.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4ios.hh"
#include "QGSP_BERT.hh"
#include "QGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4Decay.hh"
#include "G4eeToHadrons.hh"

#include "G4EmStandardPhysics.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"

// physics processes

//
// EM
//

#include "G4SynchrotronRadiation.hh"

// gamma
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4GammaConversionToMuons.hh"
#include "G4PhotoElectricEffect.hh"
#include "BDSGammaConversion_LPB.hh" //added by M.D. Salt, R.B. Appleby, 15/10/09

// charged particles
#include "G4eMultipleScattering.hh"
#include "G4MuMultipleScattering.hh"
#include "G4hMultipleScattering.hh"

//Optical processes
#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpMieHG.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4CoulombScattering.hh"

// e
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"
#include "BDSeBremsstrahlung_LPB.hh" //added by M.D. Salt, R.B.Appleby,15/10/09

// mu
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4MuonVDNuclearModel.hh"

//ions
#include "G4hIonisation.hh"
#include "G4ionIonisation.hh"


//Decay of unstable particles
#include "G4Decay.hh"

//
// Low EM
//

//gamma
#include "G4RayleighScattering.hh"
#include "G4LivermoreRayleighModel.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4LivermorePhotoElectricModel.hh"
#include "G4ComptonScattering.hh"
#include "G4LivermoreComptonModel.hh"
#include "G4GammaConversion.hh"
#include "G4LivermoreGammaConversionModel.hh"

//e
#include "G4eIonisation.hh"
#include "G4LivermoreIonisationModel.hh"
#include "G4UniversalFluctuation.hh"

#include "G4eBremsstrahlung.hh"
#include "G4LivermoreBremsstrahlungModel.hh"

#include "G4AnnihiToMuPair.hh"

//ions
#include "BDSLaserCompton.hh"
#include "G4StepLimiter.hh"
#include "G4UserSpecialCuts.hh"

//
// Hadronic
//
#include "G4TheoFSGenerator.hh"
#include "G4GeneratorPrecompoundInterface.hh"
#include "G4QGSModel.hh"
#include "G4GammaParticipants.hh"
#include "G4QGSMFragmentation.hh"
#include "G4ExcitedStringDecay.hh"

#include "G4PhotoNuclearProcess.hh"
#include "G4ElectronNuclearProcess.hh"
#include "G4PositronNuclearProcess.hh"

//Planck scattering
#include "BDSPlanckScatterBuilder.hh"

//
// particle definition
//

// Bosons
#include "G4ChargedGeantino.hh"
#include "G4Geantino.hh"
#include "G4Gamma.hh"
#include "G4OpticalPhoton.hh"

// leptons
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoMu.hh"

#include "G4TauPlus.hh"
#include "G4TauMinus.hh"
#include "G4NeutrinoTau.hh"
#include "G4AntiNeutrinoTau.hh"

#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"

// Hadrons
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"

//ShortLived
#include "G4ShortLivedConstructor.hh"

BDSPhysicsList::BDSPhysicsList(G4String physicsListNameIn):
  physicsListName(physicsListNameIn)
{
  physicsListNames = {"QGSP_BERT",
		      "QGSP_BERT_muon",
		      "QGSP_BERT_HP",
		      "QGSP_BERT_HP_muon",
		      "QGSP_BERT_HP_muon_em_low",
		      "livermore",
		      "penelope",
		      "G4EmStandard",
		      "standard",
		      "em_standard",
		      "em_single_scatter",
		      "merlin",
		      "em_low",
		      "em_muon",
		      "hadronic_standard",
		      "hadronic_muon",
		      "hadronic_QGSP_BERT",
		      "hadronic_QGSP_BERT_muon",
		      "hadronic_FTFP_BERT",
		      "hadronic_QGSP_BERT_HP_muon",
		      "hadronic_FTFP_BERT_muon",
		      "lw"};
  
  theCerenkovProcess           = nullptr;
  theScintillationProcess      = nullptr;
  theAbsorptionProcess         = nullptr;
  theRayleighScatteringProcess = nullptr;
  theMieHGScatteringProcess    = nullptr;
  theBoundaryProcess           = nullptr;

  theReferenceHadronicPhysList = nullptr;
  theReferenceEmPhysList       = nullptr;
  theBDSIMPhysList             = nullptr;
  theHadPhysList1              = nullptr;
  theHadPhysList2              = nullptr;

  verbose = BDSGlobalConstants::Instance()->Verbose();

  // construct particles

  defaultCutValue = BDSGlobalConstants::Instance()->DefaultRangeCut()*CLHEP::m;
  SetDefaultCutValue(BDSGlobalConstants::Instance()->DefaultRangeCut()*CLHEP::m);

  G4cout  << __METHOD_NAME__ << "Charged Thresholdcut = " 
	  << BDSGlobalConstants::Instance()->ThresholdCutCharged()/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout  << __METHOD_NAME__ << "Photon Thresholdcut  = " 
	  << BDSGlobalConstants::Instance()->ThresholdCutPhotons()/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout  << __METHOD_NAME__ << "Default range cut    = " 
	  << BDSGlobalConstants::Instance()->DefaultRangeCut()/CLHEP::m<<" m"<<G4endl;

  //This is the GEANT4 physics list verbose level.
  SetVerboseLevel(1);   
}

BDSPhysicsList::~BDSPhysicsList()
{
  delete theReferenceHadronicPhysList;
  delete theReferenceEmPhysList;
  delete theBDSIMPhysList;
  delete theHadPhysList1;
  delete theHadPhysList2;
}

void BDSPhysicsList::ConstructProcess()
{ 
#if BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  
  if (std::find(physicsListNames.begin(), physicsListNames.end(), physicsListName) == physicsListNames.end())
    {
      G4cerr << __METHOD_NAME__ << "unknown physics list " << physicsListName << G4endl;
      G4cout << "Possible physics lists are:" << G4endl;
      for (std::vector<G4String>::iterator it = physicsListNames.begin(); it != physicsListNames.end(); ++it)
	{G4cout << "\"" << *it << "\"" << G4endl;}
      exit(1);
    }

  bool plistFound=false;
  //standard physics lists
  if(physicsListName.contains("QGSP_BERT")){
    theReferenceHadronicPhysList = new G4HadronPhysicsQGSP_BERT();     
    theReferenceEmPhysList = new G4EmStandardPhysics();
    theReferenceHadronicPhysList->ConstructProcess();
    theReferenceEmPhysList->ConstructProcess();
    plistFound=true;
  } else if(physicsListName.contains("QGSP_BERT_HP")){
    theReferenceHadronicPhysList = new G4HadronPhysicsQGSP_BERT_HP();
    theReferenceHadronicPhysList->ConstructProcess();
    theReferenceEmPhysList = new G4EmStandardPhysics();
    theReferenceEmPhysList->ConstructProcess();
    plistFound=true;
  } else if (physicsListName.contains("QGSP_BERT_HP_muon")){ //Modified standard physics lists
    theReferenceHadronicPhysList = new G4HadronPhysicsQGSP_BERT_HP();
    theReferenceHadronicPhysList->ConstructProcess();
    ConstructMuon();
    plistFound=true;
  } else if (physicsListName.contains("QGSP_BERT_muon")){
    theReferenceHadronicPhysList = new G4HadronPhysicsQGSP_BERT();
    theReferenceHadronicPhysList->ConstructProcess();
    ConstructMuon();
    plistFound=true;
  } else if(physicsListName.contains("QGSP_BERT_HP_muon_em_low")){
    theReferenceHadronicPhysList = new G4HadronPhysicsQGSP_BERT_HP();
    theReferenceHadronicPhysList->ConstructProcess();
    ConstructMuon();
    ConstructEM_Low_Energy();
    plistFound=true;
  } else if(physicsListName.contains("livermore")){
    G4EmLivermorePhysics* physList = new G4EmLivermorePhysics;
    physList->ConstructProcess();
    plistFound=true;
  }else if(physicsListName.contains("penelope")){
    G4EmPenelopePhysics* physList = new G4EmPenelopePhysics;
    physList->ConstructProcess();
    plistFound=true;
  }else if(physicsListName.contains("G4EmStandard")){
    G4EmStandardPhysics* physList = new G4EmStandardPhysics;
    physList->ConstructProcess();
    plistFound=true;
  }
  
  //===========================================
  //Some options
  //-------------------------------------------
  //Build planck scattering if option is set
  if(BDSGlobalConstants::Instance()->DoPlanckScattering())
    {
      BDSPlanckScatterBuilder* psbuild = new BDSPlanckScatterBuilder();
      psbuild->Build();
      // psbuild is leaked. This can't be deleted as its BDSPlanckScatterProcess is registered
    }
  
  //A flag to switch on hadronic lead particle biasing
  if (BDSGlobalConstants::Instance()->UseHadLPB())
    {setenv("SwitchLeadBiasOn","1",1);}
  
  //Synchrotron radiation
  if(physicsListName.contains("synchrad"))
    {ConstructSR();}
  
  //Particle decay
  if(physicsListName.contains("synchrad"))
    {ConstructDecay();}
  
  //Optical processes
  ConstructOptical();
  //============================================
  //Need to add transportation and step limiter in all cases.
  AddTransportation();
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager *pmanager = particle->GetProcessManager();
    if((particle->GetParticleName()=="gamma")||
       (particle->GetParticleName()=="e-")||
       (particle->GetParticleName()=="e+")||
       (particle->GetParticleName()=="proton")){
      particle->SetApplyCutsFlag(true);
    }
    pmanager->AddProcess(new G4StepLimiter,-1,-1,1);
#ifndef NOUSERSPECIALCUTS
    pmanager->AddDiscreteProcess(new G4UserSpecialCuts);
#endif
  }
  //Always add parameterisation
  AddParameterisation();
  
  if(plistFound) return;
  //Search BDSIM physics lists
  if (physicsListName.contains("standard")) return;
  // register physics processes here
  // standard e+/e-/gamma electromagnetic interactions
  if(physicsListName.contains("em_standard") )
    {
      ConstructEM();
    }
  else if(physicsListName.contains("em_single_scatter") )
    {
      ConstructEMSingleScatter();
    }
  else if(physicsListName.contains("merlin") )
    {
      ConstructMerlin();
    }
  
  // low energy em processes
  else if(physicsListName.contains("em_low") )
    {
      ConstructEM_Low_Energy();
    }
      
  // standard electromagnetic + muon
  else if(physicsListName.contains("em_muon") )
    {
      ConstructEM();
      ConstructMuon();
    }
  // standard hadronic - photo-nuclear etc.
  else if(physicsListName.contains("hadronic_standard") )
    {
      ConstructEM();
      ConstructHadronic();
    }
      
  // standard electromagnetic + muon + hadronic
  else if(physicsListName.contains("hadronic_muon") )
    {
      ConstructEM();
      ConstructMuon();
      ConstructHadronic();
    }
  
  else if(physicsListName.contains("hadronic_QGSP_BERT") )
    {
      ConstructEM();
      theBDSIMPhysList = new G4HadronPhysicsQGSP_BERT("hadron");
      theBDSIMPhysList->ConstructProcess();
    }
  
  else if(physicsListName.contains("hadronic_QGSP_BERT_muon") )
    {
      ConstructEM();
      ConstructMuon();
      theBDSIMPhysList = new G4HadronPhysicsQGSP_BERT("hadron");
      theBDSIMPhysList->ConstructProcess();
    }
  
  else if(physicsListName.contains("hadronic_FTFP_BERT")){
    ConstructEM();
    theBDSIMPhysList = new G4HadronPhysicsFTFP_BERT;
    theBDSIMPhysList->ConstructProcess();
  }
  
  else if(physicsListName.contains("hadronic_QGSP_BERT_HP_muon")){
    ConstructEM();
    ConstructMuon();
    ConstructHadronic();
    theBDSIMPhysList = new G4HadronPhysicsQGSP_BERT_HP;
    theBDSIMPhysList->ConstructProcess();
  }
      
  else if(physicsListName.contains("hadronic_FTFP_BERT_muon")){
    G4cout << __METHOD_NAME__ << "Using hadronic_FTFP_BERT_muon" << G4endl;
    ConstructEM();
    ConstructMuon();
    theBDSIMPhysList = new G4HadronPhysicsFTFP_BERT;
    theBDSIMPhysList->ConstructProcess();
  }
  // physics list for laser wire - standard em stuff +
  // weighted compton scattering from laser wire
  else if(physicsListName.contains("lw")) {
    ConstructEM();
    ConstructLaserWire();
  }
  else {
    G4cerr<<"WARNING : Unknown physics list "<<physicsListName<<G4endl;
    exit(1);
  }
}

void BDSPhysicsList::ConstructParticle()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  //standard physics lists
  if (theReferenceHadronicPhysList || theReferenceEmPhysList) {
    if (theReferenceHadronicPhysList) {
      theReferenceHadronicPhysList->ConstructParticle();
    }
    if (theReferenceEmPhysList) {
      theReferenceEmPhysList->ConstructParticle();
    }
  } else {
    // pseudo-particles
    G4Geantino::GeantinoDefinition();
    G4ChargedGeantino::ChargedGeantinoDefinition();
    
    // gamma
    G4Gamma::GammaDefinition();
    
    // optical photon
    G4OpticalPhoton::OpticalPhotonDefinition();
    
    // leptons
    G4Electron::ElectronDefinition();
    G4Positron::PositronDefinition();
    G4MuonPlus::MuonPlusDefinition();
    G4MuonMinus::MuonMinusDefinition();
    G4TauPlus::TauPlusDefinition();
    G4TauMinus::TauMinusDefinition();

    G4NeutrinoE::NeutrinoEDefinition();
    G4AntiNeutrinoE::AntiNeutrinoEDefinition();
    G4NeutrinoMu::NeutrinoMuDefinition();
    G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
    G4NeutrinoTau::NeutrinoTauDefinition();
    G4AntiNeutrinoTau::AntiNeutrinoTauDefinition();

    // mesons
    G4MesonConstructor mConstructor;
    mConstructor.ConstructParticle();
    
    // baryons
    G4BaryonConstructor bConstructor;
    bConstructor.ConstructParticle();
    
    // ions
    G4IonConstructor iConstructor;
    iConstructor.ConstructParticle();
    
    //  Construct resonances and quarks
    G4ShortLivedConstructor pShortLivedConstructor;
    pShortLivedConstructor.ConstructParticle();
  }
  
  // set primary particle definition and kinetic beam parameters other than total energy
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  BDSGlobalConstants::Instance()->SetParticleDefinition(particleTable->
                                    FindParticle(BDSGlobalConstants::Instance()->ParticleName()) );
  
  if(!BDSGlobalConstants::Instance()->GetParticleDefinition()) 
    {
      G4Exception("Particle not found, quitting!", "-1", FatalException, "");
      exit(1);
    }
  
  // set kinetic beam parameters other than total energy
  BDSGlobalConstants::Instance()->SetBeamMomentum( sqrt(pow(BDSGlobalConstants::Instance()->BeamTotalEnergy(),2)-
                                    pow(BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass(),2)) );
  
  BDSGlobalConstants::Instance()->SetBeamKineticEnergy(BDSGlobalConstants::Instance()->BeamTotalEnergy() -
                                   BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass() );


  BDSGlobalConstants::Instance()->SetParticleMomentum( sqrt(pow(BDSGlobalConstants::Instance()->ParticleTotalEnergy(),2)-
                                    pow(BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass(),2)) );
  
  BDSGlobalConstants::Instance()->SetParticleKineticEnergy(BDSGlobalConstants::Instance()->ParticleTotalEnergy() -
                                   BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass() );
  
  G4cout << __METHOD_NAME__ << "Beam properties:"<<G4endl;
  G4cout << __METHOD_NAME__ << "Particle : " 
	 << BDSGlobalConstants::Instance()->GetParticleDefinition()->GetParticleName()<<G4endl;
  G4cout << __METHOD_NAME__ << "Mass : " 
	 << BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass()/CLHEP::GeV<< " GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Charge : " 
	 << BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGCharge()<< " e"<<G4endl;
  G4cout << __METHOD_NAME__ << "Total Energy : "
	 << BDSGlobalConstants::Instance()->BeamTotalEnergy()/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Kinetic Energy : "
	 << BDSGlobalConstants::Instance()->BeamKineticEnergy()/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Momentum : "
	 << BDSGlobalConstants::Instance()->BeamMomentum()/CLHEP::GeV<<" GeV"<<G4endl;
}

void BDSPhysicsList::SetCuts()
{
  SetCutsWithDefault();   

  G4double prodCutPhotons   = BDSGlobalConstants::Instance()->ProdCutPhotons();
  G4double prodCutElectrons = BDSGlobalConstants::Instance()->ProdCutElectrons();
  G4double prodCutPositrons = BDSGlobalConstants::Instance()->ProdCutPositrons();
  G4double prodCutProtons   = BDSGlobalConstants::Instance()->ProdCutProtons();
  
  G4cout << __METHOD_NAME__ << "Photon   production range cut (mm) " << prodCutPhotons   << G4endl;
  G4cout << __METHOD_NAME__ << "Electron production range cut (mm) " << prodCutElectrons << G4endl;
  G4cout << __METHOD_NAME__ << "Positron production range cut (mm) " << prodCutPositrons << G4endl;
  G4cout << __METHOD_NAME__ << "Proton   production range cut (mm) " << prodCutProtons   << G4endl;

  // always non-zero, so always set!
  SetCutValue(prodCutPhotons,  "gamma");
  SetCutValue(prodCutElectrons,"e-");
  SetCutValue(prodCutPositrons,"e+");
  SetCutValue(prodCutProtons,  "proton");
  
  DumpCutValuesTable();
}

// particular physics process constructors

void BDSPhysicsList::ConstructEM()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  ConstructEMMisc();
  ConstructMultipleScattering();
}

void BDSPhysicsList::ConstructEMSingleScatter(){
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  ConstructEMMisc();
  ConstructCoulombScattering();
}

void BDSPhysicsList::ConstructEMMisc()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if (particleName == "gamma") {
      // gamma         
      pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
      pmanager->AddDiscreteProcess(new G4ComptonScattering);
      if (BDSGlobalConstants::Instance()->UseEMLPB()){ //added by M.D. Salt, R.B. Appleby, 15/10/09
	  G4GammaConversion* gammaconversion = new G4GammaConversion();
	  GammaConversion_LPB* gammaconversion_lpb = new GammaConversion_LPB();
	  gammaconversion_lpb->RegisterProcess(gammaconversion);
	  pmanager->AddDiscreteProcess(gammaconversion_lpb);
      } else {
	pmanager->AddDiscreteProcess(new G4GammaConversion);
      }
    
      
    } else if (particleName == "e-") {
      pmanager->AddProcess(new G4eIonisation,       -1, 2,2);
      if(BDSGlobalConstants::Instance()->UseEMLPB()){ //added by M.D. Salt, R.B. Appleby, 15/10/09
	  
        G4eBremsstrahlung* ebremsstrahlung = new G4eBremsstrahlung();
        eBremsstrahlung_LPB* ebremsstrahlung_lpb = new eBremsstrahlung_LPB();
        ebremsstrahlung_lpb->RegisterProcess(ebremsstrahlung);
        pmanager->AddProcess(ebremsstrahlung_lpb,     -1,-1,3);
      } else {
	G4eBremsstrahlung* ebremsstrahlung = new G4eBremsstrahlung();
        pmanager->AddProcess(ebremsstrahlung,   -1, 3,3);     
      }
            
      if(BDSGlobalConstants::Instance()->TurnOnCerenkov()){
        G4Cerenkov* theCerenkovProcess = new G4Cerenkov;
        pmanager->AddProcess(theCerenkovProcess);
        pmanager->SetProcessOrdering(theCerenkovProcess,idxPostStep);
      }
      
    } else if (particleName == "e+") {
      //positron
      pmanager->AddProcess(new G4eIonisation,       -1, 2,2);
      if (BDSGlobalConstants::Instance()->UseEMLPB()){
	G4eBremsstrahlung* ebremsstrahlung = new G4eBremsstrahlung();
	eBremsstrahlung_LPB* ebremsstrahlung_lpb = new eBremsstrahlung_LPB();
        ebremsstrahlung_lpb->RegisterProcess(ebremsstrahlung);
        pmanager->AddProcess(ebremsstrahlung_lpb,     -1,-1,3);
      } else {
        pmanager->AddProcess(new G4eBremsstrahlung,   -1, 3,3);
      }
      pmanager->AddProcess(new G4eplusAnnihilation,  0,-1,4);
      if(BDSGlobalConstants::Instance()->TurnOnCerenkov()){
        G4Cerenkov* theCerenkovProcess = new G4Cerenkov;
        pmanager->AddProcess(theCerenkovProcess);
        pmanager->SetProcessOrdering(theCerenkovProcess,idxPostStep);
      }
    } else if ((!particle->IsShortLived()) &&
	       (particle->GetPDGCharge() != 0.0) && 
	       (particle->GetParticleName() != "chargedgeantino")) {
      //all others charged particles except geantino
      pmanager->AddProcess(new G4hIonisation,       -1, 2,2);
           if(BDSGlobalConstants::Instance()->TurnOnCerenkov()){
        G4Cerenkov* theCerenkovProcess = new G4Cerenkov;
        pmanager->AddProcess(theCerenkovProcess);
        pmanager->SetProcessOrdering(theCerenkovProcess,idxPostStep);
      }
    }
  }
}

void BDSPhysicsList::ConstructMultipleScattering(){
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle     = theParticleIterator->value();
    G4ProcessManager*     pmanager     = particle->GetProcessManager();
    G4String              particleName = particle->GetParticleName();
    if (particleName == "e-") {
      //electron
      pmanager->AddProcess(new G4eMultipleScattering,-1, 1,1);

      
    } else if (particleName == "e+") {
      
      pmanager->AddProcess(new G4eMultipleScattering,-1, 1,1);
    } else if ((!particle->IsShortLived()) &&
	       (particle->GetPDGCharge() != 0.0) && 
	       (particle->GetParticleName() != "chargedgeantino")) {
      
      //all others charged particles except geantino
      pmanager->AddProcess(new G4hMultipleScattering,-1, 1,1);
    }
  }
}

void BDSPhysicsList::ConstructCoulombScattering(){
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName(); 
    if (particleName == "e-") {
      pmanager->AddDiscreteProcess(new G4CoulombScattering);
    } else if (particleName == "e+") {
      pmanager->AddDiscreteProcess(new G4CoulombScattering);
    }else if ((!particle->IsShortLived()) &&
	      (particle->GetPDGCharge() != 0.0) && 
	      (particle->GetParticleName() != "chargedgeantino")) {
      //all others charged particles except geantino
      pmanager->AddDiscreteProcess(new G4CoulombScattering);
    } 
  }
}

// particular physics process constructors
void BDSPhysicsList::ConstructMuon()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();

    if (particleName == "gamma") {
      // gamma         
      G4GammaConversionToMuons* gammaconversiontomuons = new G4GammaConversionToMuons();
      pmanager->AddDiscreteProcess(gammaconversiontomuons);
    } else if (particleName == "e+") {
      //positron
      //===========ee to hadrons in development================
      G4eeToHadrons* eetohadrons = new G4eeToHadrons();
      pmanager->AddDiscreteProcess(eetohadrons);
      //-------------------------------------------------------
      G4AnnihiToMuPair* annihitomupair = new G4AnnihiToMuPair();
      pmanager->AddDiscreteProcess(annihitomupair);  
    } else if( particleName == "mu+" || 
               particleName == "mu-"    ) {
      //muon  
      pmanager->AddProcess(new G4MuMultipleScattering,-1, 1,1);
      pmanager->AddProcess(new G4MuIonisation,      -1, 2,2);
      pmanager->AddProcess(new G4MuBremsstrahlung,  -1, 3,3);
      pmanager->AddProcess(new G4MuPairProduction,  -1, 4,4);
      if(BDSGlobalConstants::Instance()->TurnOnCerenkov()){
        G4Cerenkov* theCerenkovProcess = new G4Cerenkov;
        pmanager->AddProcess(theCerenkovProcess);
        pmanager->SetProcessOrdering(theCerenkovProcess,idxPostStep);
	/*	pmanager->AddDiscreteProcess(new G4MuonVDNuclearModel); */
      }
    }
  }
}
   

void BDSPhysicsList::ConstructDecay()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  theParticleIterator->reset();
  G4Decay* theDecayProcess = new G4Decay();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    
    if (theDecayProcess->IsApplicable(*particle)) { 
      pmanager -> AddProcess(theDecayProcess);
      pmanager -> SetProcessOrdering(theDecayProcess, idxPostStep);
      pmanager -> SetProcessOrdering(theDecayProcess, idxAtRest);
    }
  }
}


void BDSPhysicsList::ConstructOptical()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  bool bCerOn= BDSGlobalConstants::Instance()->TurnOnCerenkov();
  bool bBirksOn= BDSGlobalConstants::Instance()->TurnOnBirksSaturation();

//  theCerenkovProcess->DumpPhysicsTable();
//  theScintillationProcess->DumpPhysicsTable();
//  theRayleighScatteringProcess->DumpPhysicsTable();

  if(bCerOn){
    if(!theCerenkovProcess){
      theCerenkovProcess = new G4Cerenkov("Cerenkov");
    }
  }
  
  theScintillationProcess        = new G4Scintillation("Scintillation");
  if(BDSGlobalConstants::Instance()->TurnOnOpticalAbsorption()){
    theAbsorptionProcess         = new G4OpAbsorption();
  }
  if(BDSGlobalConstants::Instance()->TurnOnRayleighScattering()){
    theRayleighScatteringProcess = new G4OpRayleigh();
  }
  if(BDSGlobalConstants::Instance()->TurnOnMieScattering()){
    theMieHGScatteringProcess    = new G4OpMieHG();
  }
  if(BDSGlobalConstants::Instance()->TurnOnOpticalSurface()){
    theBoundaryProcess           = new G4OpBoundaryProcess();
  }

  SetVerboseLevel(1);
  theScintillationProcess->SetScintillationYieldFactor(BDSGlobalConstants::Instance()->ScintYieldFactor());
  theScintillationProcess->SetTrackSecondariesFirst(true);

  // Use Birks Correction in the Scintillation process

  if(bBirksOn){
    G4EmSaturation* emSaturation = G4LossTableManager::Instance()->EmSaturation();
    theScintillationProcess->AddSaturation(emSaturation);
  }

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if(bCerOn){
      if (theCerenkovProcess->IsApplicable(*particle)) {
	pmanager->AddProcess(theCerenkovProcess);
	pmanager->SetProcessOrdering(theCerenkovProcess,idxPostStep);
	theCerenkovProcess->SetMaxNumPhotonsPerStep(20);
	theCerenkovProcess->SetMaxBetaChangePerStep(10.0);
	theCerenkovProcess->SetTrackSecondariesFirst(true);
      }
    }
    if (theScintillationProcess->IsApplicable(*particle)) {
      pmanager->AddProcess(theScintillationProcess);
      pmanager->SetProcessOrderingToLast(theScintillationProcess, idxAtRest);
      pmanager->SetProcessOrderingToLast(theScintillationProcess, idxPostStep);
    }
    if (particleName == "opticalphoton") {
#ifdef BDSDEBUG
      G4cout << "AddDiscreteProcess to OpticalPhoton " << G4endl;
#endif
      if(BDSGlobalConstants::Instance()->TurnOnOpticalAbsorption()){
	pmanager->AddDiscreteProcess(theAbsorptionProcess);
      }
      if(BDSGlobalConstants::Instance()->TurnOnRayleighScattering()){
	pmanager->AddDiscreteProcess(theRayleighScatteringProcess);
      }
      if(BDSGlobalConstants::Instance()->TurnOnMieScattering()){
	pmanager->AddDiscreteProcess(theMieHGScatteringProcess);
      }
      if(BDSGlobalConstants::Instance()->TurnOnOpticalSurface()){
	pmanager->AddDiscreteProcess(theBoundaryProcess);
      }
    }
  }
}


void BDSPhysicsList::ConstructMerlin()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    
    if (particleName == "e-") {
      //electron
      pmanager->AddProcess(new G4eMultipleScattering,-1, 1,1);
      pmanager->AddProcess(new G4eIonisation,       -1, 2,2);
      pmanager->AddProcess(new G4eBremsstrahlung,   -1, 3,3);      
    } 
  }
}

void BDSPhysicsList::ConstructEM_Low_Energy()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  //Applicability range for Livermore models                                                                                                                                
  //for higher energies, the Standard models are used                                                                                                                       
  G4double highEnergyLimit = 1*CLHEP::GeV;

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
     
    if (particleName == "gamma") {
      G4RayleighScattering* rayl = new G4RayleighScattering();
      G4LivermoreRayleighModel*
	raylModel = new G4LivermoreRayleighModel();
      raylModel->SetHighEnergyLimit(highEnergyLimit);
      rayl->AddEmModel(0, raylModel);
      pmanager->AddDiscreteProcess(rayl);

      G4PhotoElectricEffect* phot = new G4PhotoElectricEffect();
      G4LivermorePhotoElectricModel*
	photModel = new G4LivermorePhotoElectricModel();
      photModel->SetHighEnergyLimit(highEnergyLimit);
      phot->AddEmModel(0, photModel);
      pmanager->AddDiscreteProcess(phot);

      G4ComptonScattering* compt = new G4ComptonScattering();
      G4LivermoreComptonModel*
	comptModel = new G4LivermoreComptonModel();
      comptModel->SetHighEnergyLimit(highEnergyLimit);
      compt->AddEmModel(0, comptModel);
      pmanager->AddDiscreteProcess(compt);
      
      G4GammaConversion* conv = new G4GammaConversion();
      G4LivermoreGammaConversionModel*
	convModel = new G4LivermoreGammaConversionModel();
      convModel->SetHighEnergyLimit(highEnergyLimit);
      conv->AddEmModel(0, convModel);
      pmanager->AddDiscreteProcess(conv);
      
    } else if (particleName == "e-") {
        pmanager->AddProcess(new G4eMultipleScattering,-1, 1,1);
	G4eIonisation* eIoni = new G4eIonisation();
	G4LivermoreIonisationModel*
	  eIoniModel = new G4LivermoreIonisationModel();
	eIoniModel->SetHighEnergyLimit(highEnergyLimit);
	eIoni->AddEmModel(0, eIoniModel, new G4UniversalFluctuation() );
	pmanager->AddProcess(eIoni,                   -1,-1, 1);
	
	G4eBremsstrahlung* eBrem = new G4eBremsstrahlung();
	G4LivermoreBremsstrahlungModel*
	  eBremModel = new G4LivermoreBremsstrahlungModel();
	eBremModel->SetHighEnergyLimit(highEnergyLimit);
	eBrem->AddEmModel(0, eBremModel);
	pmanager->AddProcess(eBrem,                   -1,-1, 2);
	    
    } else if (particleName == "e+") {
        pmanager->AddProcess(new G4eMultipleScattering,-1, 1,1);
	pmanager->AddProcess(new G4eIonisation,        -1, 2,2);
	pmanager->AddProcess(new G4eBremsstrahlung,    -1, 3,3);
	pmanager->AddProcess(new G4eplusAnnihilation,   0,-1,4);
    } else if( particleName == "mu+" || 
               particleName == "mu-"    ) {
        pmanager->AddProcess(new G4MuMultipleScattering,-1, 1,1);
        pmanager->AddProcess(new G4MuIonisation,      -1, 2,2);
        pmanager->AddProcess(new G4MuBremsstrahlung,  -1, 3,3);
        pmanager->AddProcess(new G4MuPairProduction,  -1, 4,4);       

    } else if (particleName == "GenericIon") {
        pmanager->AddProcess(new G4hMultipleScattering,-1, 1,1);
	pmanager->AddProcess(new G4ionIonisation,     -1,-1, 1);

    } else if ((!particle->IsShortLived()) &&
	       (particle->GetPDGCharge() != 0.0) && 
	       (particle->GetParticleName() != "chargedgeantino")) {

      pmanager->AddProcess(new G4hMultipleScattering,-1, 1,1);
      pmanager->AddProcess(new G4hIonisation,       -1,-1, 1);
    }
  }
}

void BDSPhysicsList::ConstructLaserWire()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4cout << "Constructing laser-wire" << G4endl;

  theParticleIterator->reset();

  BDSLaserCompton* lwProcess = new BDSLaserCompton;

  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    
    if (particleName == "e-") {
      pmanager->AddProcess(lwProcess);
      pmanager->SetProcessOrderingToLast(lwProcess,idxPostStep);
    }

    if (particleName == "e+") {
      pmanager->AddProcess(lwProcess);
      pmanager->SetProcessOrderingToLast(lwProcess,idxPostStep);
    }

  }

}


// Hadron Processes

#include "G4HadronElasticProcess.hh"
#include "G4HadronFissionProcess.hh"
#include "G4HadronCaptureProcess.hh"

#include "G4PionPlusInelasticProcess.hh"
#include "G4PionMinusInelasticProcess.hh"
#include "G4KaonPlusInelasticProcess.hh"
#include "G4KaonZeroSInelasticProcess.hh"
#include "G4KaonZeroLInelasticProcess.hh"
#include "G4KaonMinusInelasticProcess.hh"
#include "G4ProtonInelasticProcess.hh"
#include "G4AntiProtonInelasticProcess.hh"
#include "G4NeutronInelasticProcess.hh"
#include "G4AntiNeutronInelasticProcess.hh"
#include "G4LambdaInelasticProcess.hh"
#include "G4AntiLambdaInelasticProcess.hh"
#include "G4SigmaPlusInelasticProcess.hh"
#include "G4SigmaMinusInelasticProcess.hh"
#include "G4AntiSigmaPlusInelasticProcess.hh"
#include "G4AntiSigmaMinusInelasticProcess.hh"
#include "G4XiZeroInelasticProcess.hh"
#include "G4XiMinusInelasticProcess.hh"
#include "G4AntiXiZeroInelasticProcess.hh"
#include "G4AntiXiMinusInelasticProcess.hh"
#include "G4OmegaMinusInelasticProcess.hh"
#include "G4AntiOmegaMinusInelasticProcess.hh"

// Low-energy Models
#include "G4HadronElastic.hh"

#include "G4CascadeInterface.hh"

// -- generator models
#include "G4TheoFSGenerator.hh"
#include "G4ExcitationHandler.hh"
#include "G4GeneratorPrecompoundInterface.hh"
#include "G4StringModel.hh"
#include "G4PreCompoundModel.hh"
#include "G4QGSMFragmentation.hh"
#include "G4ExcitedStringDecay.hh"

void BDSPhysicsList::ConstructHadronic()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  
    /*
    From (14-7-14)
    http://geant4.cern.ch/support/proc_mod_catalog/physics_lists/useCases.shtml  

    LHC neutron fluxes

    The Simulation of neutron fluxes needs, in addition to the simulation of hadronic showers, a transport for low energy neutron down to thermal energies. Recommended: QGSP_BERT_HP, QGSP_BIC_HP
    
    Linear collider neutron fluxes

    Recommended: QGSP_BERT_HP, QGSP_BIC_HP

    Shielding applications (all energies)

    Recommended: QGSP_BERT_HP, QGSP_BIC_HP, QGSP_INCLXX, Shielding
  */

  /* based on these recommendations QGSP_BERT_HP, QGSP_BIC_HP are chosen 
     with QGSP_INCLXX and Shielding perhaps optional for certain studies
   */

  theHadPhysList1 = new G4HadronPhysicsQGSP_BERT_HP();
  theHadPhysList1->ConstructProcess();

  theHadPhysList2 = new G4HadronPhysicsQGSP_BIC_HP();
  theHadPhysList2->ConstructProcess();
}

void BDSPhysicsList::ConstructSR()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "synch. rad. is turned on" << G4endl;
#endif
  G4SynchrotronRadiation* synchrotron = new G4SynchrotronRadiation();

  theParticleIterator->reset();

  while( (*theParticleIterator)() )
    {
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();

      // add to charged particles
      if (particle->GetPDGCharge() != 0)
	{
	  pmanager->AddProcess(synchrotron);
	  pmanager->SetProcessOrderingToLast(synchrotron,idxPostStep);
	}
    }
  return; 
}

void BDSPhysicsList::AddParameterisation()
{
  auto theFastSimulationManagerProcess = new G4FastSimulationManagerProcess();
  G4cout << "FastSimulationManagerProcess" <<G4endl;
  theParticleIterator->reset();

  while( (*theParticleIterator)() )
    {
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      pmanager->AddDiscreteProcess(theFastSimulationManagerProcess);
    }
}
