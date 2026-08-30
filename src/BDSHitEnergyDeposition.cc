/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSHitEnergyDeposition.hh"
#include "BDSHitEnergyDepositionExtra.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Allocator.hh"

G4Allocator<BDSHitEnergyDeposition> BDSAllocatorEnergyDeposition;

BDSHitEnergyDeposition::BDSHitEnergyDeposition(G4double energyIn,
					       G4double sHitIn,
					       G4double weightIn,
					       G4bool   storeExtras,
					       G4double preStepKineticEnergy,
					       G4double X, 
					       G4double Y, 
					       G4double Z,
					       G4double x,
					       G4double y,
					       G4double z,
					       G4double globalTime,
					       G4int    partID,
					       G4int    trackID,
					       G4int    parentID,
					       G4int    turnsTaken,
					       G4double stepLength,
					       G4int    beamlineIndex,
					       G4int    postStepProcessType,
					       G4int    postStepProcessSubType,
								 G4int    subElementIDIn):
  energy(energyIn),
  sHit(sHitIn),
  weight(weightIn),
  subElementID(subElementIDIn),
  extra(nullptr)
{
  if (storeExtras)
    {
      extra = new BDSHitEnergyDepositionExtra(preStepKineticEnergy,
					      X, Y, Z, x, y, z, globalTime,
					      partID, trackID, parentID,
					      turnsTaken, stepLength,
					      beamlineIndex, postStepProcessType, postStepProcessSubType);
    }
}

BDSHitEnergyDeposition::~BDSHitEnergyDeposition()
{
  delete extra;
}
