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
#include "BDSPhysicsVectorLinear.hh"
#include "BDSWrapperMuonSplitting.hh"

#include "G4ParticleDefinition.hh"
#include "G4Track.hh"
#include "G4Types.hh"
#include "G4VParticleChange.hh"
#include "G4VProcess.hh"

#include <cmath>
#include <limits>
#include <vector>

G4int BDSWrapperMuonSplitting::nCallsThisEvent = 0;

BDSWrapperMuonSplitting::BDSWrapperMuonSplitting(G4VProcess* originalProcess,
                                                 G4int splittingFactorIn,
                                                 G4double splittingThresholdEKIn,
                                                 G4int splittingFactor2In,
                                                 G4double splittingThresholdEK2In,
                                                 G4bool excludeWeight1ParticlesIn,
                                                 G4double muonSplittingExclusionWeightIn):
  BDSWrapperProcess("MuonSplittingWrapper"),
  splittingFactor(splittingFactorIn),
  splittingThresholdEK(splittingThresholdEKIn),
  splittingFactor2(splittingFactor2In),
  splittingThresholdEK2(splittingThresholdEK2In),
  excludeWeight1Particles(excludeWeight1ParticlesIn),
  muonSplittingExclusionWeight(muonSplittingExclusionWeightIn),
  splitting(nullptr)
{
  RegisterProcess(originalProcess);
  theProcessSubType = originalProcess->GetProcessSubType();
  theProcessName = "MuonSplittingWrapper("+originalProcess->GetProcessName()+")";
  if (splittingFactor2 < splittingFactor)
    {splittingFactor2 = splittingFactor;} // always want more high energy
  if (splittingThresholdEK2 < splittingThresholdEK)
    {splittingThresholdEK2 = splittingThresholdEK;}
  
  std::vector<G4double> eK = {0.8*splittingThresholdEK, splittingThresholdEK};
  std::vector<G4double> va = {2.0, static_cast<G4double>(splittingFactorIn)};
  if (splittingThresholdEK2 > splittingThresholdEK)
    {
      eK.push_back(splittingThresholdEK2);
      va.push_back((G4double)splittingFactor2In);
    }
  splitting = new BDSPhysicsVectorLinear(eK, va);
}

BDSWrapperMuonSplitting::~BDSWrapperMuonSplitting()
{
  delete splitting;
}

G4VParticleChange* BDSWrapperMuonSplitting::PostStepDoIt(const G4Track& track,
							 const G4Step& step)
{
  G4VParticleChange* particleChange = pRegProcess->PostStepDoIt(track, step);
  
  if (splittingFactor == 1)
    {return particleChange;}
  
  G4double parentEk = track.GetKineticEnergy();
  if (parentEk < 0.8*splittingThresholdEK) // smaller of the two thresholds by design
    {return particleChange;}
  
  G4int nSecondaries = particleChange->GetNumberOfSecondaries();
  if (nSecondaries == 0)
    {return particleChange;}
  
  // if weight == 1
  if (excludeWeight1Particles && std::abs(track.GetWeight() - 1.0) > std::numeric_limits<double>::epsilon())
    {return particleChange;}
  
  if (track.GetWeight() > muonSplittingExclusionWeight)
    {return particleChange;}
  
  G4bool muonPresent = false;
  std::vector<G4int> secondaryPDGIDs;
  for (G4int i = 0; i < nSecondaries; i++)
    {
      G4int secondaryPDGID = particleChange->GetSecondary(i)->GetDefinition()->GetPDGEncoding();
      muonPresent = std::abs(secondaryPDGID) == 13 || muonPresent;
    }
  if (!muonPresent)
    {return particleChange;}
  
  // we keep hold of the tracks and manage their memory
  std::vector<G4Track*> originalSecondaries;
  std::vector<G4Track*> originalMuons;
  for (G4int i = 0; i < nSecondaries; i++)
    {
      G4Track* secondary = particleChange->GetSecondary(i);
      if (std::abs(secondary->GetDefinition()->GetPDGEncoding()) != 13)
	{originalSecondaries.push_back(secondary);}
      else
	{originalMuons.push_back(secondary);}
    }
  
  G4int nOriginalSecondaries = nSecondaries;
  
  particleChange->Clear(); // doesn't delete the secondaries
  
  G4double spf2 = splitting->Value(parentEk);
  G4int thisTimeSplittingFactor = static_cast<G4int>(std::round(spf2));

  // Attempt to generate more muons. This might be difficult or rare, so we must
  // tolerate this and go for up to a number.
  G4int maxTrials = 10 * thisTimeSplittingFactor;
  G4int nSuccessfulMuonSplits = 0;
  G4int iTry = 0;
  std::vector<G4Track*> newMuons;
  while (iTry < maxTrials && nSuccessfulMuonSplits < thisTimeSplittingFactor-1)
    {
      iTry++;
      particleChange->Clear(); // wipes the vector of tracks, but doesn't delete them
      particleChange = pRegProcess->PostStepDoIt(track, step);
      G4bool success = false;
      for (G4int i = 0; i < particleChange->GetNumberOfSecondaries(); i++)
        {
          G4Track* sec = particleChange->GetSecondary(i);
          if (std::abs(sec->GetDefinition()->GetPDGEncoding()) == 13)
            {
              newMuons.push_back(sec);
              success = true;
            }
          else
            {delete sec;}
        }
      particleChange->Clear();
      if (success)
        {nSuccessfulMuonSplits++;}
    }
  
  particleChange->Clear();
  particleChange->SetNumberOfSecondaries(nOriginalSecondaries + static_cast<G4int>(newMuons.size()));
  // cache this flag so we can reset it back afterwards
  G4bool originalSetSecondaryWeightByProcess = particleChange->IsSecondaryWeightSetByProcess();
  particleChange->SetSecondaryWeightByProcess(true);
  if (nSuccessfulMuonSplits == 0)
    {// we've cleared the original ones, so we have to put them back
      for (auto secondary : originalSecondaries)
        {particleChange->AddSecondary(secondary);}
      for (auto muon : originalMuons)
        {particleChange->AddSecondary(muon);}
      return particleChange;
    }
    
  // the original muon(s) count as 1 even if there are 2 of them as it's 1x call to the process
  G4double weightFactor = 1.0 / (static_cast<G4double>(nSuccessfulMuonSplits) + 1.0);
  // put in the original secondaries with an unmodified weight
  for (auto aSecondary : originalSecondaries)
    {particleChange->AddSecondary(aSecondary);}
  for (auto originalMuon : originalMuons)
    {
      G4double existingWeight = originalMuon->GetWeight();
      G4double newWeight = existingWeight * weightFactor;
      originalMuon->SetWeight(newWeight);
      particleChange->AddSecondary(originalMuon);
    }
  for (auto newMuon : newMuons)
    {
      G4double existingWeight = newMuon->GetWeight();
      G4double newWeight = existingWeight * weightFactor;
      newMuon->SetWeight(newWeight);
      particleChange->AddSecondary(newMuon);
    }
  // IMPORTANT - we must reset this back to its default value as the process owns
  // (usually) a single particle change object that it reuses, so setting this flag
  // will change that process's behaviour forever more in the run even if this wrapper
  // decides not to act. We must reset it after AddSecondary().
  particleChange->SetSecondaryWeightByProcess(originalSetSecondaryWeightByProcess);

  nCallsThisEvent++;
  return particleChange;
}
