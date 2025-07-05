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
#include "BDSAcceleratorModel.hh"
#include "BDSGlobalConstants.hh"
#include "BDSLinkEventAction.hh"
#include "BDSLinkTrackingAction.hh"
#include "BDSPhysicsUtilities.hh"
#include "BDSTrajectory.hh"
#include "BDSTrajectoryOptions.hh"
#include "BDSTrajectoryPrimary.hh"
#include "BDSUtilities.hh"

#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4Types.hh"

#include <set>

class G4LogicalVolume;

BDSLinkTrackingAction::BDSLinkTrackingAction(G4bool batchMode,
                                     BDSLinkEventAction* eventActionIn,
                                     G4int  verboseSteppingEventStartIn,
                                     G4int  verboseSteppingEventStopIn,
                                     G4bool verboseSteppingPrimaryOnlyIn,
                                     G4int  verboseSteppingLevelIn):
  interactive(!batchMode),
  eventAction(eventActionIn),
  verboseSteppingEventStart(verboseSteppingEventStartIn),
  verboseSteppingEventStop(verboseSteppingEventStopIn),
  verboseSteppingPrimaryOnly(verboseSteppingPrimaryOnlyIn),
  verboseSteppingLevel(verboseSteppingLevelIn)
{
  trajectoryStorageOptions = {false, true, true, false, false, false, false, false, false};
}

void BDSLinkTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  G4int  eventIndex = eventAction->CurrentEventIndex();
  G4bool verboseSteppingThisEvent = BDS::VerboseThisEvent(eventIndex, verboseSteppingEventStart, verboseSteppingEventStop);
  G4bool primaryParticle = track->GetParentID() == 0;

  if (primaryParticle && verboseSteppingThisEvent)
    {fpTrackingManager->GetSteppingManager()->SetVerboseLevel(verboseSteppingLevel);}
  else if (!primaryParticle && verboseSteppingThisEvent && !verboseSteppingPrimaryOnly)
    {fpTrackingManager->GetSteppingManager()->SetVerboseLevel(verboseSteppingLevel);}
  
  if (!primaryParticle)
    {// ie secondary particle
      // only store if we want to or interactive
      if (interactive)
        {
   
          auto traj = new BDSTrajectory(track,
                                        interactive,
                                        trajectoryStorageOptions);
          fpTrackingManager->SetStoreTrajectory(1);
          fpTrackingManager->SetTrajectory(traj);
        }
      else // mark as don't store
        {fpTrackingManager->SetStoreTrajectory(0);}
    }
  else
    {// it's a primary particle
      // if it's a primary track then we always store something
      // but only store the actual trajectory points if we explicitly want
      // trajectory points or we're using the visualiser.
      auto traj = new BDSTrajectoryPrimary(track,
                                           interactive, trajectoryStorageOptions, interactive);
      fpTrackingManager->SetStoreTrajectory(1);
      fpTrackingManager->SetTrajectory(traj);
    }
}

void BDSLinkTrackingAction::PostUserTrackingAction(const G4Track* track)
{
  // turn off verbosity always as we selectively turn it on in the start tracking option
  fpTrackingManager->GetSteppingManager()->SetVerboseLevel(0);

  if (track->GetParentID() == 0)
    {
      G4LogicalVolume* lv = track->GetVolume()->GetLogicalVolume();
      std::set<G4LogicalVolume*>* collimators = BDSAcceleratorModel::Instance()->VolumeSet("collimators");
      if (collimators->find(lv) != collimators->end())
        {eventAction->SetPrimaryAbsorbedInCollimator(true);}
    }
}
