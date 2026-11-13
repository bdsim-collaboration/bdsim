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
#include "BDSMultiSensitiveDetectorOrdered.hh"
#include "BDSRunManager.hh"
#include "BDSSDEnergyDeposition.hh"
#include "BDSSDEnergyDepositionGlobal.hh"
#include "BDSStackingAction.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Run.hh"
#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4VSensitiveDetector.hh"
#include "G4Version.hh"

#if G4VERSION_NUMBER > 1029
#include "G4MultiSensitiveDetector.hh"
#endif

G4double BDSStackingAction::energyKilled = 0;

BDSStackingAction::BDSStackingAction(const BDSGlobalConstants* globals)
{
  killNeutrinos     = globals->KillNeutrinos();
  stopSecondaries   = globals->StopSecondaries();
  maxTracksPerEvent = globals->MaximumTracksPerEvent();
  if (maxTracksPerEvent == 0) // 0 is default -> no action - set maximum possible number
    {maxTracksPerEvent = LONG_MAX;}
  minimumEK = globals->MinimumKineticEnergy();
  particlesToExcludeFromCuts = globals->ParticlesToExcludeFromCutsAsSet();
}

BDSStackingAction::~BDSStackingAction()
{;}

G4ClassificationOfNewTrack BDSStackingAction::ClassifyNewTrack(const G4Track * aTrack)
{
  G4ClassificationOfNewTrack classification = fUrgent;

#ifdef BDSDEBUG 
  G4cout<<"StackingAction: ClassifyNewtrack "<<aTrack->GetTrackID()<<
    " "<<aTrack->GetDefinition()->GetParticleName()<<G4endl;
  G4StackManager* SM = G4EventManager::GetEventManager()->GetStackManager();
  G4cout<<"N total, waiting, urgent, postponed tracks: "
	<< std::setw(6) << SM->GetNTotalTrack()    << " : "
	<< std::setw(6) << SM->GetNWaitingTrack()  << " : "
	<< std::setw(6) << SM->GetNUrgentTrack()   << " : "
	<< std::setw(6) << SM->GetNPostponedTrack()
	<< G4endl;
#endif
  G4int pdgCode = aTrack->GetParticleDefinition()->GetPDGEncoding();
  if ( (aTrack->GetKineticEnergy() < minimumEK) && (particlesToExcludeFromCuts.count(pdgCode) == 0) )
    {classification = fKill;}
  
  // If beyond max number of tracks, kill it
  if (aTrack->GetTrackID() > maxTracksPerEvent)
    {classification = fKill;}

  // Optionally kill all neutrinos
  if (killNeutrinos)
    {
      G4int pdgNr = std::abs(pdgCode);
      if( pdgNr == 12 || pdgNr == 14 || pdgNr == 16)
	{classification = fKill;}
    }

  // Optionally kill secondaries
  if (stopSecondaries && (aTrack->GetParentID() > 0))
    {classification = fKill;}

  // Here we must take care of energy conservation. If we artificially kill the track
  // we should record its loss as energy deposition. Find if the volume is sensitive
  // and if so record the track there. Note a track is not a step and is a snap shot at
  // one particular point. Therefore, it has a different method in BDSSDEnergyDeposition.
  if (classification == fKill)
    {
      G4VPhysicalVolume* pv = aTrack->GetVolume();
      if (pv)
	{
	  G4VSensitiveDetector* sd = pv->GetLogicalVolume()->GetSensitiveDetector();
	  if (sd) // SD optional attachment to logical volume
	    {
	      if (auto ecSD = dynamic_cast<BDSSDEnergyDeposition*>(sd))
		{ecSD->ProcessHitsTrack(aTrack, nullptr);}
#if G4VERSION_NUMBER > 1029
	      else if (auto mSD = dynamic_cast<G4MultiSensitiveDetector*>(sd))
		{
		  for (G4int i=0; i < (G4int)mSD->GetSize(); ++i)
		    {
		      if (auto ecSD2 = dynamic_cast<BDSSDEnergyDeposition*>(mSD->GetSD(i)))
			{ecSD2->ProcessHitsTrack(aTrack, nullptr);}
		      if (auto egSD = dynamic_cast<BDSSDEnergyDepositionGlobal*>(mSD->GetSD(i)))
			{egSD->ProcessHitsTrack(aTrack, nullptr);}
		      else if (auto mSDO = dynamic_cast<BDSMultiSensitiveDetectorOrdered*>(sd))
			{
			  for (G4int j=0; j < (G4int)mSDO->GetSize(); ++j)
			    {
			      if (auto ecSD3 = dynamic_cast<BDSSDEnergyDeposition*>(mSDO->GetSD(j)))
				{ecSD3->ProcessHitsTrack(aTrack, nullptr);}
			      // else another SD -> don't use -> based on which SDs are constructed with BDSMultiSensitiveDetectorOrdered
			      // in BDSSDManager. e.g. we dont' need BDSSDEnergyDepositionGlobal here
			    }
			  // else another SD -> don't use
			}
		    }
		}
#endif
	      else if (auto mSDO = dynamic_cast<BDSMultiSensitiveDetectorOrdered*>(sd))
		{
		  for (G4int i=0; i < (G4int)mSDO->GetSize(); ++i)
		    {
		      if (auto ecSD2 = dynamic_cast<BDSSDEnergyDeposition*>(mSDO->GetSD(i)))
			{ecSD2->ProcessHitsTrack(aTrack, nullptr);}
		      // else another SD -> don't use
		    }
		}
	      else
		{energyKilled += aTrack->GetTotalEnergy();} // no suitable SD, but add up anyway
	    }
	  else
	    {energyKilled += aTrack->GetTotalEnergy();} // no SD, but add up anyway
	}
      else
	{energyKilled += aTrack->GetTotalEnergy();} // no PV - unusual but possible - add up anyway
    }
  
  return classification;
}

void BDSStackingAction::NewStage()
{
  // urgent stack empty, looking into the waiting stack
#ifdef BDSDEBUG
  G4cout<<"StackingAction: New stage"<<G4endl;
#endif
}
    
void BDSStackingAction::PrepareNewEvent()
{;}


