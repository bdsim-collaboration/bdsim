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
#ifndef BDSSDTERMINATOR_H
#define BDSSDTERMINATOR_H

#include "G4VSensitiveDetector.hh"

/**
 * @brief Sensitivity that measures primary particle turns for terminator.
 *
 * @author Laurie Nevay
 */

class BDSSDTerminator: public G4VSensitiveDetector
{
public:
  explicit BDSSDTerminator(G4String name);
  virtual ~BDSSDTerminator();

  virtual void   Initialize (G4HCofThisEvent* HCE);
  virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist);
  virtual void   EndOfEvent (G4HCofThisEvent* HCE);

  /// Externally accessible counter for event number. Set in BeginOfEventAction.
  static G4int eventNumber;

private:
  G4int moduloEvents; ///< Cache of print turn number on these events.
  G4int moduloTurns;  ///< Cache of print turn number on these turns.
};

#endif
