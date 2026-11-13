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
#ifndef BDSLINKSTACKINGACTION_H
#define BDSLINKSTACKINGACTION_H
#include "G4ClassificationOfNewTrack.hh"
#include "G4Types.hh"
#include "G4UserStackingAction.hh"

#include <set>

class BDSGlobalConstants;
class G4Track;

/**
 * @brief BDSIM's Geant4 stacking action.
 */

class BDSLinkStackingAction: public G4UserStackingAction
{
public:
  /// Force use of supplied constructor.
  BDSLinkStackingAction() = delete;

  explicit BDSLinkStackingAction(const BDSGlobalConstants* globals,
                                 const std::set<G4int>&    pdgIDsToAllowIn      = std::set<G4int>(),
                                 G4bool                    protonsAndIonsOnlyIn = true,
                                 G4double                  minimumEKIn          = 0);
  virtual ~BDSLinkStackingAction();

  /// Decide whether to kill tracks if they're neutrinos or we're killing all secondaries. Note
  /// the even won't conserve energy with the stopSecondaries on.
  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);

  static G4double kineticEnergyKilled;

private:
  G4bool killNeutrinos;     ///< Local copy of whether to kill neutrinos for tracking efficiency.
  G4bool stopSecondaries;   ///< Whether particles with parentID > 0 will be killed.
  G4long maxTracksPerEvent; ///< Maximum number of tracks before start killing.
  std::set<G4int> pdgIDsToAllow;
  G4bool emptyPDGIDs;
  G4bool protonsAndIonsOnly;
  G4double minimumEK;       ///< Minimum kinetic energy to generate a hit for.
 };

#endif

