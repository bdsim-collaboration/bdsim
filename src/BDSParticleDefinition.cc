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
#include "BDSIonDefinition.hh"
#include "BDSParticleDefinition.hh"
#include "BDSPhysicalConstants.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "G4ParticleDefinition.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <iomanip>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>

BDSParticleDefinition::BDSParticleDefinition(G4ParticleDefinition* particleIn,
                                             G4double              totalEnergyIn,
                                             G4double              kineticEnergyIn,
                                             G4double              momentumIn,
                                             G4double              ffactIn,
                                             BDSIonDefinition*     ionDefinitionIn,
                                             G4int                 ionPDGIDIn):
  particle(particleIn),
  ionDefinition(nullptr),
  ionPDGID(ionPDGIDIn),
  name(particleIn->GetParticleName()),
  mass(particleIn->GetPDGMass()),
  charge(0),
  totalEnergy(0),
  kineticEnergy(0),
  momentum(0),
  gamma(1.0),
  beta(1.0),
  brho(std::numeric_limits<double>::max()),// if zero charge infinite magnetic rigidity
  ffact(ffactIn),
  forwards(true)
{
  charge = particle->GetPDGCharge();
  if (ionDefinition) // may be nullptr
    {
      ionDefinition = new BDSIonDefinition(*ionDefinitionIn);
      if (ionDefinition->OverrideCharge()) // if override for ions
        {charge = ionDefinition->Charge();}
    }
  SetEnergies(totalEnergyIn, kineticEnergyIn, momentumIn);
}

BDSParticleDefinition::BDSParticleDefinition(const G4String&   nameIn,
                                             G4double          massIn,
                                             G4double          chargeIn,
                                             G4double          totalEnergyIn,
                                             G4double          kineticEnergyIn,
                                             G4double          momentumIn,
                                             G4double          ffactIn,
                                             BDSIonDefinition* ionDefinitionIn,
                                             G4int             ionPDGIDIn):
  particle(nullptr),
  ionDefinition(nullptr),
  ionPDGID(ionPDGIDIn),
  name(nameIn),
  mass(massIn),
  charge(chargeIn),
  totalEnergy(0),
  kineticEnergy(0),
  momentum(0),
  gamma(1.0),
  beta(1.0),
  brho(std::numeric_limits<double>::max()),// if zero charge infinite magnetic rigidity
  ffact(ffactIn),
  forwards(true)
{
  if (ionDefinitionIn)
    {ionDefinition = new BDSIonDefinition(*ionDefinitionIn);}
  SetEnergies(totalEnergyIn, kineticEnergyIn, momentumIn);
}

void BDSParticleDefinition::SetEnergies(G4double totalEnergyIn,
					G4double kineticEnergyIn,
					G4double momentumIn)
{
  if (BDS::IsFinite(totalEnergyIn))
    {
      if (totalEnergyIn <= mass)
        {
          throw BDSException(__METHOD_NAME__, "total energy (" + std::to_string(totalEnergyIn / CLHEP::GeV)
                                              + " GeV) is less than or equal to the mass (" + std::to_string(mass / CLHEP::GeV)
                                              + " GeV) of the particle \"" + name + "\"");
        }
      totalEnergy   = totalEnergyIn;
      kineticEnergy = totalEnergy - mass;
      CalculateMomentum();
    }
  else if (BDS::IsFinite(kineticEnergyIn))
    {
      if (kineticEnergyIn <= 0)
        {
          throw BDSException(__METHOD_NAME__, "kinetic energy (" + std::to_string(kineticEnergyIn/CLHEP::GeV)
                                              + " GeV) must be > 0");
        }
      kineticEnergy = kineticEnergyIn;
      totalEnergy   = mass + kineticEnergyIn;
      CalculateMomentum();
    }
  else if (BDS::IsFinite(momentumIn))
    {
      if (momentumIn <= 0)
        {
          throw BDSException(__METHOD_NAME__, "momentum (" + std::to_string(momentumIn/CLHEP::GeV)
                                              + " GeV) must be > 0");
        }
      momentum    = momentumIn;
      totalEnergy = std::hypot(momentumIn, mass);
      if (std::isnan(totalEnergy))
        {throw BDSException(__METHOD_NAME__, "sqrt(-ve) encountered in calculating total energy");}
      kineticEnergy = totalEnergy - mass;
    }
  else
    {throw BDSException(__METHOD_NAME__, "total energy, kinetic energy and momentum 0 - one must be non-zero.");}
  CalculateRigidity(ffact);
  CalculateLorentzFactors();
}

BDSParticleDefinition::BDSParticleDefinition(const BDSParticleDefinition& other):
  particle(other.particle),
  ionPDGID(other.ionPDGID),
  name(other.name),
  mass(other.mass),
  charge(other.charge),
  totalEnergy(other.totalEnergy),
  kineticEnergy(other.kineticEnergy),
  momentum(other.momentum),
  gamma(other.gamma),
  beta(other.beta),
  brho(other.brho),
  ffact(other.ffact),
  forwards(other.forwards)
{
  if (other.ionDefinition)
    {ionDefinition = new BDSIonDefinition(*other.ionDefinition);}
  else
    {ionDefinition = nullptr;}
}

BDSParticleDefinition::BDSParticleDefinition(BDSParticleDefinition&& other) noexcept:
  particle(other.particle),
  ionPDGID(other.ionPDGID),
  name(other.name),
  mass(other.mass),
  charge(other.charge),
  totalEnergy(other.totalEnergy),
  kineticEnergy(other.kineticEnergy),
  momentum(other.momentum),
  gamma(other.gamma),
  beta(other.beta),
  brho(other.brho),
  ffact(other.ffact),
  forwards(other.forwards)
{
  ionDefinition = other.ionDefinition;
  other.ionDefinition = nullptr;
}

BDSParticleDefinition& BDSParticleDefinition::operator=(BDSParticleDefinition&& other) noexcept
{
  if (this != &other)
    {
      delete ionDefinition;
      ionDefinition = other.ionDefinition;
      other.ionDefinition = nullptr;

      particle = other.particle;
      ionPDGID = other.ionPDGID;
      name     = other.name;
      mass     = other.mass;
      charge   = other.charge;
      totalEnergy = other.totalEnergy;
      kineticEnergy = other.kineticEnergy;
      momentum = other.momentum;
      gamma    = other.gamma;
      beta     = other.beta;
      brho     = other.brho;
      ffact    = other.ffact;
      forwards = other.forwards;
    }
  return *this;
}

BDSParticleDefinition::~BDSParticleDefinition()
{
  delete ionDefinition;
}

G4int BDSParticleDefinition::PDGID() const
{
  if (particle)
    {return particle->GetPDGEncoding();}
  else if (ionDefinition)
    {return ionPDGID;}
  else
    {return 0;}
}

std::ostream& operator<<(std::ostream& out, const BDSParticleDefinition& def)
{
  G4int pre = 12;
  out << "Particle:       \""<< def.name << "\"" << G4endl;
  out << "Mass:            " << std::setprecision(pre) << def.mass/CLHEP::GeV              << " GeV" << G4endl;
  out << "Charge:          " << def.charge             << " e" << G4endl;
  out << "Total Energy:    " << std::setprecision(pre) << def.totalEnergy/CLHEP::GeV       << " GeV" << G4endl;
  out << "Kinetic Energy:  " << std::setprecision(pre) << def.kineticEnergy/CLHEP::GeV     << " GeV" << G4endl;
  out << "Momentum:        " << std::setprecision(pre) << def.momentum/CLHEP::GeV          << " GeV" << G4endl;
  out << "Gamma:           " << std::setprecision(pre) << def.gamma                                  << G4endl;
  out << "Beta:            " << std::setprecision(pre) << def.beta                                   << G4endl;
  out << "FFact:           " << std::setprecision(pre) << def.ffact                                  << G4endl;
  out << "Rigidity (Brho): " << std::setprecision(pre) << def.brho/(CLHEP::tesla*CLHEP::m) << " T*m" << G4endl;
  return out;
}

void BDSParticleDefinition::CalculateMomentum()
{
  try
    {momentum = std::sqrt(std::pow(totalEnergy,2) - std::pow(mass,2));}
  catch (const std::domain_error&) // sqrt(-ve)
    {throw BDSException(__METHOD_NAME__, "Total energy insufficient to include mass or particle.");}
}

void BDSParticleDefinition::CalculateRigidity(const G4double& ffactIn)
{
  // magnetic rigidity (brho)
  // formula: B(Tesla)*rho(m) = p(GeV)/(0.299792458 * charge(e))
  // charge (in e units); rigidity (in T*m)
  if (BDS::IsFinite(charge))
    {
      brho = ffactIn * momentum / CLHEP::GeV / BDS::cOverGeV / charge;
      brho *= CLHEP::tesla*CLHEP::m; // rigidity (in Geant4 units)
    }
}

void BDSParticleDefinition::CalculateLorentzFactors()
{
  gamma = totalEnergy / mass;
  beta = std::sqrt(1 - (1./std::pow(gamma,2)));
}

void BDSParticleDefinition::ApplyChangeInKineticEnergy(G4double dEk)
{
  if (!BDS::IsFinite(dEk))
    {return;}
  G4double newEk = kineticEnergy + dEk;
  if (newEk < 0)
    {
      forwards = !forwards;
      newEk = std::abs(newEk);
      BDS::Warning(__METHOD_NAME__, "particle change of direction");
    }
  SetEnergies(0,newEk,0);
}
