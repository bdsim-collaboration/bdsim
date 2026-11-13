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
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSTunnelHit.hh"
#include "BDSTunnelSD.hh"

#include "G4AffineTransform.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4ios.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4RotationMatrix.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"

BDSTunnelSD::BDSTunnelSD(G4String name)
  :G4VSensitiveDetector(name),
   tunnelHitsCollection(nullptr),
   HCID(-1),
   energy(0.0),
   X(0.0),
   Y(0.0),
   Z(0.0),
   S(0.0),
   x(0.0),
   y(0.0),
   z(0.0),
   r(0.0),
   theta(0.0)
{
  verbose = BDSGlobalConstants::Instance()->Verbose();
  collectionName.insert("tunnel_hits");
}

BDSTunnelSD::~BDSTunnelSD()
{;}

void BDSTunnelSD::Initialize(G4HCofThisEvent* HCE)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  //SensitiveDetectorName is member variable name from G4VSensitiveDetector
  tunnelHitsCollection = new BDSTunnelHitsCollection(SensitiveDetectorName,collectionName[0]);
  if (HCID < 0)
    {HCID = G4SDManager::GetSDMpointer()->GetCollectionID(tunnelHitsCollection);}
  HCE->AddHitsCollection(HCID,tunnelHitsCollection);

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "HCID tunnel hits: " << HCID << G4endl;
#endif
}

G4bool BDSTunnelSD::ProcessHits(G4Step* aStep, G4TouchableHistory* readOutTH)
{
  if(BDSGlobalConstants::Instance()->StopTracks())
    {energy = (aStep->GetTrack()->GetTotalEnergy() - aStep->GetTotalEnergyDeposit());} // Why subtract the energy deposit of the step? Why not add?
    //this looks like accounting for conservation of energy when you're killing a particle
  //which may normally break energy conservation for the whole event
  //see developer guide 6.2.2...
  else
    {energy = aStep->GetTotalEnergyDeposit();}
#ifdef BDSDEBUG
  G4cout << "BDSTunnelSD> energy = " << energy << G4endl;
#endif
  //if the energy is 0, don't do anything
  if (energy==0.) return false;      

  G4int nCopy = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();
  
  // Get translation and rotation of volume w.r.t the World Volume
  // get the coordinate transform from the read out geometry instead of the actual geometry
  // if it exsits, else assume on axis. The read out geometry is in accelerator s,x,y
  // coordinates along beam line axis
  G4AffineTransform tf;
  G4VPhysicalVolume* theVolume;
  if (readOutTH)
    {
      tf = readOutTH->GetHistory()->GetTopTransform();
      theVolume = readOutTH->GetVolume();
    }
  else
    {
      tf = (aStep->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform());
      theVolume = aStep->GetPostStepPoint()->GetPhysicalVolume();
    }
  G4ThreeVector posbefore = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector posafter  = aStep->GetPostStepPoint()->GetPosition();

  //calculate local coordinates
  G4ThreeVector posbeforelocal  = tf.TransformPoint(posbefore);
  G4ThreeVector posafterlocal   = tf.TransformPoint(posafter);

  // use the second point as the point of energy deposition
  // originally this was the mean of the pre and post step points, but
  // that appears to give uneven energy deposition about volume edges.
  // global
  X = posafter.x();
  Y = posafter.y();
  Z = posafter.z();
  // local
  x = posafterlocal.x();
  y = posafterlocal.y();
  z = posafterlocal.z();

  // local cylindrical coordinates for output
  r     = sqrt(x*x + y*y);
  theta = atan(y/x);

  // get the s coordinate (central s + local z)
  // true argument denotes it's a tunnel section
  BDSPhysicalVolumeInfo* theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(theVolume, true);
  S = -1000; // unphysical default value to allow easy identification in output
  if (theInfo)
    {S = theInfo->GetSPos() + z;}
  
  G4int event_number = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
  
  if(verbose && BDSGlobalConstants::Instance()->StopTracks())
    {
      G4cout << "BDSTunnelSD: Current Volume: " 
	     << aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() 
	     << "\tEvent:  " << event_number 
	     << "\tEnergy: " << energy/CLHEP::GeV 
	     << "GeV\tPosition: " << S/CLHEP::m <<" m"<< G4endl;
    }
  
  G4double weight = aStep->GetTrack()->GetWeight();
  if (weight == 0)
    {G4cerr << "Error: BDSTunnelSD: weight = 0" << G4endl; exit(1);}
  
  G4int    ptype      = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
  G4String volName    = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
  G4String regionName = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetRegion()->GetName();
  
  G4bool precisionRegion = false;
  if (regionName.contains((G4String)"precisionRegion"))
    {precisionRegion=true;}
  //G4bool precisionRegion = get this info from the logical volume in future
  
  G4int turnstaken    = BDSGlobalConstants::Instance()->TurnsTaken();
  
  //create hits and put in hits collection of the event
  //do analysis / output in end of event action
  BDSTunnelHit* hit = new BDSTunnelHit(nCopy,
				       energy,
				       X,
				       Y,
				       Z,
				       S,
				       x,
				       y,
				       z,
				       r,
				       theta,
				       ptype, 
				       weight, 
				       precisionRegion,
				       turnstaken,
				       event_number);
  
  // don't worry, won't add 0 energy tracks as filtered at top by if statement
  tunnelHitsCollection->insert(hit);

  // this will kill all particles - both primaries and secondaries, but if it's being
  // recorded in an SD that means it's hit something, so ok
  if(BDSGlobalConstants::Instance()->StopTracks())
    {aStep->GetTrack()->SetTrackStatus(fStopAndKill);}
   
  return true;
}

G4bool BDSTunnelSD::ProcessHits(G4GFlashSpot*aSpot, G4TouchableHistory* readOutTH)
{ 
  energy = aSpot->GetEnergySpot()->GetEnergy();
#ifdef BDSDEBUG
  G4cout << "BDSTunnelSD>gflash energy = " << energy << G4endl;
#endif
  if (energy==0.) return false;

  G4VPhysicalVolume* currentVolume;
  if (readOutTH)
    {currentVolume = readOutTH->GetVolume();}
  else
    {currentVolume = aSpot->GetTouchableHandle()->GetVolume();}
  
  G4String           volName        = currentVolume->GetName();
  G4int              nCopy          = currentVolume->GetCopyNo();
  
  // Get Translation and Rotation of Sampler Volume w.r.t the World Volume
  G4AffineTransform tf = (aSpot->GetTouchableHandle()->GetHistory()->GetTopTransform());
  G4ThreeVector pos    = aSpot->GetPosition();

  //calculate local coordinates
  G4ThreeVector poslocal = tf.TransformPoint(pos);
  
  //global
  X = pos.x();
  Y = pos.y();
  Z = pos.z();
  //local
  x = poslocal.x();
  y = poslocal.y();
  z = poslocal.z();

  // local cylindrical coordinates for output
  r     = sqrt(x*x + y*y);
  theta = tan(y/x);

  // get the s coordinate (central s + local z)
  BDSPhysicalVolumeInfo* theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(currentVolume, true);
  G4double sCentral = -1000;
  if (theInfo)
    {sCentral = theInfo->GetSPos();}
  S = sCentral + z;
  
  G4int event_number = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
  
  if(verbose && BDSGlobalConstants::Instance()->StopTracks())
    {
      G4cout << " BDSTunnelSD: Current Volume: " <<  volName 
	     << " Event: "    << event_number 
	     << " Energy: "   << energy/CLHEP::GeV << " GeV"
	     << " Position: " << S/CLHEP::m   << " m" 
	     << G4endl;
    }
  
  G4double weight = aSpot->GetOriginatorTrack()->GetPrimaryTrack()->GetWeight();
  if (weight == 0)
    {G4cerr << "Error: BDSTunnelSD: weight = 0" << G4endl; exit(1);}
  int ptype = aSpot->GetOriginatorTrack()->GetPrimaryTrack()->GetDefinition()->GetPDGEncoding();

  G4int turnstaken = BDSGlobalConstants::Instance()->TurnsTaken();
  
  // see explanation in other processhits function
  BDSTunnelHit* hit = new BDSTunnelHit(nCopy,
				       energy,
				       X,
				       Y,
				       Z,
				       S,
				       x,
				       y,
				       z,
				       r,
				       theta,
				       ptype, 
				       weight, 
				       0,
				       turnstaken,
				       event_number);
  // don't worry, won't add 0 energy tracks as filtered at top by if statement
  tunnelHitsCollection->insert(hit);
  
  return true;
}
