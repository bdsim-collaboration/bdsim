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
#include "BDSPhysicsVectorLinear.hh"
#include "BDSWrapperSynchrotronRadiationCut.hh"

#include "G4ParticleDefinition.hh"
#include "G4Gamma.hh"
#include "G4Track.hh"
#include "G4Types.hh"
#include "G4VParticleChange.hh"
#include "G4FieldManager.hh"
#include "G4VProcess.hh"
#include <cmath>
#include <limits>
#include <vector>


BDSWrapperSynchrotronRadiationCut::BDSWrapperSynchrotronRadiationCut(G4VProcess* originalProcess,
                                                 G4double energyCutIn):
  BDSWrapperProcess("SynchrotronRadiationCut"),
  energyCut(energyCutIn)
{
    RegisterProcess(originalProcess);
    theProcessSubType = originalProcess->GetProcessSubType();
    theProcessName = "SynchrotronRadiationCut("+originalProcess->GetProcessName()+")";

}
G4VParticleChange* BDSWrapperSynchrotronRadiationCut::PostStepDoIt(
    const G4Track& track,
    const G4Step& step)
{


    G4VParticleChange* particleChange = pRegProcess->PostStepDoIt(track, step);

    G4int numSecondaries = particleChange->GetNumberOfSecondaries();

    std::vector<G4Track*> originalSecondaries;
    originalSecondaries.reserve(numSecondaries);

    for (G4int i = 0; i < numSecondaries; i++)
    {
        originalSecondaries.push_back(particleChange->GetSecondary(i));
    }
    auto* processSR =
    static_cast<G4SynchrotronRadiation*>(pRegProcess);

    auto* preStepPoint = step.GetPreStepPoint();
    G4ThreeVector preStepPosition = preStepPoint->GetPosition();

    auto* fieldMgr =
        track.GetVolume()->GetLogicalVolume()->GetFieldManager();

    const G4Field* pField = fieldMgr->GetDetectorField();

    G4int loopCount = 0;

    for (auto* ithSecondary : originalSecondaries)
    {
        if (ithSecondary->GetParticleDefinition() == G4Gamma::Definition())
        {
            G4double energySRPhoton = ithSecondary->GetKineticEnergy();

            if (energySRPhoton < energyCut)
            {

              auto* aDynamicParticle = track.GetDynamicParticle();

              G4double mass = aDynamicParticle->GetDefinition()->GetPDGMass();
              G4double gamma = aDynamicParticle->GetTotalEnergy() / mass;

              G4ThreeVector unitMomentum = aDynamicParticle->GetMomentumDirection();

              G4double newEnergy = RecursiveEnergyCall(
                    loopCount,
                    processSR,
                    pField,
                    preStepPosition,
                    unitMomentum,
                    gamma,
                    mass,
                    step,
                    energySRPhoton);
              G4ParticleDefinition* gammaDef = G4Gamma::GammaDefinition();
              G4ThreeVector dir = ithSecondary->GetMomentumDirection().unit();

              auto* dynParticle = new G4DynamicParticle(gammaDef, dir, newEnergy);

              auto* newPhoton = new G4Track(dynParticle,
                                    track.GetGlobalTime(),
                                    track.GetPosition());

              newPhoton->SetParentID(track.GetTrackID());
              newPhoton->SetCreatorProcess(pRegProcess);

              G4double initialWeight=particleChange->GetParentWeight();
              particleChange->ProposeParentWeight(initialWeight*loopCount);
              particleChange->AddSecondary(newPhoton);
              ithSecondary->SetTrackStatus(fKillTrackAndSecondaries);
              loopCount = 0;
            }
        }
    }



    return particleChange;
}

BDSWrapperSynchrotronRadiationCut::~BDSWrapperSynchrotronRadiationCut() {};

G4double BDSWrapperSynchrotronRadiationCut::RecursiveEnergyCall(
    G4int& count,
    G4SynchrotronRadiation* processSR,
    const G4Field* pField,
    const G4ThreeVector& preStepPosition,
    const G4ThreeVector& unitMomentum,
    G4double gamma,
    G4double mass,
    const G4Step& step,
    G4double energySRPhotonIn)
{
    constexpr G4int maxRecursions = 10000;

    if (count >= maxRecursions)
    {
        return energySRPhotonIn;
    }

    G4double globPosVec[4], FieldValueVec[6];

    globPosVec[0] = preStepPosition.x();
    globPosVec[1] = preStepPosition.y();
    globPosVec[2] = preStepPosition.z();
    globPosVec[3] = step.GetPreStepPoint()->GetGlobalTime();

    pField->GetFieldValue(globPosVec, FieldValueVec);

    G4ThreeVector FieldValue(
        FieldValueVec[0],
        FieldValueVec[1],
        FieldValueVec[2]);

    G4double perpB = (FieldValue.cross(unitMomentum)).mag();

    G4double newEnergy = processSR->GetRandomEnergySR(gamma, perpB, mass);

    ++count;

    return (newEnergy > energyCut)
        ? newEnergy
        : RecursiveEnergyCall(
            count,
            processSR,
            pField,
            preStepPosition,
            unitMomentum,
            gamma,
            mass,
            step,
            energySRPhotonIn);
}