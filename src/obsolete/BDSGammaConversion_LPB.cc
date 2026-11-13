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
//GammaConversion Leading Particle Biasing Method, M.D. Salt, R.B. Appleby, 15/10/09

#include "BDSGammaConversion_LPB.hh"
#include "CLHEP/Random/Random.h"
#include "BDSGlobalConstants.hh"

G4VParticleChange* GammaConversion_LPB::PostStepDoIt(const G4Track& track, const G4Step& step)
{
  //System to control degree of biasing used
  G4bool fActive = true;
  G4double fBiasFraction = BDSGlobalConstants::Instance()->LPBFraction();
  if(fBiasFraction < CLHEP::RandFlat::shoot()){
    fActive = false;
  }

  //Obtain initial parent weight
  G4double initialWeight = track.GetWeight();

  //Secondaries store
  std::vector<G4Track*> secondaries;

  //Declare particle change
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

  //Permit weight setting for leading particle biasing
  particleChange->SetSecondaryWeightByProcess(true);

  //Add the secondaries and perform leading particle biasing
  std::vector<G4Track*>::iterator iter = secondaries.begin();
  G4double survivalProbability(0.0); //relates to the first [0] secondary
  survivalProbability = secondaries[0]->GetKineticEnergy() / track.GetKineticEnergy();
  if(fActive)
    {
    if(survivalProbability > CLHEP::RandFlat::shoot())
    {
      //secondaries[0] survives
      secondaries[0]->SetWeight(initialWeight / survivalProbability);
      secondaries[1]->SetTrackStatus(fStopAndKill);
    }
    else
    {
      //secondaries[1] survives
      secondaries[0]->SetTrackStatus(fStopAndKill);
      secondaries[1]->SetWeight(initialWeight / (1.0 - survivalProbability));
    }
  }
  else
  {
    secondaries[0]->SetWeight(initialWeight);
    secondaries[1]->SetWeight(initialWeight);
  }

  while(iter != secondaries.end())
  {
    G4Track* myTrack = *iter;
    particleChange->AddSecondary(myTrack);
    iter++;
  }
  //Kill the incident photon
  particleChange->ProposeTrackStatus(fStopAndKill);
  return particleChange;
}



//Version 2 - no secondaries are killed, bu leading particles are split with reduced weight

G4VParticleChange* GammaConversion_LPB_v2::PostStepDoIt(const G4Track& track, const G4Step& step)
{
  G4int multiplicity = 2;

  //System to control degree of biasing used
  // G4bool fActive = true;
  // G4double fBiasFraction = BDSGlobalConstants::Instance()->LPBFraction();
  // if(fBiasFraction < CLHEP::RandFlat::shoot()){
  //   fActive = false;
  // }

  //Obtain initial parent weight
  G4double initialWeight = track.GetWeight();

  //Secondaries store
  std::vector<G4Track*> secondaries;

  //Declare particle change
  G4VParticleChange* particleChange = nullptr;

  //Get secondaries
  particleChange = pRegProcess->PostStepDoIt(track, step);
  assert (0 != particleChange);
  G4int j(0);
  for(j=0; j<particleChange->GetNumberOfSecondaries(); j++){
    secondaries.push_back(new G4Track(*(particleChange->GetSecondary(j))));
  }
  
  //Permit weight setting for leading particle biasing
  particleChange->SetSecondaryWeightByProcess(true);

  //Add the secondaries and perform leading particle biasing
  std::vector<G4Track*>::iterator iter = secondaries.begin();
  G4double survivalProbability(0.0); //relates to the first [0] secondary
  survivalProbability = secondaries[0]->GetKineticEnergy() / track.GetKineticEnergy();
  if(0)
    //if(fActive)
  {
    if(survivalProbability > CLHEP::RandFlat::shoot())
    {
      //secondaries[0] is multiplied with reduced weight
      secondaries[0]->SetWeight(initialWeight / (G4double)multiplicity);
      for(int i=0; i<(multiplicity-1); i++){
	secondaries.push_back(new G4Track(*secondaries[0]));
      }
    }
    else
    {
      //secondaries[1] is multiplied with reduced weight
      secondaries[1]->SetWeight(initialWeight / (G4double)multiplicity);
      for(int i=0; i<(multiplicity-1); i++){
	secondaries.push_back(new G4Track(*secondaries[1]));
      }
    }
  }
  else
  {
    secondaries[0]->SetWeight(initialWeight);
    secondaries[1]->SetWeight(initialWeight);
  }

  particleChange->Clear();
  particleChange->SetNumberOfSecondaries(secondaries.size());

  while(iter != secondaries.end())
  {
    G4Track* myTrack = *iter;
    particleChange->AddSecondary(myTrack);
    iter++;
  }
  //Kill the incident photon
  particleChange->ProposeTrackStatus(fStopAndKill);
  return particleChange;
}
