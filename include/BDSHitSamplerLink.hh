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
#ifndef BDSHITSAMPLERLINK_H
#define BDSHITSAMPLERLINK_H

#include "BDSHitSampler.hh"
#include "BDSParticleCoordsFull.hh"

#include "G4Types.hh"
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

/**
 * @brief The information recorded from a particle impacting a link sampler.
 *
 * Everything public for simplicity of the class.
 * 
 * @author Laurie Nevay
 */

class BDSHitSamplerLink: public BDSHitSampler
{
public:
  BDSHitSamplerLink() = delete; ///< No default constructor.
  BDSHitSamplerLink(G4int samplerIDIn,
		    const BDSParticleCoordsFull& coordsIn,
		    G4double momentumIn,
		    G4double massIn,
		    G4int    ZIn,
		    G4int    AIn,
		    G4double chargeIn,
		    G4int    pdgIDIn,
		    G4int    parentIDIn,
		    G4int    trackIDIn,
		    G4int    nElectronsIn = 0);

  /// Note this should not be inline when we use a G4Allocator.
  virtual ~BDSHitSamplerLink();
  
  inline void* operator new(size_t);
  inline void operator delete(void *aHit);
  
  G4int Z;
  G4int A;
  G4int eventID;
  G4int externalParticleID;
  G4int externalParentID;
};

typedef G4THitsCollection<BDSHitSamplerLink> BDSHitsCollectionSamplerLink;
extern G4Allocator<BDSHitSamplerLink> BDSAllocatorSamplerLink;

inline void* BDSHitSamplerLink::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSAllocatorSamplerLink.MallocSingle();
  return aHit;
}

inline void BDSHitSamplerLink::operator delete(void *aHit)
{
  BDSAllocatorSamplerLink.FreeSingle((BDSHitSamplerLink*) aHit);
}

#endif
