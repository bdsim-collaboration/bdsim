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
#ifndef BDSWRAPPERPROCESS_H
#define BDSWRAPPERPROCESS_H
#include "G4Types.hh"
#include "G4VProcess.hh"
#include "G4WrapperProcess.hh"

class G4Track;

/**
 * @brieft Wrapper of wrapper to fix interface.
 * 
 * G4WrapperProcess doesn't override GetCurrentInteractionLength, so no wrapped
 * processes provide a correct current interaction length necessary for tracking.
 * This function isn't virtual so we can't override just that sadly. Therefore,
 * we override the 3 big functions where the member variable currentInteractionLength
 * would be updated and copy it out of the wrapped process. Even if this is updated in
 * Geant4 now we need to retain backwards compatibility.
 *
 * @author Laurie Nevay
 */

class BDSWrapperProcess: public G4WrapperProcess
{
public:
  using G4WrapperProcess::G4WrapperProcess;
  virtual ~BDSWrapperProcess(){;}

  /// Same as base class but update currentInteractionLength.
  virtual G4double AlongStepGetPhysicalInteractionLength(const G4Track& track,
							 G4double  previousStepSize,
							 G4double  currentMinimumStep,
							 G4double& proposedSafety,
							 G4GPILSelection* selection);

  /// Same as base class but update currentInteractionLength.
  virtual G4double AtRestGetPhysicalInteractionLength(const G4Track& track,
						      G4ForceCondition* condition);

  /// Same as base class but update currentInteractionLength.
  virtual G4double PostStepGetPhysicalInteractionLength(const G4Track& track,
							G4double previousStepSize,
							G4ForceCondition* condition);
  
  //inline G4double GetTotalNumberOfInteractionLengthTraversed() const {return pRegProcess->GetTotalNumberOfInteractionLengthTraversed();}
  
  // We only need this but unfortunately this function isn't virtual in G4WrapperProcess,
  // so we can't override it and fix the interface. Therefore, we have to update it in the
  // other calls for path lengths.
  //inline G4double GetCurrentInteractionLength() const {return pRegProcess->GetCurrentInteractionLength();}
};

#endif
