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
#ifndef BDSTRAJECTORYPOINTLINK_H
#define BDSTRAJECTORYPOINTLINK_H
#include "globals.hh"
#include "G4Allocator.hh"

/**
 * @brief Extra information recorded for a single piece of Trajectory Link information.
 *
 * @author Siobhan Alden
 */

class BDSTrajectoryPointLink
{
public:
  /// Default (in effect) constructor for energy counter hit. The intention (by a lack of
  /// setter methods is that all information should be provided as instantiation time for
  /// an instance of this class.
  BDSTrajectoryPointLink(G4int    chargeIn,
			 G4int    turnsTakenIn,
			 G4double massIn,
			 G4double rigidityIn);
  
  ~BDSTrajectoryPointLink();

  inline void* operator new(size_t) ;
  inline void operator delete(void *aHit);
  
  G4int    charge;
  G4int    turnsTaken;
  G4double mass;
  G4double rigidity;

private:
  /// Private default constructor.  
  BDSTrajectoryPointLink() = delete;
};

extern G4Allocator<BDSTrajectoryPointLink> BDSAllocatorTrajectoryPointLink;

inline void* BDSTrajectoryPointLink::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSAllocatorTrajectoryPointLink.MallocSingle();
  return aHit;
}

inline void BDSTrajectoryPointLink::operator delete(void *aHit)
{
  BDSAllocatorTrajectoryPointLink.FreeSingle((BDSTrajectoryPointLink*) aHit);
}

#endif
