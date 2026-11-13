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
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSHitEnergyDepositionGlobal.hh"
#include "BDSSDVolumeExit.hh"

#include "globals.hh"
#include "G4SDManager.hh"
#include "G4StepPoint.hh"
#include "G4StepStatus.hh"
#include "G4Track.hh"

BDSSDVolumeExit::BDSSDVolumeExit(G4String name,
				 G4bool   worldExitIn):
  G4VSensitiveDetector("volume_exit/" + name),
  colName(name),
  HCIDve(-1),
  hits(nullptr)
{
  collectionName.insert(name);

  statusToMatch = worldExitIn ? G4StepStatus::fWorldBoundary : fGeomBoundary;
}

void BDSSDVolumeExit::Initialize(G4HCofThisEvent* HCE)
{
  hits = new BDSHitsCollectionEnergyDepositionGlobal(GetName(), colName);
  if (HCIDve < 0)
    {HCIDve = G4SDManager::GetSDMpointer()->GetCollectionID(hits);}
  HCE->AddHitsCollection(HCIDve, hits);
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Hits Collection ID: " << HCIDve << G4endl;
#endif
}

G4bool BDSSDVolumeExit::ProcessHits(G4Step* aStep,
				    G4TouchableHistory* /*th*/)
{
  G4StepPoint* postStepPoint = aStep->GetPostStepPoint();

  if (postStepPoint->GetStepStatus() == statusToMatch)
    {
      G4double totalEnergy           = postStepPoint->GetTotalEnergy();
      G4double preStepKineticEnergy  = aStep->GetPreStepPoint()->GetKineticEnergy();
      G4double postStepKineticEnergy = postStepPoint->GetKineticEnergy();
      G4double stepLength            = aStep->GetStepLength();
      G4ThreeVector position         = postStepPoint->GetPosition();
      G4double T          = postStepPoint->GetGlobalTime();
      G4Track* track      = aStep->GetTrack();
      G4int    pdgID      = track->GetDefinition()->GetPDGEncoding();
      G4int    trackID    = track->GetTrackID();
      G4int    parentID   = track->GetParentID();
      G4double weight     = track->GetWeight();
      G4int    turnsTaken = BDSGlobalConstants::Instance()->TurnsTaken();
      
      BDSHitEnergyDepositionGlobal* hit = new BDSHitEnergyDepositionGlobal(totalEnergy,
									   preStepKineticEnergy,
									   postStepKineticEnergy,
									   stepLength,
									   position.x(),
									   position.y(),
									   position.z(),
									   T,
									   pdgID,
									   trackID,
									   parentID,
									   weight,
									   turnsTaken);
      hits->insert(hit);
      return true;
    }
  else
    {return false;}
}
