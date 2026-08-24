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
#include "BDSHitSamplerLink.hh"
#include "BDSLinkEventInfo.hh"
#include "BDSLinkRegistry.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSPhysicsUtilities.hh"
#include "BDSSDSamplerLink.hh"

#include "G4DynamicParticle.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Types.hh"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>
#include <limits>
#include <vector>

BDSSDSamplerLink::BDSSDSamplerLink(const G4String& name):
  BDSSensitiveDetector("samplerlink/" + name),
  samplerLinkCollection(nullptr),
  itsCollectionName(name),
  itsHCID(-1),
  registry(nullptr),
  minimumEK(0),
  protonsAndIonsOnly(true)
{
  collectionName.insert(name);
}

BDSSDSamplerLink::~BDSSDSamplerLink()
{;}

void BDSSDSamplerLink::Initialize(G4HCofThisEvent* HCE)
{
  // Create SamplerLink hits collection
  samplerLinkCollection = new BDSHitsCollectionSamplerLink(GetName(),itsCollectionName);

  // Record id for use in EventAction to save time - slow string lookup by collection name
  if (itsHCID < 0)
    {itsHCID = G4SDManager::GetSDMpointer()->GetCollectionID(samplerLinkCollection);}
  HCE->AddHitsCollection(itsHCID,samplerLinkCollection);
}

G4bool BDSSDSamplerLink::ProcessHits(G4Step* aStep, G4TouchableHistory* /*readOutTH*/)
{
  // Do not store hit if the particle pre step point is not on the boundary
  G4StepPoint* postStepPoint = aStep->GetPostStepPoint();
  if (postStepPoint->GetStepStatus() != fGeomBoundary)
    {return false;} // this step was not stored

  G4Track* track = aStep->GetTrack();
  const G4DynamicParticle* dp = track->GetDynamicParticle();
  G4double charge = dp->GetCharge(); // dynamic effective charge
  auto pd = dp->GetParticleDefinition();

  // check against various filters
  G4double ek = track->GetKineticEnergy();
  if (ek < minimumEK)
    {return false;}

  G4int PDGtype = pd->GetPDGEncoding();
  if (protonsAndIonsOnly)
    {
      if (!BDS::IsIon(dp) && PDGtype != 2212)
        {return false;}
    }

  G4int trackID   = track->GetTrackID();           // unique ID of track
  G4int parentID  = track->GetParentID();          // unique ID of track's mother
  G4double T      = track->GetGlobalTime();        // time since beginning of event
  G4double energy = track->GetTotalEnergy();       // total track energy

  const G4ThreeVector& pos = track->GetPosition();          // current particle position (global)
  const G4ThreeVector& mom = track->GetMomentumDirection(); // current particle direction (global) (unit)
  G4double weight   = track->GetWeight();                   // weighting
  G4int nElectrons  = dp->GetTotalOccupancy();
  G4double mass     = dp->GetMass();
  G4double momentum = dp->GetTotalMomentum();

  G4int z = pd->GetAtomicNumber();
  G4int a = pd->GetAtomicMass();

  // In a parallel world track->GetVolume() is the mass-world volume.  As in
  // BDSSDSampler, use the pre-step touchable: ProcessHits is called for the
  // step through the sensitive sampler and its pre-step volume owns the ID.
  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  G4int samplerID = preStepPoint->GetTouchable()->GetVolume()->GetCopyNo();
  const G4Event* event = G4EventManager::GetEventManager()->GetConstCurrentEvent();
  const auto* eventInfo = event ?
    dynamic_cast<const BDSLinkEventInfo*>(event->GetUserInformation()) : nullptr;
  if (eventInfo && eventInfo->linkSamplerID >= 0 &&
      samplerID != eventInfo->linkSamplerID)
    {return false;}
  //G4cout << "samplerID " << samplerID << G4endl;

  // Initialize variables for the local position and direction
  G4ThreeVector localPosition;
  G4ThreeVector localDirection;
  
  // Get coordinate transform and prepare local coordinates
  G4Transform3D globalToLocal = G4Transform3D::Identity;
  G4ThreeVector globalToLocalOffset = G4ThreeVector();
  G4bool noRotation = true;
  if (registry)
    {
      noRotation          = registry->NoRotation(samplerID);
      globalToLocal       = registry->TransformInverse(samplerID);
      globalToLocalOffset = globalToLocal.getTranslation();
    }
  if (noRotation)
    {
      localPosition  = pos + globalToLocalOffset;
      localDirection = mom;
    }
  else if (globalToLocal != G4Transform3D::Identity)
    {
      // The global to local transform is defined in the registry.
      // Cast 3 vector to 'point' to transform position (required to be explicit for * operator)
      localPosition = globalToLocal * (HepGeom::Point3D<G4double>)pos;
      // Cast 3 vector to 3 vector to transform vector (required to be explicit for * operator)
      localDirection = globalToLocal * (HepGeom::Vector3D<G4double>)mom;
    }

  // Project the hit from the finite sampler volume boundary to its nominal
  // centre plane.  The sampler itself is in the parallel world, so this does
  // not perturb navigation through angled mass-world geometry.
  if (std::abs(localDirection.z()) > std::numeric_limits<G4double>::epsilon())
    {
      const G4double pathToNominal = localPosition.z() / localDirection.z();
      localPosition.setX(localPosition.x() - localDirection.x()/localDirection.z()*localPosition.z());
      localPosition.setY(localPosition.y() - localDirection.y()/localDirection.z()*localPosition.z());
      const G4double beta = energy > 0 ? momentum / energy : 0;
      if (beta > 0)
        {T -= pathToNominal / (beta * CLHEP::c_light);}
    }
  localPosition.setZ(0.0);

  BDSParticleCoordsFull coords(localPosition.x(),
			       localPosition.y(),
			       localPosition.z(),
			       localDirection.x(),
			       localDirection.y(),
			       localDirection.z(),
			       T,
			       localPosition.z(), // s = z here
			       energy,
			       weight);

  BDSHitSamplerLink* smpHit = new BDSHitSamplerLink(samplerID,
                                                    coords,
                                                    momentum,
                                                    mass,
                                                    z,
                                                    a,
                                                    charge,
                                                    PDGtype,
                                                    parentID,
                                                    trackID,
                                                    nElectrons);
  
  samplerLinkCollection->insert(smpHit);
  // A link call returns particles at the first output plane it encounters.
  // Without this, a track in the parallel world can continue into delegated
  // elements placed later in the link world and create spurious extra hits.
  track->SetTrackStatus(fStopAndKill);
  return true; // the hit was stored
}

G4VHit* BDSSDSamplerLink::last() const
{
  BDSHitSamplerLink* lastHit = samplerLinkCollection->GetVector()->back();
  return dynamic_cast<G4VHit*>(lastHit);
}
