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
#ifndef BDSHITSAMPLER_H
#define BDSHITSAMPLER_H

#include "BDSParticleCoordsFull.hh"

#include "globals.hh"
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

/**
 * @brief The information recorded from a particle impacting a sampler.
 *
 * Everything public for simplicity of the class.
 */

class BDSHitSampler: public G4VHit
{
public:
  BDSHitSampler(G4int samplerIDIn,
		const BDSParticleCoordsFull& coordsIn,
		G4double momentumIn,
		G4double massIn,
		G4double chargeIn,
		G4double rigidityIn,
		G4int    pdgIDIn,
		G4int    parentIDIn,
		G4int    trackIDIn,
		G4int    turnsTakenIn,
		G4int    beamlineIndexIn,
		G4int    nElectronsIn = 0);

  /// Note this should not be inline when we use a G4Allocator.
  virtual ~BDSHitSampler();
  
  inline void* operator new(size_t);
  inline void operator delete(void *aHit);

  G4int                 samplerID;
  BDSParticleCoordsFull coords;
  G4double              momentum;
  G4double              mass;
  G4double              charge; ///< Double as g4 uses charge as a double.
  G4double              rigidity;
  G4int                 pdgID;
  G4int                 parentID;
  G4int                 trackID;
  G4int                 turnsTaken;
  G4int                 beamlineIndex;
  G4int                 nElectrons; ///< Can only get this at inspection time so include here.
  
private:
  BDSHitSampler() = delete; ///< No default constructor.
};

typedef G4THitsCollection<BDSHitSampler> BDSHitsCollectionSampler;
extern G4Allocator<BDSHitSampler> BDSAllocatorSampler;

inline void* BDSHitSampler::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSAllocatorSampler.MallocSingle();
  return aHit;
}

inline void BDSHitSampler::operator delete(void *aHit)
{
  BDSAllocatorSampler.FreeSingle((BDSHitSampler*) aHit);
}

#endif
