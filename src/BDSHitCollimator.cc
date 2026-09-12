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
#include "BDSHitCollimator.hh"

#include "G4Allocator.hh"

G4Allocator<BDSHitCollimator> BDSAllocatorCollimator;

BDSHitCollimator::BDSHitCollimator(const BDSBeamline*   beamlineIn,
				   G4int                collimatorIndexIn,
				   const G4ThreeVector& preStepPositionIn,
				   const G4ThreeVector& preStepMomentumIn,
				   G4double             totalEnergyIn,
				   BDSHitEnergyDeposition* energyDepositionHitIn):
  beamline(beamlineIn),
  collimatorIndex(collimatorIndexIn),
  preStepPosition(preStepPositionIn),
  preStepMomentum(preStepMomentumIn),
  totalEnergy(totalEnergyIn),
  energyDepositionHit(energyDepositionHitIn)
{;}

BDSHitCollimator::~BDSHitCollimator()
{;}
