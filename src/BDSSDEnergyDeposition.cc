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
#include "BDSAuxiliaryNavigator.hh"
#include "BDSHitEnergyDeposition.hh"
#include "BDSSDEnergyDeposition.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AffineTransform.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"
#include "Randomize.hh"
#include <map>

BDSSDEnergyDeposition::BDSSDEnergyDeposition(const G4String& name,
                                             G4bool          storeExtrasIn,
                                             G4bool          killedParticleMassAddedToElossIn):
  BDSSensitiveDetector("energy_counter/"+name),
  storeExtras(storeExtrasIn),
  killedParticleMassAddedToEloss(killedParticleMassAddedToElossIn),
  colName(name),
  hits(nullptr),
  HCIDe(-1),
  auxNavigator(new BDSAuxiliaryNavigator())
{
  collectionName.insert(colName);
}

BDSSDEnergyDeposition::~BDSSDEnergyDeposition()
{
  delete auxNavigator;
}

void BDSSDEnergyDeposition::Initialize(G4HCofThisEvent* HCE)
{
  hits = new BDSHitsCollectionEnergyDeposition(GetName(),colName);
  if (HCIDe < 0)
    {HCIDe = G4SDManager::GetSDMpointer()->GetCollectionID(hits);}
  HCE->AddHitsCollection(HCIDe,hits);
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Hits Collection ID: " << HCIDe << G4endl;
#endif
}

G4bool BDSSDEnergyDeposition::ProcessHits(G4Step* aStep,
                                          G4TouchableHistory* /*th*/)
{
  // Get the energy deposited along the step
  G4double energy = aStep->GetTotalEnergyDeposit();

  //if the energy is 0, don't do anything
  if (!BDS::IsFinite(energy))
    {return false;}

  G4Track* track = aStep->GetTrack();
  G4int parentID = track->GetParentID(); // needed later on too
  G4int ptype    = track->GetDefinition()->GetPDGEncoding();

  // step points - used many times
  G4StepPoint* preStepPoint  = aStep->GetPreStepPoint();
  G4StepPoint* postStepPoint = aStep->GetPostStepPoint();

  G4double preStepKineticEnergy = preStepPoint->GetKineticEnergy();
  
  // attribute the energy deposition to a uniformly random position along the step - correct!
  // random distance - store to use twice to ensure global and local represent the same point
  G4double randDist = G4UniformRand();
  
  // global coordinate positions of the step
  const G4ThreeVector& posbefore = preStepPoint->GetPosition();
  const G4ThreeVector& posafter  = postStepPoint->GetPosition();
  G4ThreeVector eDepPos   = posbefore + randDist*(posafter - posbefore);

  // calculate local coordinates
  BDSStep stepLocal = auxNavigator->ConvertToLocal(aStep);
  const G4ThreeVector& posbeforelocal = stepLocal.PreStepPoint();
  const G4ThreeVector& posafterlocal  = stepLocal.PostStepPoint();
  G4ThreeVector eDepPosLocal = posbeforelocal + randDist*(posafterlocal - posbeforelocal);
  G4double stepLength = (posafterlocal - posbeforelocal).mag();
  
  // global
  G4double X = eDepPos.x();
  G4double Y = eDepPos.y();
  G4double Z = eDepPos.z();
  // local
  G4double x = eDepPosLocal.x();
  G4double y = eDepPosLocal.y();
  G4double z = eDepPosLocal.z();

  G4VPhysicalVolume* preStepPV = aStep->GetPreStepPoint()->GetPhysicalVolume();
  if (preStepPV)
  {
    auto it = fVolumeHitCount.find(preStepPV);
    if (it == fVolumeHitCount.end())
    {
      G4cout << "New volume: "
             << std::setw(40) << std::left << preStepPV->GetName()
             << "  address: " << preStepPV
             << G4endl;
      fVolumeHitCount[preStepPV] = {preStepPV->GetName(), 1};
    }
    else
    {it->second.second++;}
  }
  // Just as the energy deposition is attributed to a uniformly random
  // point between the preStep and the postStep positions, attribute the
  // deposition to random time between preStep and postStep times,
  // using the same random number as for the position.
  G4double preGlobalTime  = preStepPoint->GetGlobalTime();
  G4double postGlobalTime = postStepPoint->GetGlobalTime();
  G4double globalTime = preGlobalTime + randDist * (postGlobalTime - preGlobalTime);

  // get the s coordinate (central s + local z)
  // volume is from curvilinear coordinate parallel geometry
  BDSPhysicalVolumeInfo* theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(stepLocal.VolumeForTransform());
  G4int beamlineIndex = -1;
  
  // declare lambda for updating parameters if info found (avoid duplication of code)
  G4double sBefore = -1000;
  G4double sAfter  = -1000;
  auto UpdateParams = [&](BDSPhysicalVolumeInfo* info)
    {
      G4double sCentre = info->GetSPos();
      sAfter           = sCentre + posafterlocal.z();
      sBefore          = sCentre + posbeforelocal.z();
      beamlineIndex    = info->GetBeamlineIndex();
    };
  
  if (theInfo)
    {UpdateParams(theInfo);}
  else
    {
      // Try again but with the pre step point only
      G4ThreeVector unitDirection = (posafter - posbefore).unit();
      BDSStep stepLocal2 = auxNavigator->ConvertToLocal(posbefore, unitDirection);
      theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(stepLocal2.VolumeForTransform());
      if (theInfo)
        {UpdateParams(theInfo);}
      else
        {
          // Try yet again with just a slight shift (100um is bigger than any padding space).
          G4ThreeVector shiftedPos = posbefore + 0.1*CLHEP::mm*unitDirection;
          stepLocal2 = auxNavigator->ConvertToLocal(shiftedPos, unitDirection);
          theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(stepLocal2.VolumeForTransform());
          if (theInfo)
            {UpdateParams(theInfo);}
          else
            {
#ifdef BDSDEBUG
              G4cerr << "No volume info for ";
              auto vol = stepLocal.VolumeForTransform();
              if (vol)
                {G4cerr << vol->GetName() << G4endl;}
              else
                {G4cerr << "Unknown" << G4endl;}
#endif
              // unphysical default value to allow easy identification in output
              sAfter        = -1000;
              sBefore       = -1000;
              beamlineIndex = -2;
            }
        }
    }
  
  G4double sHit = sBefore + randDist*(sAfter - sBefore);

  G4double weight      = track->GetWeight();
  G4int    trackID     = track->GetTrackID();
  G4int    turnsTaken  = BDSGlobalConstants::Instance()->TurnsTaken();
  
  G4int postStepProcessType    = -1;
  G4int postStepProcessSubType = -1;
  if (storeExtras)
    {// physics processes
      const G4StepPoint* postPoint = aStep->GetPostStepPoint();
      const G4VProcess* postProcess = postPoint->GetProcessDefinedStep();
      if (postProcess)
        {
          postStepProcessType = postProcess->GetProcessType();
          postStepProcessSubType = postProcess->GetProcessSubType();
        }
    }
  
  //create hits and put in hits collection of the event
  BDSHitEnergyDeposition* hit = new BDSHitEnergyDeposition(energy,
                                                           sHit,
                                                           weight,
                                                           storeExtras,
                                                           preStepKineticEnergy,
                                                           X, Y, Z,
                                                           x, y, z,
                                                           globalTime,
                                                           ptype,
                                                           trackID,
                                                           parentID,
                                                           turnsTaken,
                                                           stepLength,
                                                           beamlineIndex,
                                                           postStepProcessType,
                                                           postStepProcessSubType);
  
  // don't worry, won't add 0 energy tracks as filtered at top by if statement
  hits->insert(hit);
   
  return true;
}

G4bool BDSSDEnergyDeposition::ProcessHitsTrack(const G4Track* track,
                                               G4TouchableHistory* /*th*/)
{
  G4int    parentID   = track->GetParentID(); // needed later on too
  G4int    ptype      = track->GetDefinition()->GetPDGEncoding();

  // depending on our option, include the rest mass - for backwards compatibility
  G4double energy     = killedParticleMassAddedToEloss ? track->GetTotalEnergy() : track->GetKineticEnergy();
  
  G4double globalTime = track->GetGlobalTime();
  G4double weight     = track->GetWeight();
  G4int    trackID    = track->GetTrackID();
  G4double preStepKineticEnergy = track->GetKineticEnergy();

  // if the energy is 0, don't do anything
  if (!BDS::IsFinite(energy))
    {return false;}
  
  G4double stepLength = 0;
  const G4ThreeVector& posGlobal = track->GetPosition();
  G4double X = posGlobal.x();
  G4double Y = posGlobal.y();
  G4double Z = posGlobal.z();

  // calculate local coordinates
  const G4ThreeVector& momGlobalUnit = track->GetMomentumDirection();
  BDSStep stepLocal = auxNavigator->ConvertToLocal(posGlobal, momGlobalUnit, 1*CLHEP::mm, true, 1*CLHEP::mm);
  G4ThreeVector posLocal = stepLocal.PreStepPoint();
  // local
  G4double x = posLocal.x();
  G4double y = posLocal.y();
  G4double z = posLocal.z();
  
  // get the s coordinate (central s + local z)
  // volume is from curvilinear coordinate parallel geometry
  BDSPhysicalVolumeInfo* theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(stepLocal.VolumeForTransform());
  G4int beamlineIndex = -1;
  
  // declare lambda for updating parameters if info found (avoid duplication of code)
  G4double sBefore = -1000;
  G4double sAfter  = -1000;
  auto UpdateParams = [&](BDSPhysicalVolumeInfo* info)
    {
      G4double sCentre = info->GetSPos();
      sAfter           = sCentre + posLocal.z();
      sBefore          = sCentre + posLocal.z();
      beamlineIndex    = info->GetBeamlineIndex();
    };
  
  if (theInfo)
    {UpdateParams(theInfo);}
  else
    {
      // Try yet again with just a slight shift (100um is bigger than any padding space).
      G4ThreeVector shiftedPos = posGlobal + 0.1*CLHEP::mm * momGlobalUnit;
      BDSStep stepLocal2 = auxNavigator->ConvertToLocal(shiftedPos, momGlobalUnit);
      theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(stepLocal2.VolumeForTransform());
      if (theInfo)
        {UpdateParams(theInfo);}
      else
        {
#ifdef BDSDEBUG
          G4cerr << "No volume info for ";
          auto vol = stepLocal.VolumeForTransform();
          if (vol)
            {G4cerr << vol->GetName() << G4endl;}
          else
            {G4cerr << "Unknown" << G4endl;}
#endif
          // unphysical default value to allow easy identification in output
          sAfter        = -1000;
          sBefore       = -1000;
          beamlineIndex = -2;
        }
    }
  G4double sHit = sBefore; // duplicate

  G4int turnsTaken = BDSGlobalConstants::Instance()->TurnsTaken();

  G4int postStepProcessType    = -1;
  G4int postStepProcessSubType = -1;
  if (storeExtras)
    {// physics processes
      const G4Step* aStep = track->GetStep();
      if (aStep)
        {
          const G4StepPoint* postPoint = aStep->GetPostStepPoint();
          const G4VProcess* postProcess = postPoint->GetProcessDefinedStep();
          if (postProcess)
            {
              postStepProcessType = postProcess->GetProcessType();
              postStepProcessSubType = postProcess->GetProcessSubType();
            }
        }
    }
  
  //create hits and put in hits collection of the event
  BDSHitEnergyDeposition* hit = new BDSHitEnergyDeposition(energy,
                                                           sHit,
                                                           weight,
                                                           storeExtras,
                                                           preStepKineticEnergy,
                                                           X, Y, Z,
                                                           x, y, z,
                                                           globalTime,
                                                           ptype,
                                                           trackID,
                                                           parentID,
                                                           turnsTaken,
                                                           stepLength,
                                                           beamlineIndex,
                                                           postStepProcessType,
                                                           postStepProcessSubType);
  
  // don't worry, won't add 0 energy tracks as filtered at top by if statement
  hits->insert(hit);
   
  return true;
}

G4VHit* BDSSDEnergyDeposition::last() const
{
  BDSHitEnergyDeposition* lastHit = hits->GetVector()->back();
  return dynamic_cast<G4VHit*>(lastHit);
}

void BDSSDEnergyDeposition::EndOfEvent(G4HCofThisEvent*)
{
  if (fVolumeHitCount.empty())
  {return;}
  G4cout << "\n--- Energy Deposition Volume Summary ---" << G4endl;
  G4cout << std::setw(40) << std::left << "Volume Name"
         << std::setw(20) << "Address"
         << std::setw(10) << "Hits"
         << G4endl;
  G4cout << std::string(70, '-') << G4endl;
  for (const auto& entry : fVolumeHitCount)
  {
    G4cout << std::setw(40) << std::left << entry.second.first
           << std::setw(20) << entry.first
           << std::setw(10) << entry.second.second
           << G4endl;
  }
  G4cout << std::string(70, '-') << G4endl;
  fVolumeHitCount.clear();
}