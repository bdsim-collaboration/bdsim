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
#include "BDSHitApertureImpact.hh"

#include "G4Types.hh"
#include "G4Allocator.hh"

G4Allocator<BDSHitApertureImpact> BDSAllocatorApertureImpacts;

BDSHitApertureImpact::BDSHitApertureImpact():
  totalEnergy(0),
  preStepKineticEnergy(0),
  S(0),
  x(0),
  y(0),
  xp(0),
  yp(0),
  weight(0),
  globalTime(0),
  partID(0),
  trackID(0),
  parentID(0),
  turnsTaken(0),
  beamlineIndex(0),
  nElectrons(0)
{;}

BDSHitApertureImpact::BDSHitApertureImpact(G4double totalEnergyIn,
					   G4double preStepKineticEnergyIn,
					   G4double SIn,
					   G4double xIn,
					   G4double yIn,
					   G4double xpIn,
					   G4double ypIn,
					   G4double weightIn,
					   G4double globalTimeIn,
					   G4int    partIDIn,
					   G4int    trackIDIn,
					   G4int    parentIDIn,
					   G4int    turnsTakenIn,
					   G4int    beamlineIndexIn,
					   G4int    nElectronsIn):
  totalEnergy(totalEnergyIn),
  preStepKineticEnergy(preStepKineticEnergyIn),
  S(SIn),
  x(xIn),
  y(yIn),
  xp(xpIn),
  yp(ypIn),
  weight(weightIn),
  globalTime(globalTimeIn),
  partID(partIDIn),
  trackID(trackIDIn),
  parentID(parentIDIn),
  turnsTaken(turnsTakenIn),
  beamlineIndex(beamlineIndexIn),
  nElectrons(nElectronsIn)
{;}

BDSHitApertureImpact::~BDSHitApertureImpact()
{;}
