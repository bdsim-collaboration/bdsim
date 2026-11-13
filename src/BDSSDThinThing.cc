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
#include "BDSSDThinThing.hh"
#include "BDSTrajectoryOptions.hh"
#include "BDSTrajectoryPoint.hh"
#include "BDSUtilities.hh"

#include "G4String.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4Types.hh"
#include "G4VHit.hh"

#include <vector>

BDSSDThinThing::BDSSDThinThing(const G4String& name,
			       const BDS::TrajectoryOptions& trajectoryOptionsIn):
  BDSSensitiveDetector("thinthing/" + name),
  thinThingCollection(nullptr),
  itsCollectionName(name),
  itsHCID(-1),
  storeTrajectoryLocal(trajectoryOptionsIn.storeLocal),
  storeTrajectoryLinks(trajectoryOptionsIn.storeLinks),
  storeTrajectoryIon(trajectoryOptionsIn.storeIon)
{
  collectionName.insert(name);
}

BDSSDThinThing::~BDSSDThinThing()
{;}

void BDSSDThinThing::Initialize(G4HCofThisEvent* HCE)
{
  // Create ThinThing hits collection
  thinThingCollection = new BDSHitsCollectionThinThing(GetName(), itsCollectionName);

  // Record id for use in EventAction to save time - slow string lookup by collection name
  if (itsHCID < 0)
    {itsHCID = G4SDManager::GetSDMpointer()->GetCollectionID(thinThingCollection);}
  HCE->AddHitsCollection(itsHCID, thinThingCollection);
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Hits Collection ID: " << itsHCID << G4endl;
#endif
}

G4bool BDSSDThinThing::ProcessHits(G4Step* step, G4TouchableHistory* rOHist)
{
  std::vector<G4VHit*> hits;
  return ProcessHitsOrdered(step, rOHist, hits);
}

G4bool BDSSDThinThing::ProcessHitsOrdered(G4Step* step,
					  G4TouchableHistory* /*rOHist*/,
					  const std::vector<G4VHit*>& /*hits*/)
{
  // primary filter applied outside this

  // if scattering point
  G4bool condition1 = BDSTrajectoryPoint::IsScatteringPoint(step);
  // if finite delta p
  G4bool condition2 = BDS::IsFinite(step->GetPostStepPoint()->GetMomentumDirection() - step->GetPreStepPoint()->GetMomentumDirection());
  // if detla E
  G4bool condition3 = BDS::IsFinite(step->GetTotalEnergyDeposit());

  if (condition1 || condition2 || condition3)
    {
      G4Track* track = step->GetTrack();
      G4int turnsTaken = BDSGlobalConstants::Instance()->TurnsTaken();
      BDSHitThinThing* hit = new BDSHitThinThing(track->GetParticleDefinition()->GetPDGEncoding(),
						 track->GetTrackID(),
						 track->GetParentID(),
						 turnsTaken,
						 new BDSTrajectoryPoint(step,
									storeTrajectoryLocal,
									storeTrajectoryLinks,
									storeTrajectoryIon));
      thinThingCollection->insert(hit);
      return true;
    }
  else
    {return false;}
}

G4VHit* BDSSDThinThing::last() const
{
  auto hitsVector = thinThingCollection->GetVector();
  if (hitsVector->empty())
    {return nullptr;}
  else
    {
      BDSHitThinThing* lastHit = hitsVector->back();
      return static_cast<G4VHit*>(lastHit);
    }
}
