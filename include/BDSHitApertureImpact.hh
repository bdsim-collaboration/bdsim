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
#ifndef BDSHITAPERTUREIMPACT_H
#define BDSHITAPERTUREIMPACT_H

#include "globals.hh"
#include "G4VHit.hh"
#include "G4ThreeVector.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

/**
 * @brief Snapshot of information for particle passing through a collimator.
 *
 * Everything public for simplicity of the class.
 * 
 * @author Laurie Nevay
 */

class BDSHitApertureImpact: public G4VHit
{
public:
  BDSHitApertureImpact();
  BDSHitApertureImpact(G4double totalEnergyIn,
		       G4double preStepKineticEnergyIn,
		       G4double SIn,
		       G4double xIn,
		       G4double yIn,
		       G4double pxIn,
		       G4double pyIn,
		       G4double weightIn,
		       G4double globalTimeIn,
		       G4int    partIDIn,
		       G4int    trackIDIn,
		       G4int    parentIDIn,
		       G4int    turnsTakenIn,
		       G4int    beamlineIndexIn,
		       G4int    nElectronsIn);

  /// Note this should not be inline when we use a G4Allocator.
  virtual ~BDSHitApertureImpact();

  inline void* operator new(size_t);
  inline void operator delete(void *aHit);
  
  G4double totalEnergy;
  G4double preStepKineticEnergy;
  G4double S;
  G4double x;
  G4double y;
  G4double xp;
  G4double yp;
  G4double weight;
  G4double globalTime;
  G4int    partID;
  G4int    trackID;
  G4int    parentID;
  G4int    turnsTaken;
  G4int    beamlineIndex;
  G4int    nElectrons;
};

typedef G4THitsCollection<BDSHitApertureImpact> BDSHitsCollectionApertureImpacts;
extern G4Allocator<BDSHitApertureImpact> BDSAllocatorApertureImpacts;

inline void* BDSHitApertureImpact::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSAllocatorApertureImpacts.MallocSingle();
  return aHit;
}

inline void BDSHitApertureImpact::operator delete(void *aHit)
{
  BDSAllocatorApertureImpacts.FreeSingle((BDSHitApertureImpact*) aHit);
}

#endif
