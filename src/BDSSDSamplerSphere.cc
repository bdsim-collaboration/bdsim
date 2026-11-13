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
#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"
#include "BDSHitSamplerSphere.hh"
#include "BDSParticleCoordsSpherical.hh"
#include "BDSPhysicalConstants.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSSDSamplerSphere.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AffineTransform.hh"
#include "G4DynamicParticle.hh"
#include "G4ParticleDefinition.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"

#include <vector>

BDSSDSamplerSphere::BDSSDSamplerSphere(const G4String& name):
  BDSSensitiveDetector("samplersphere/" + name),
  samplerHitsCollection(nullptr),
  itsCollectionName(name),
  itsHCID(-1),
  registry(nullptr),
  globals(nullptr)
{
  collectionName.insert(name);
}

BDSSDSamplerSphere::~BDSSDSamplerSphere()
{;}

void BDSSDSamplerSphere::Initialize(G4HCofThisEvent* HCE)
{
  // Create Sampler hits collection
  samplerHitsCollection = new BDSHitsCollectionSamplerSphere(GetName(), itsCollectionName);

  // Record id for use in EventAction to save time - slow string lookup by collection name
  if (itsHCID < 0)
    {itsHCID = G4SDManager::GetSDMpointer()->GetCollectionID(samplerHitsCollection);}
  HCE->AddHitsCollection(itsHCID,samplerHitsCollection);

  registry = BDSSamplerRegistry::Instance(); // cache pointer to registry
  globals  = BDSGlobalConstants::Instance(); // cache pointer to globals
}

G4bool BDSSDSamplerSphere::ProcessHits(G4Step* aStep, G4TouchableHistory* /*readOutTH*/)
{
  // Do not store hit if the particle pre step point is not on the boundary
  G4StepPoint* postStepPoint = aStep->GetPostStepPoint();
  if(postStepPoint->GetStepStatus() != fGeomBoundary)
    {return false;} // this step was not stored
  
  G4Track* track    = aStep->GetTrack();
  const G4DynamicParticle* dp = track->GetDynamicParticle();
  G4int trackID     = track->GetTrackID();
  G4int parentID    = track->GetParentID();
  G4double T        = track->GetGlobalTime();
  G4double energy   = track->GetTotalEnergy();
  G4double charge   = dp->GetCharge(); // dynamic effective charge
  G4int turnsTaken  = globals->TurnsTaken();
  const G4ThreeVector& pos = track->GetPosition();          // current particle position (global)
  const G4ThreeVector& mom = track->GetMomentumDirection(); // current particle direction (global) (unit)
  G4double weight   = track->GetWeight();            // weighting
  G4int nElectrons  = dp->GetTotalOccupancy();
  G4double mass     = dp->GetMass();
  G4double rigidity = 0;
  if (BDS::IsFinite(charge))
    {rigidity = BDS::Rigidity(track->GetMomentum().mag(), charge);}
  G4double p = dp->GetTotalMomentum();
  
  // The copy number of physical volume is the sampler ID in BDSIM scheme.
  // track->GetVolume gives the volume in the mass world. pre/postStepPoint->->GetVolume()
  // give the ones in the parallel sampler world this SD is attached to. If the post step
  // point is on a boundary, it belongs to the next volume - ie not the one of interest
  // so always use the pre step point for volume identification.
  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  G4int samplerID = preStepPoint->GetTouchable()->GetVolume()->GetCopyNo();

  //Initialize variables for the local position and direction
  G4ThreeVector localPosition;
  G4ThreeVector localDirection;
  
  // Get coordinate transform and prepare local coordinates
  G4Transform3D globalToLocal = registry->GetTransformInverse(samplerID);
  if (globalToLocal == G4Transform3D::Identity) // no transform was provided - look it up
    {
      // Transform not provided so look up geometry and get the transform. OK in mass world
      // but error-prone in parallel worlds for very thin volumes.
      // NOTE: we're looking up mass world here!
      G4AffineTransform tf = preStepPoint->GetTouchableHandle()->GetHistory()->GetTopTransform();
      localPosition  = tf.TransformPoint(pos);
      localDirection = tf.TransformAxis(mom);
    }
  else
    {
      // The global to local transform is defined in the registry.
      // Cast 3 vector to 'point' to transform position (required to be explicit for * operator)
      localPosition  = globalToLocal * (HepGeom::Point3D<G4double>)pos;
      // Cast 3 vector to 3 vector to transform vector (required to be explicit for * operator)
      localDirection = globalToLocal * (HepGeom::Vector3D<G4double>)mom;
    }

  const BDSSamplerPlacementRecord& info = registry->GetInfo(samplerID);
  G4double s           = info.SPosition();
  G4int beamlineIndex  = info.BeamlineIndex();
  G4int    PDGtype     = track->GetDefinition()->GetPDGEncoding();
  G4String pName       = track->GetDefinition()->GetParticleName();

  // spherical coords
  G4double r = localPosition.mag();
  G4ThreeVector unitR = localPosition.unit();
  G4ThreeVector lds = localDirection - unitR; // localDirectionSpherical
  
  G4double rp = localDirection.dot(unitR);
  G4double thetap = localDirection.theta(unitR);
  G4double phip = localDirection.deltaPhi(unitR);
  
  BDSParticleCoordsSpherical coords(r, localPosition.theta(), localPosition.phi(),
                                    rp, thetap, phip, T);

  BDSHitSamplerSphere* smpHit = new BDSHitSamplerSphere(samplerID,
							    coords,
							    energy,
							    weight,
							    s,
							    p,
							    mass,
							    charge,
							    rigidity,
							    PDGtype,
							    parentID,
							    trackID,
							    turnsTaken,
							    beamlineIndex,
							    nElectrons);
  
  samplerHitsCollection->insert(smpHit);
  return true; // the hit was stored
}

G4VHit* BDSSDSamplerSphere::last() const
{
  BDSHitSamplerSphere* lastHit = samplerHitsCollection->GetVector()->back();
  return dynamic_cast<G4VHit*>(lastHit);
}
