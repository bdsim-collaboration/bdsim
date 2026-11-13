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
#include "BDSAuxiliaryNavigator.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSSDApertureImpacts.hh"
#include "BDSStep.hh"

#include "globals.hh"
#include "G4AffineTransform.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepStatus.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"

BDSSDApertureImpacts::BDSSDApertureImpacts(const G4String& name):
  G4VSensitiveDetector("aperture_impacts/"+name),
  colName(name),
  hits(nullptr),
  HCIDe(-1),
  auxNavigator(new BDSAuxiliaryNavigator())
{
  collectionName.insert(colName);
}

BDSSDApertureImpacts::~BDSSDApertureImpacts()
{
  delete auxNavigator;
}

void BDSSDApertureImpacts::Initialize(G4HCofThisEvent* HCE)
{
  hits = new BDSHitsCollectionApertureImpacts(GetName(),colName);
  if (HCIDe < 0)
    {HCIDe = G4SDManager::GetSDMpointer()->GetCollectionID(hits);}
  HCE->AddHitsCollection(HCIDe,hits);
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Hits Collection ID: " << HCIDe << G4endl;
#endif
}

G4bool BDSSDApertureImpacts::ProcessHits(G4Step* aStep,
					 G4TouchableHistory* /*th*/)
{
  // check if pre step point is on geometry boundary - ie first step into a volume
  G4StepPoint* preStepPoint  = aStep->GetPreStepPoint();
  G4StepStatus preStepStatus = preStepPoint->GetStepStatus();
  if (preStepStatus != G4StepStatus::fGeomBoundary)
    {return false;} // wasn't on a boundary - don't generate a hit

  // check if momentum is away from accelerator axis - ie leaving vacuum and entering
  // beam pip, not coming in from outside
  // get local coordinates
  G4ThreeVector globalPos = preStepPoint->GetPosition();
  G4ThreeVector globalMom = preStepPoint->GetMomentum();
  BDSStep localPosMom = auxNavigator->ConvertToLocal(globalPos,
						     globalMom,
						     0.1*CLHEP::mm);

  G4VSolid* preStepSolid = preStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetSolid();
  G4ThreeVector posLocal = localPosMom.PreStepPoint();
  G4ThreeVector surfaceNormal = preStepSolid->SurfaceNormal(posLocal);

  // check if we have a surface normal pointing into the beam pipe (ie inside
  // of the beam pipe). do this by projecting the surface normal onto a vector
  // from the central curvilinear axis to the local point (radial vector). If
  // +ve, then it's pointing out, else flip it along its direction.
  G4ThreeVector radius(posLocal.x(), posLocal.y(), 0);
  if (radius.dot(surfaceNormal) < 0)
    {surfaceNormal *= -1;}

  // test if leaving or entering by projecting the momentum onto the outwards
  // pointing surface normal
  G4double dotProduct = surfaceNormal.dot(localPosMom.PreStepPoint());
  G4bool leavingBeamPipe = dotProduct >= 0; // +ve means it's leaving beam pipe - ie in direction of unit normal out
  if (!leavingBeamPipe)
    {return false;}

  G4ThreeVector momLocal     = localPosMom.PostStepPoint();
  G4ThreeVector momLocalUnit = momLocal.unit();
  G4double x             = posLocal.x();
  G4double y             = posLocal.y();
  G4int    turnsTaken    = BDSGlobalConstants::Instance()->TurnsTaken();
  G4int    beamlineIndex = -1;

  // get the (global) S coordinate of the hit in the beam line
  G4double S = -1;

  // volume is from curvilinear coordinate parallel geometry
  BDSPhysicalVolumeInfo* theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(localPosMom.VolumeForTransform());
  
  // declare lambda for updating parameters if info found (avoid duplication of code)
  auto UpdateParams = [&](BDSPhysicalVolumeInfo* info)
    {
      S             = info->GetSPos() + posLocal.z();
      beamlineIndex = info->GetBeamlineIndex();
    };
  
  if (theInfo)
    {UpdateParams(theInfo);}
  else
    {// Try again but with the pre step point shifted marginally
      G4ThreeVector unitDir = globalMom.unit();
      G4ThreeVector newPos  = globalPos + 1e-3*unitDir;
      BDSStep stepLocal2 = auxNavigator->ConvertToLocal(newPos, unitDir);
      theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(stepLocal2.VolumeForTransform());
      if (theInfo)
	{UpdateParams(theInfo);}
    }
  
  //create hits and put in hits collection of the event
  G4Track* track = aStep->GetTrack();
  G4int nElectrons = track->GetDynamicParticle()->GetTotalOccupancy();
  BDSHitApertureImpact* hit = new BDSHitApertureImpact(preStepPoint->GetTotalEnergy(),
                                                       preStepPoint->GetKineticEnergy(),
                                                       S,
                                                       x,
                                                       y,
                                                       momLocalUnit.x(),
                                                       momLocalUnit.y(),
                                                       preStepPoint->GetWeight(),
                                                       preStepPoint->GetGlobalTime(),
                                                       track->GetDefinition()->GetPDGEncoding(),
                                                       track->GetTrackID(),
                                                       track->GetParentID(),
                                                       turnsTaken,
                                                       beamlineIndex,
						       nElectrons);

  hits->insert(hit);
   
  return true;
}
