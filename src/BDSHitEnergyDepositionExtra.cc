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
#include "BDSHitEnergyDepositionExtra.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Allocator.hh"

G4Allocator<BDSHitEnergyDepositionExtra> BDSAllocatorEnergyDepositionExtra;

BDSHitEnergyDepositionExtra::BDSHitEnergyDepositionExtra(G4double preStepKineticEnergyIn,
							 G4double XIn, 
							 G4double YIn, 
							 G4double ZIn,
							 G4double xIn,
							 G4double yIn,
							 G4double zIn,
							 G4double globalTimeIn,
							 G4int    partIDIn,
							 G4int    trackIDIn,
							 G4int    parentIDIn,
							 G4int    turnsTakenIn,
							 G4double stepLengthIn,
							 G4int    beamlineIndexIn,
							 G4int    postStepProcessTypeIn,
							 G4int    postStepProcessSubTypeIn):
  preStepKineticEnergy(preStepKineticEnergyIn),
  X(XIn),
  Y(YIn),
  Z(ZIn),
  x(xIn),
  y(yIn),
  z(zIn),
  globalTime(globalTimeIn),
  partID(partIDIn),
  trackID(trackIDIn),
  parentID(parentIDIn),
  turnsTaken(turnsTakenIn),
  stepLength(stepLengthIn),
  beamlineIndex(beamlineIndexIn),
  postStepProcessType(postStepProcessTypeIn),
  postStepProcessSubType(postStepProcessSubTypeIn)
{;}

BDSHitEnergyDepositionExtra::~BDSHitEnergyDepositionExtra()
{;}
