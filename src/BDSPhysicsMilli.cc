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

#include "G4hIonisation.hh"
#include "G4hMultipleScattering.hh"
#include "G4WentzelVIModel.hh"
#include "G4ProcessManager.hh"
#include "G4StepLimiter.hh"

BDSPhysicsMilli::BDSPhysicsMilli(const G4String&, G4int ver): G4VPhysicsConstructor("G4millicharged"), verbose(ver)
{
    G4EmParameters* param = G4EmParameters::Instance();
    //param->SetDefaults();
    param->SetVerbose(verbose);
    //param->SetMinEnergy(100*eV);
    //param->SetMaxEnergy(10*TeV);
    //param->SetLowestElectronEnergy(10*eV);
    //param->SetNumberOfBinsPerDecade(20);
    //param->ActivateAngularGeneratorForIonisation(true);
    //param->SetMscThetaLimit(0.0);
    //param->SetFluo(true);
    //param->SetAuger(true);
    //param->SetPixe(true);
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
    G4double mass = BDSGlobalConstants::Instance()->millichargeMass();
    G4double emin = mass/20000.;
    if(emin < keV) { emin = keV; }
    G4double emax = std::max(10.*TeV, mass*100);
    G4int nbin = G4lrint(10*std::log10(emax/emin));
    G4double energyLimit = 1.*MeV;

    G4hIonisation* hIoni = nullptr;
    hIoni = new G4hIonisation();
    hIoni->SetDEDXBinning(nbin);
    hIoni->SetMinKinEnergy(emin);
    hIoni->SetMaxKinEnergy(emax);
    G4AutoDelete::Register(hIoni);

    G4hMultipleScattering* hMpl = nullptr;
    hMpl = new G4hMultipleScattering();

    G4WentzelVIModel* modelmpl = nullptr;
    modelmpl = new G4WentzelVIModel();
    modelmpl->SetActivationLowEnergyLimit(energyLimit);
    hMpl->SetEmModel(modelmpl, 1);

    G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

#if G4VERSION_NUMBER > 1029
    auto aParticleIterator = GetParticleIterator();
#endif
    aParticleIterator->reset();

    while( (*aParticleIterator)() )
    {
        G4ParticleDefinition* particle = aParticleIterator->value();
        G4String particleName = particle->GetParticleName();

        if(particleName == "e+")
        //if(particleName == BDSGlobalConstants::Instance()->millichargeName())
        {
            ph->RegisterProcess(hIoni, particle);
            G4ProcessManager* pManager = particle->GetProcessManager();
            pManager->AddProcess(hIoni,-1,2,2);

            ph->RegisterProcess(hMpl, particle);
            ph->RegisterProcess(new G4StepLimiter(), particle);
            continue;
        }
    }
}
