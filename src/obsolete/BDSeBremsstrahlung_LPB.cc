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
//eBremsstrahlung Leading Particle Biasing Method, M.D. Salt, R.B. Appleby, 15/10/09

#include "BDSeBremsstrahlung_LPB.hh"
#include "CLHEP/Random/Random.h"
#include "BDSGlobalConstants.hh"

G4VParticleChange* eBremsstrahlung_LPB::PostStepDoIt(const G4Track& track, const G4Step& step)
{
  //System to control degree of biasing used
  G4bool fActive = true;
  G4double fBiasFraction = BDSGlobalConstants::Instance()->LPBFraction();
  if(fBiasFraction < CLHEP::RandFlat::shoot()) {
    fActive = false;
  }
  
  //Obtain initial parent weight
  G4double initialWeight = track.GetWeight();
  
  //Store for the secondaries
  std::vector<G4Track*> secondaries;
  
  //Particle change declaration
  G4VParticleChange* particleChange = nullptr;

  //Get secondaries
  particleChange = pRegProcess->PostStepDoIt(track, step);
  assert (0 != particleChange);
  G4int j(0);

  for(j=0; j<particleChange->GetNumberOfSecondaries(); j++)
  {
    secondaries.push_back(new G4Track(*(particleChange->GetSecondary(j))));
  }
  particleChange->Clear();
  particleChange->SetNumberOfSecondaries(secondaries.size());
  
  //Permit weight adjustment for leading particle biasing
  particleChange->SetParentWeightByProcess(true);
  particleChange->SetSecondaryWeightByProcess(true);
  
  //Add secondaries and perform leading particle biasing
  std::vector<G4Track*>::iterator iter = secondaries.begin();
  G4double survivalProbability(0.0);
  while(iter != secondaries.end())
  {
    G4Track* myTrack = *iter;
    if(fActive)
    {
      survivalProbability = myTrack->GetKineticEnergy() / track.GetKineticEnergy();
      if(survivalProbability > CLHEP::RandFlat::shoot())
      {
        //Parent killed
        myTrack->SetWeight(initialWeight / survivalProbability);
        particleChange->ProposeTrackStatus(fStopAndKill);
      }
      else
      {
        //Secondary Killed
        myTrack->SetTrackStatus(fStopAndKill);
        particleChange->ProposeParentWeight(initialWeight / (1.0 - survivalProbability));
      }
    }
    else
    {
      particleChange->ProposeParentWeight(initialWeight);
      myTrack->SetWeight(initialWeight);
    }
    particleChange->AddSecondary(myTrack);
    iter++;
  }
  return particleChange;
}

//Version 2 - no secondaries are killed, bu leading particles are split 

G4VParticleChange* eBremsstrahlung_LPB_v2::PostStepDoIt(const G4Track& track, const G4Step& step)
{
  G4int multiplicity = 2;

  //System to control degree of biasing used
  G4bool fActive = true;
  G4double fBiasFraction = BDSGlobalConstants::Instance()->LPBFraction();
  if(fBiasFraction < CLHEP::RandFlat::shoot()) {
    fActive = false;
  }
  
  //Obtain initial parent weight
  G4double initialWeight = track.GetWeight();
  
  //Store for the secondaries
  std::vector<G4Track*> secondaries;
  
  //Particle change declaration
  G4VParticleChange* particleChange = nullptr;

  //Get secondaries
  particleChange = pRegProcess->PostStepDoIt(track, step);
  assert (0 != particleChange);
  G4int j(0);

  for(j=0; j<particleChange->GetNumberOfSecondaries(); j++)
  {
    secondaries.push_back(new G4Track(*(particleChange->GetSecondary(j))));
  }
  particleChange->Clear();
  particleChange->SetNumberOfSecondaries(secondaries.size());
  
  //Permit weight adjustment for leading particle biasing
  particleChange->SetParentWeightByProcess(true);
  particleChange->SetSecondaryWeightByProcess(true);
  
  //Add secondaries and perform leading particle biasing
  G4double survivalProbability(0.0);

  std::vector<G4Track*>::iterator iter = secondaries.begin();
  G4Track* myTrack = *iter;
  while(iter != secondaries.end()){
    if(fActive)
    {
      survivalProbability = myTrack->GetKineticEnergy() / track.GetKineticEnergy();
      if(survivalProbability > CLHEP::RandFlat::shoot())
	{
	//Secondary is multiplied with reduced weight
	myTrack->SetWeight(initialWeight / (G4double)multiplicity);
	for(int i=0; i<(multiplicity-1); i++){
	  secondaries.push_back(new G4Track(*myTrack));
	}
      }
    }
    else
    {
      particleChange->ProposeParentWeight(initialWeight);
      secondaries[0]->SetWeight(initialWeight);
    }
    iter++;
  }
 
  particleChange->SetNumberOfSecondaries(secondaries.size());

    iter = secondaries.begin();
    while(iter != secondaries.end())
      {
	G4Track* myTrack = *iter;
	particleChange->AddSecondary(myTrack);
	iter++;
      }
  
     return particleChange;
}
