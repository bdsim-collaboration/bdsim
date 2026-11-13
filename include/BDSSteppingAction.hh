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
#ifndef BDSSTEPPINGACTION_H
#define BDSSTEPPINGACTION_H

#include "G4UserSteppingAction.hh"
#include "G4Types.hh"

/**
 * @brief Provide extra output for Geant4 through a verbose stepping action.
 */

class BDSSteppingAction: public G4UserSteppingAction
{
public:
  BDSSteppingAction();
  BDSSteppingAction(G4bool verboseStepIn,
		    G4int  verboseEventStartIn,
		    G4int  verboseEventStopIn);
  virtual ~BDSSteppingAction();

  /// If this event is verbose, then print out verbose stepping information
  /// for this step.
  virtual void UserSteppingAction(const G4Step* step);

private:
  /// The implementation of the print out.
  void VerboseSteppingAction(const G4Step* step);
  
  const G4bool verboseStep;
  const G4bool verboseEventStart;
  const G4bool verboseEventStop;
};

#endif
