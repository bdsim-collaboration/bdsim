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
#include "BDSGlobalConstants.hh"
#include "BDSParticleMilli.hh"
#include "BDSPhysicsMilli.hh"

#include "G4SystemOfUnits.hh"
#include "G4ParticleDefinition.hh"
#include "G4LossTableManager.hh"
#include "G4EmParameters.hh"
#include "G4PhysicsListHelper.hh"
#include "G4AutoDelete.hh"
#include "G4Version.hh"
#include "G4BuilderType.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eMultipleScattering.hh"
#include "G4CoulombScattering.hh"

BDSPhysicsMilli::BDSPhysicsMilli(const G4String&, G4int ver): G4VPhysicsConstructor("G4millicharged"), verbose(ver)
{
    G4EmParameters* param = G4EmParameters::Instance();
    param->SetDefaults();
    param->SetVerbose(verbose);
    param->SetMinEnergy(100*eV);
    param->SetMaxEnergy(10*TeV);
    param->SetLowestElectronEnergy(10*eV);
    param->SetNumberOfBinsPerDecade(20);
    param->ActivateAngularGeneratorForIonisation(true);
    param->SetMscThetaLimit(0.0);
    param->SetFluo(true);
    param->SetAuger(true);
    param->SetPixe(true);
    SetPhysicsType(bElectromagnetic);
}

BDSPhysicsMilli::~BDSPhysicsMilli()
{;}

void BDSPhysicsMilli::ConstructParticle()
{
    // millicharged
    ParticleMilli::MillichargeDefinition();
}

void BDSPhysicsMilli::ConstructProcess()
{
    G4eIonisation* eIoni = nullptr;
    eIoni = new G4eIonisation();
    G4AutoDelete::Register(eIoni);

    G4eBremsstrahlung* eBrem = nullptr;
    eBrem = new G4eBremsstrahlung();
    G4AutoDelete::Register(eBrem);

    G4eMultipleScattering* eMult = nullptr;
    eMult = new G4eMultipleScattering();
    G4AutoDelete::Register(eMult);

    G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

#if G4VERSION_NUMBER > 1029
    auto aParticleIterator = GetParticleIterator();
#endif
    aParticleIterator->reset();

    while( (*aParticleIterator)() )
    {
        G4ParticleDefinition* particle = aParticleIterator->value();
        G4String particleName = particle->GetParticleName();

        if(particleName == BDSGlobalConstants::Instance()->millichargeName())
        {
            ph->RegisterProcess(eIoni, particle);
            ph->RegisterProcess(eBrem, particle);
            ph->RegisterProcess(eMult, particle);
            continue;
        }
    }
}
