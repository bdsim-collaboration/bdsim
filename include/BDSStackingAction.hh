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
#ifndef BDSSTACKINGACTION_H
#define BDSSTACKINGACTION_H

#include "globals.hh"
#include "G4ClassificationOfNewTrack.hh"
#include "G4UserStackingAction.hh"

#include <set>

class BDSGlobalConstants;
class G4Track;

/**
 * @brief BDSIM's Geant4 stacking action.
 */

class BDSStackingAction: public G4UserStackingAction
{
public:
  BDSStackingAction(const BDSGlobalConstants* globals);
  virtual ~BDSStackingAction();

  /// Decide whether to kill tracks if they're neutrinos or we're killing all secondaries. Note
  /// the event won't conserve energy with the stopSecondaries on.
  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
  
  virtual void NewStage(); ///< We don't do anything here.
  virtual void PrepareNewEvent(); ///< We don't do anything here.

  static G4double energyKilled;

private:
  /// Force use of supplied constructor.
  BDSStackingAction() = delete;

  G4bool killNeutrinos;     ///< Local copy of whether to kill neutrinos for tracking efficiency.
  G4bool stopSecondaries;   ///< Whether particles with parentID > 0 will be killed.
  G4long maxTracksPerEvent; ///< Maximum number of tracks before start killing.
  G4double minimumEK;
  std::set<G4int> particlesToExcludeFromCuts;
 };

#endif

