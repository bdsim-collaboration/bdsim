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
#ifndef BDSLINKTRACKINGACTION_H
#define BDSLINKTRACKINGACTION_H
#include "BDSTrajectoryOptions.hh"

#include "G4Types.hh"
#include "G4UserTrackingAction.hh"

class BDSLinkEventAction;
class G4Track;

/**
 * @brief Action to decide whether or not to store trajectory information.
 */

class BDSLinkTrackingAction: public G4UserTrackingAction
{
public:
  BDSLinkTrackingAction(G4bool batchMode,
			BDSLinkEventAction* eventActionIn,
			G4int  verboseSteppingEventStartIn,
			G4int  verboseSteppingEventStopIn,
			G4bool verboseSteppingPrimaryOnlyIn,
			G4int  verboseSteppingLevelIn);
  
  virtual ~BDSLinkTrackingAction(){;}

  /// Used to decide whether or not to store trajectories.
  virtual void PreUserTrackingAction(const G4Track* track);

  /// Detect whether track is a primary and if so whether it ended in a collimator.
  virtual void PostUserTrackingAction(const G4Track* track);

private:
  /// No default constructor required.
  BDSLinkTrackingAction() = delete;
  
  /// Whether we're using the visualiser - in which case always store
  /// trajectories for visualisation.
  const G4bool interactive;

  /// Cache of event action to communicate whether a primary stopped in a collimator or not.
  BDSLinkEventAction* eventAction;

  G4int  verboseSteppingEventStart;
  G4int  verboseSteppingEventStop;
  G4bool verboseSteppingPrimaryOnly;
  G4int  verboseSteppingLevel;
  BDS::TrajectoryOptions trajectoryStorageOptions;
};

#endif
