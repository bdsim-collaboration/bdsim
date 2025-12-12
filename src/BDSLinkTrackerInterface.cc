#include <vector>
#include <string>
#include <filesystem>

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4Electron.hh"

#include "BDSLinkTrackerInterface.hh"
#include "BDSLinkBunch.hh"
#include "BDSIMLink.hh"
#include "BDSParticleDefinition.hh"
#include "BDSException.hh"


BDSLinkTrackerInterface* BDSLinkTrackerInterface::singleton = nullptr;

BDSLinkTrackerInterface* BDSLinkTrackerInterface::GetInstance(std::string bdsimConfigFileIn,
                                                              int referenceParticlePDGIn,
                                                              double referenceKineticEnergyIn,
                                                              double relativeEnergyCutIn,
                                                              int seedIn,
                                                              int referenceIonChargeIn,
                                                              bool batchModeIn) {
  // TODO does not work on all platforms
  //if (!std::filesystem::exists(bdsimConfigFileIn)) {
  //  std::cout << "File does not exist.\n";
  //  return nullptr;
  //}


  if(singleton != nullptr) {
    return singleton;
  }
  else {
    singleton = new BDSLinkTrackerInterface(bdsimConfigFileIn,
                                            referenceParticlePDGIn,
                                            referenceKineticEnergyIn,
                                            relativeEnergyCutIn,
                                            seedIn,
                                            referenceIonChargeIn,
                                            batchModeIn);
    return singleton;
  }
}

BDSLinkTrackerInterface* BDSLinkTrackerInterface::GetInstance() {
  if(singleton) {
    return singleton;
  }
  else {
    std::cout << "BDSLinkTrackerInterface not initialised" << std::endl;
    return nullptr;
  }
}

BDSLinkTrackerInterface::BDSLinkTrackerInterface(std::string bdsimConfigFileIn,
                                                 int referenceParticlePDGIn,
                                                 double referenceKineticEnergyIn,
                                                 double relativeEnergyCutIn,
                                                 int seedIn,
                                                 int referenceIonChargeIn,
                                                 bool batchModeIn) :
                                                 bdsimConfigFile(bdsimConfigFileIn),
                                                 referenceParticlePDG(referenceParticlePDGIn),
                                                 referenceKineticEnergy(referenceKineticEnergyIn),
                                                 relativeEnergyCut(relativeEnergyCutIn),
                                                 seed(seedIn),
                                                 referenceIonCharge(referenceIonChargeIn),
                                                 batchMode(batchModeIn)
{
  // keep local tables for ease
  g4particle_table = G4ParticleTable::GetParticleTable();
  g4ion_table = g4particle_table->GetIonTable();

  // create link objects
  linkBunch = new BDSLinkBunch();
  linkBDSIM = new BDSIMLink(linkBunch);

  std::vector<std::string> bdsim_args;
  bdsim_args.push_back("bdsim");
  bdsim_args.push_back("--file="+bdsimConfigFile);
  bdsim_args.push_back("--seed="+std::to_string(seed));
  bdsim_args.push_back("--output=None");

  // append batch configuration
  if(batchMode)
    bdsim_args.push_back(std::string("--batch"));
  else
    bdsim_args.push_back(std::string("--vis_mac=vis.mac"));

  // set minimum kinetic energy
  if(relativeEnergyCut < 1e-6)
    relativeEnergyCut = 1.0;
  minimumKineticEnergy = relativeEnergyCut * referenceKineticEnergy;

  // Create a vector of char* pointing to c_str()
  std::vector<char*> c_args;
  for (auto& arg : bdsim_args) {
    c_args.push_back(const_cast<char*>(arg.c_str()));
  }

  // initialise link object
  linkBDSIM->Initialise(c_args.size(),
                        c_args.data(),
                        true,
                        minimumKineticEnergy/CLHEP::GeV,
                        false);

  referenceParticleDefinition = PrepareBDSParticleDefinition(referenceParticlePDG,
                                                             /* totalEnergy */ 0,
                                                             referenceKineticEnergy,
                                                             /* momentum= */ 0 ,
                                                             static_cast<double>(referenceIonCharge));
}

BDSParticleDefinition* BDSLinkTrackerInterface::PrepareBDSParticleDefinition(int pdg,
                                                                             double totalEnergy,
                                                                             double kineticEnergy,
                                                                             double momentum,
                                                                             int ionCharge) {
  G4ParticleDefinition *particleDefGeant = nullptr;
  BDSParticleDefinition *particleDefinition = nullptr;
  BDSIonDefinition* ionDef = nullptr;

  if (pdg < 1000000000) {  // Not an ion
    particleDefGeant = g4particle_table->FindParticle(pdg);
    particleDefinition = new BDSParticleDefinition(particleDefGeant, 0,
                                                  kineticEnergy, momentum, 1, nullptr);
  }
  else { // Ions
    particleDefGeant = g4ion_table->GetIon(pdg);

    if (ionCharge == 0) {
      ionCharge = particleDefGeant->GetAtomicNumber();
    }

    ionDef = new BDSIonDefinition(particleDefGeant->GetAtomicMass(),
                                  particleDefGeant->GetAtomicNumber(),
                                  ionCharge);

    auto mass = g4ion_table->GetIonMass(ionDef->Z(), ionDef->A());
    auto charge = ionDef->Charge();

    auto bdsimPartName = "ion " + std::to_string(ionDef->A()) +
                         " " + std::to_string(ionDef->Z()) +
                         " " + std::to_string(charge);

    particleDefinition = new BDSParticleDefinition(bdsimPartName, mass, charge, 0,
                                                   kineticEnergy, momentum, 1, ionDef, pdg);
  }
  return particleDefinition;
}

BDSParticleDefinition* BDSLinkTrackerInterface::PrepareBDSParticleDefinition_Bjorn(int pdgIDIn,
                                                                                   double momentumIn,
                                                                                   double kineticEnergyIn,
                                                                                   int ionChargeIn)
{
  G4int pdgID = (G4int) pdgIDIn;

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particleDefGeant;

  // Wrap in our class that calculates momentum and kinetic energy.
  // Requires that one of E, Ek, P be non-zero (only one).
  BDSParticleDefinition* particleDefinition = nullptr;
  BDSIonDefinition* ionDef = nullptr;

  // PDG for ions = 10LZZZAAAI
  if (pdgID > 1000000000) // is an ion
  {

    G4IonTable* ionTable = particleTable->GetIonTable();
    particleDefGeant = ionTable->GetIon(pdgID);
    if (!particleDefGeant)
    {throw BDSException("BDSXtrackInterface> Ion \"" + std::to_string(pdgID) + "\" not found");}

    G4int ionCharge = ionChargeIn==0 ? particleDefGeant->GetAtomicNumber() : (G4int) ionChargeIn;
    ionDef = new BDSIonDefinition(particleDefGeant->GetAtomicMass(),
                                  particleDefGeant->GetAtomicNumber(),
                                  ionCharge);

    // correct the particle mass in the case of partially-stripped ions
    G4double mass   = ionTable->GetIonMass(ionDef->Z(), ionDef->A());
    G4double charge = ionDef->Charge(); // correct even if overridden
    mass += ionDef->NElectrons()*G4Electron::Definition()->GetPDGMass();
    // The constructor with a custom name requires the bdsim name of the ion
    std::string bdsimPartName = "ion " + std::to_string(static_cast<int>(ionDef->A()))
                                + " " + std::to_string(static_cast<int>(ionDef->Z()))
                                + " " + std::to_string(static_cast<int>(charge));

    particleDefinition = new BDSParticleDefinition(bdsimPartName, mass, charge, 0,
                                                   kineticEnergyIn, momentumIn, 1, ionDef, pdgID);
  }
  else
  {
    particleDefGeant = particleTable->FindParticle(pdgID);
    if (!particleDefGeant)
    {throw BDSException("BDSXtrackInterface> Particle \"" + std::to_string(pdgID) + "\" not found");}
    particleDefinition = new BDSParticleDefinition(particleDefGeant, 0, kineticEnergyIn, momentumIn, 1, nullptr);
  }

  return particleDefinition;
}

void BDSLinkTrackerInterface::AddParticle(double x, double y, double px, double py,
                                          double ct, double deltap, double chi,
                                          double chargeRatio, double /*s*/,
                                          int trackid, int pdgid) {

  auto q = chargeRatio * referenceParticleDefinition->Charge();
  auto mass_ratio = chargeRatio / chi;
  auto p = referenceParticleDefinition->Momentum() * (deltap + 1) * mass_ratio;

  auto pdg = 0;
  if(pdgid == 0)
    pdg = referenceParticleDefinition->PDGID();
  else
    pdg = pdgid;

  auto partDef = PrepareBDSParticleDefinition(pdg, p, 0, q);
  auto t = - ct * CLHEP::m / (referenceParticleDefinition->Beta() * CLHEP::c_light);
  auto oneplusdelta = (1 + deltap);
  auto xp = px / oneplusdelta;
  auto yp = py / oneplusdelta;
  auto sqrtarg = 1 - std::pow(xp,2) - std::pow(yp,2);

  auto zp = 0.0;

  if (sqrtarg >=0)
    { zp = std::sqrt(sqrtarg); }

  auto coords = BDSParticleCoordsFull(x * CLHEP::m,
                                      y * CLHEP::m,
                                      0,
                                      xp,
                                      yp,
                                      zp,
                                      t,
                                      0,
                                      partDef->TotalEnergy(),
                                      1);

  linkBunch->AddParticle(partDef, coords, trackid, trackid);
  return;
}

void BDSLinkTrackerInterface::AddParticle(double x, double y,
                                          double px, double py, double pz,
                                          double t, double /*s*/,
                                          int trackid, int pdgid) {
  auto pdg = 0;
  auto q = 0.0;

  if(pdgid == 0) {
    pdg = referenceParticleDefinition->PDGID();
    q = referenceParticleDefinition->Charge();
  }
  else {
    pdg = pdgid;
    q = G4ParticleTable::GetParticleTable()->FindParticle(pdg)->GetPDGCharge();
  }

  auto p = std::sqrt(std::pow(px,2) +
                             std::pow(py,2) +
                             std::pow(pz,2));
  auto partDef = PrepareBDSParticleDefinition(pdg, p, 0, q);
  auto xp = px / pz;
  auto yp = py / pz;
  auto zp = pz / p;

  auto coords = BDSParticleCoordsFull(x,
                                      y,
                                      0,
                                      xp,
                                      yp,
                                      zp,
                                      t,
                                      0,
                                      partDef->TotalEnergy(),
                                      1);

  linkBunch->AddParticle(partDef, coords, trackid, trackid);
  return;
}

void BDSLinkTrackerInterface::AddParticles(std::vector<double> x, std::vector<double> y,
                                           std::vector<double> px, std::vector<double> py,
                                           std::vector<double> ct, std::vector<double> deltap,
                                           std::vector<double> chi, std::vector<double> chargeRatio,
                                           std::vector<double> s, std::vector<int> trackid,
                                           std::vector<int> pdgid) {
  for(size_t i=0; i<x.size(); i++) {
    AddParticle(x[i], y[i], px[i], py[i], ct[i], deltap[i],
                chi[i], chargeRatio[i], s[i], trackid[i], pdgid[i]);
  }
}

void BDSLinkTrackerInterface::ClearData() // TODO separate ClearSamplerHits to another function and add to top of track function
{
  auto* link = GetBDSIMLink();  // Retrieve pointer to BDSIMLink
  link->ClearSamplerHits();
  auto* bunch = GetBunchLink();  // Retrieve pointer to BDSBunch
  bunch->ClearParticles();
  auto& active_state = GetParticleActiveState();  // Retrieve reference
  std::vector<bool>().swap(active_state);  // Efficient clear
}