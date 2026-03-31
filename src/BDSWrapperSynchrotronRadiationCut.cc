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
  G4int aboveThreshold = 0; G4int belowThreshold = 0;
  std::vector<G4Track*> secondariesToKeep;

  for (int i = 0; i < numSecondaries; i++)
  {
    G4Track* ithSecondary = particleChange->GetSecondary(i);
    if (ithSecondary->GetParticleDefinition() == G4Gamma::Definition())
    {
      G4double energySRPhoton = ithSecondary->GetKineticEnergy();
      if (energySRPhoton < energyCut)
      {
        ithSecondary->SetTrackStatus(fStopAndKill);
        belowThreshold++;
      }
      else
      {
        secondariesToKeep.push_back(ithSecondary);
        aboveThreshold++;
      }
    }
    else
    {
      secondariesToKeep.push_back(ithSecondary);
    }
  }

  if (aboveThreshold == 0)
  {
    particleChange->Clear(); // doesn't delete the secondaries
    particleChange->SetNumberOfSecondaries(0);
    return particleChange;
  }
  else
  {
    G4double newWeight = (aboveThreshold+belowThreshold)/aboveThreshold;
    particleChange->Clear(); // doesn't delete the secondaries
    particleChange->SetNumberOfSecondaries(secondariesToKeep.size());
    G4bool originalSetSecondaryWeightByProcess = particleChange->IsSecondaryWeightSetByProcess();
    particleChange->SetSecondaryWeightByProcess(true);
    for (auto aSecondary : secondariesToKeep)
    {
      if (aSecondary->GetParticleDefinition() == G4Gamma::Definition())
      {
        G4double weightUpdated = aSecondary->GetWeight()*newWeight;
        aSecondary->SetWeight(weightUpdated);
      }
      particleChange->AddSecondary(aSecondary);
      }
      particleChange->SetSecondaryWeightByProcess(originalSetSecondaryWeightByProcess);
      return particleChange;
    }
  return particleChange;
}

BDSWrapperSynchrotronRadiationCut::~BDSWrapperSynchrotronRadiationCut() {};


