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
#include "BDSWrapperProcess.hh"

#include "G4Types.hh"
#include "G4VProcess.hh"

G4double BDSWrapperProcess::AlongStepGetPhysicalInteractionLength(const G4Track& track,
                                       G4double  previousStepSize,
                                       G4double  currentMinimumStep,
                                       G4double& proposedSafety,
                                       G4GPILSelection* selection)
{
  G4double result = pRegProcess->AlongStepGetPhysicalInteractionLength(track,
                                           previousStepSize,
                                           currentMinimumStep,
                                           proposedSafety,
                                           selection);
  currentInteractionLength = pRegProcess->GetCurrentInteractionLength();
  theNumberOfInteractionLengthLeft = pRegProcess->GetNumberOfInteractionLengthLeft();
  return result;
}

G4double BDSWrapperProcess::AtRestGetPhysicalInteractionLength(const G4Track& track,
                                                               G4ForceCondition* condition)
{
  G4double result = pRegProcess->AtRestGetPhysicalInteractionLength( track, condition);
  currentInteractionLength = pRegProcess->GetCurrentInteractionLength();
  theNumberOfInteractionLengthLeft = pRegProcess->GetNumberOfInteractionLengthLeft();
  return result;
}

G4double BDSWrapperProcess::PostStepGetPhysicalInteractionLength(const G4Track& track,
                                                                 G4double previousStepSize,
                                                                 G4ForceCondition* condition)
{
  G4double value = pRegProcess->PostStepGetPhysicalInteractionLength(track,
                                                                     previousStepSize,
                                                                     condition);
  currentInteractionLength = pRegProcess->GetCurrentInteractionLength();
  theNumberOfInteractionLengthLeft = pRegProcess->GetNumberOfInteractionLengthLeft();
  return value;
}