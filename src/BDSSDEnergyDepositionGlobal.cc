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
#include "BDSHitEnergyDepositionGlobal.hh"
#include "BDSSDEnergyDepositionGlobal.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Event.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "Randomize.hh"

BDSSDEnergyDepositionGlobal::BDSSDEnergyDepositionGlobal(const G4String& name,
							 G4bool killedParticleMassAddedToElossIn):
  BDSSensitiveDetector("energy_deposiiton_global/"+name),
  killedParticleMassAddedToEloss(killedParticleMassAddedToElossIn),
  colName(name),
  hits(nullptr),
  HCIDe(-1),
  energy(0),
  preStepKineticEnergy(0),
  postStepKineticEnergy(0),
  stepLength(0),
  weight(0),
  X(0),
  Y(0),
  Z(0),
  globalTime(0),
  pdgID(0),
  trackID(-1),
  parentID(-1),
  turnsTaken(0)
{
  collectionName.insert(colName);
}

BDSSDEnergyDepositionGlobal::~BDSSDEnergyDepositionGlobal()
{;}

void BDSSDEnergyDepositionGlobal::Initialize(G4HCofThisEvent* HCE)
{
  hits = new BDSHitsCollectionEnergyDepositionGlobal(GetName(), colName);
  if (HCIDe < 0)
    {HCIDe = G4SDManager::GetSDMpointer()->GetCollectionID(hits);}
  HCE->AddHitsCollection(HCIDe, hits);
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Hits Collection ID: " << HCIDe << G4endl;
#endif
}

G4bool BDSSDEnergyDepositionGlobal::ProcessHits(G4Step* aStep,
						G4TouchableHistory* /*th*/)
{
  // Get the energy deposited along the step
  energy = aStep->GetTotalEnergyDeposit();

  //if the energy is 0, don't do anything
  if (!BDS::IsFinite(energy))
    {return false;}

  // step points - used many times
  G4StepPoint* preStepPoint  = aStep->GetPreStepPoint();
  G4StepPoint* postStepPoint = aStep->GetPostStepPoint();

  G4Track* track = aStep->GetTrack();
  preStepKineticEnergy  = preStepPoint->GetKineticEnergy();
  postStepKineticEnergy = postStepPoint->GetKineticEnergy();
  stepLength            = aStep->GetStepLength();
  weight                = track->GetWeight();
  
  // attribute the energy deposition to a uniformly random position along the step - correct!
  // random distance - store to use twice to ensure global and local represent the same point
  G4double randDist = G4UniformRand();
  
  // global coordinate positions of the step
  G4ThreeVector posbefore = preStepPoint->GetPosition();
  G4ThreeVector posafter  = postStepPoint->GetPosition();
  G4ThreeVector eDepPos   = posbefore + randDist*(posafter - posbefore);
  
  // global
  X = eDepPos.x();
  Y = eDepPos.y();
  Z = eDepPos.z();

  // Just as the energy deposition is attributed to a uniformly random
  // point between the preStep and the postStep positions, attribute the
  // deposition to random time between preStep and postStep times,
  // using the same random number as for the position.
  G4double preGlobalTime  = preStepPoint->GetGlobalTime();
  G4double postGlobalTime = postStepPoint->GetGlobalTime();
  globalTime = preGlobalTime + randDist * (postGlobalTime - preGlobalTime);
  
  pdgID      = track->GetDefinition()->GetPDGEncoding();
  trackID    = track->GetTrackID();
  parentID   = track->GetParentID(); // needed later on too
  turnsTaken = BDSGlobalConstants::Instance()->TurnsTaken();
  
  //create hits and put in hits collection of the event
  BDSHitEnergyDepositionGlobal* hit = new BDSHitEnergyDepositionGlobal(energy,
								       preStepKineticEnergy,
								       postStepKineticEnergy,
								       stepLength,
								       X, Y, Z,
								       globalTime,
								       pdgID,
								       trackID,
								       parentID,
								       weight,
								       turnsTaken);
  
  // don't worry, won't add 0 energy tracks as filtered at top by if statement
  hits->insert(hit);
   
  return true;
}

G4bool BDSSDEnergyDepositionGlobal::ProcessHitsTrack(const G4Track* track,
						     G4TouchableHistory* /*th*/)
{
  energy = killedParticleMassAddedToEloss ? track->GetTotalEnergy() : track->GetKineticEnergy();
  // if the energy is 0, don't do anything
  if (!BDS::IsFinite(energy))
    {return false;}
  
  preStepKineticEnergy  = track->GetKineticEnergy();
  postStepKineticEnergy = preStepKineticEnergy;
  stepLength            = 0;
  
  const G4ThreeVector& posGlobal = track->GetPosition();
  X = posGlobal.x();
  Y = posGlobal.y();
  Z = posGlobal.z();
  
  globalTime = track->GetGlobalTime();
  pdgID      = track->GetDefinition()->GetPDGEncoding();
  trackID    = track->GetTrackID();
  parentID   = track->GetParentID();
  weight     = track->GetWeight();
  turnsTaken = BDSGlobalConstants::Instance()->TurnsTaken();
  
  BDSHitEnergyDepositionGlobal* hit = new BDSHitEnergyDepositionGlobal(energy,
								       preStepKineticEnergy,
								       postStepKineticEnergy,
								       stepLength,
								       X, Y, Z,
								       globalTime,
								       pdgID,
								       trackID,
								       parentID,
								       weight,
								       turnsTaken);
  hits->insert(hit);
   
  return true;
}

G4VHit* BDSSDEnergyDepositionGlobal::last() const
{
  auto hitsVector = hits->GetVector();
  if (hitsVector->empty())
    {return nullptr;}
  else
    {
      BDSHitEnergyDepositionGlobal* lastHit = hitsVector->back();
      return static_cast<G4VHit*>(lastHit);
    }
}
