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
#ifndef BDSHITTHINTHING_H
#define BDSHITTHINTHING_H

#include "G4Allocator.hh"
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Types.hh"

#include <vector>

class BDSTrajectoryPoint;
class BDSTrajectoryPointHit;
class BDSTrajectoryPrimary;

class BDSHitThinThing; // forward declaration to allow typedef required for static function
typedef G4THitsCollection<BDSHitThinThing> BDSHitsCollectionThinThing;
extern G4Allocator<BDSHitThinThing> BDSAllocatorThinThing;

/**
 * @brief A hit if a particle lost energy in a thin object.
 *
 * Everything public for simplicity of the class.
 * 
 * @author Laurie Nevay
 */

class BDSHitThinThing: public G4VHit
{
public:
  BDSHitThinThing(G4int pdgIDIn,
		  G4int trackIDIn,
		  G4int parentIDIn,
		  G4int turnsTakenIn,
		  BDSTrajectoryPoint* hitIn);

  /// Note this should not be inline when we use a G4Allocator.
  virtual ~BDSHitThinThing();
  
  inline void* operator new(size_t);
  inline void operator delete(void* aHit);
  
  /// Allocate and return a new hit object.
  BDSTrajectoryPointHit* GetNewTrajectoryPointHit() const;

  G4int pdgID;
  G4int trackID;
  G4int parentID;
  G4int turnsTaken;
  BDSTrajectoryPoint* hit;

  /// Utility function to get a vector of trajectory points from the hits collection
  static std::vector<const BDSTrajectoryPoint*> TrajectoryPointsFromHC(BDSHitsCollectionThinThing* hits);
  
  static std::vector<const BDSTrajectoryPointHit*>
  ResolvePossibleEarlierThinHits(const std::vector<const BDSTrajectoryPrimary*>& primaryTrajectoryHits,
				 const BDSHitsCollectionThinThing* thinThingHits);
  
private:
  BDSHitThinThing() = delete; ///< No default constructor.
};

inline void* BDSHitThinThing::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSAllocatorThinThing.MallocSingle();
  return aHit;
}

inline void BDSHitThinThing::operator delete(void* aHit)
{
  BDSAllocatorThinThing.FreeSingle((BDSHitThinThing*) aHit);
}
#endif
