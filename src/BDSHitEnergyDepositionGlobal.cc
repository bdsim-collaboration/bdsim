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
#include "BDSHitEnergyDepositionGlobal.hh"

#include "globals.hh"

BDSHitEnergyDepositionGlobal::BDSHitEnergyDepositionGlobal(G4double totalEnergyIn,
							   G4double preStepKineticEnergyIn,
							   G4double postStepKineticEnergyIn,
							   G4double stepLengthIn,
							   G4double XIn,
							   G4double YIn,
							   G4double ZIn,
							   G4double TIn,
							   G4int    pdgIDIn,
							   G4int    trackIDIn,
							   G4int    parentIDIn,
							   G4double weightIn,
							   G4int    turnsTakenIn):
  totalEnergy(totalEnergyIn),
  preStepKineticEnergy(preStepKineticEnergyIn),
  postStepKineticEnergy(postStepKineticEnergyIn),
  stepLength(stepLengthIn),
  X(XIn),
  Y(YIn),
  Z(ZIn),
  T(TIn),
  pdgID(pdgIDIn),
  trackID(trackIDIn),
  parentID(parentIDIn),
  weight(weightIn),
  turnsTaken(turnsTakenIn)
{;}

BDSHitEnergyDepositionGlobal::~BDSHitEnergyDepositionGlobal()
{;}
