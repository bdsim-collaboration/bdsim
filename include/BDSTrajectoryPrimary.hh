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
#ifndef BDSTRAJECTORYPRIMARY_H
#define BDSTRAJECTORYPRIMARY_H

#include "BDSTrajectory.hh"

#include "G4Allocator.hh"
#include "G4Types.hh"

#include <ostream>

class BDSTrajectoryPoint;
class G4Step;
class G4Track;

namespace BDS
{
  struct TrajectoryOptions;
}

/**
 * @brief Trajectory information for only the primary.
 *
 * Similar to BDSTrajectory but keeps two separate BDSTrajectoryPoints. One
 * for the end of the trajectory (last hit), and one for the first scattering
 * point (first 'hit').
 *
 * @author L. Nevay
 */

class BDSTrajectoryPrimary: public BDSTrajectory
{
public:
  BDSTrajectoryPrimary(const G4Track* aTrack,
		       G4bool         interactive,
		       const BDS::TrajectoryOptions& storageOptionsIn,
		       G4bool         storeTrajectoryPointsIn);
  
  /// copy constructor is not needed
  BDSTrajectoryPrimary(BDSTrajectoryPrimary &) = delete;

  virtual ~BDSTrajectoryPrimary();

  /// Override and return true always.
  virtual G4bool IsPrimary() const {return true;}

  inline void* operator new(size_t);
  inline void operator delete(void*);
  inline int operator == (const BDSTrajectoryPrimary& right) const {return (this==&right);}

  /// Append a step point to this trajectory. This is required for the trajectory
  /// points to show up in the visualisation correctly.
  virtual void AppendStep(const G4Step* aStep);
  
  /// Merge another trajectory into this one. The first hit will always be lower
  /// in this one but the lastpoint could be later, so copy that. Must implement
  /// this function to avoid double deletion if trajectory is merged.
  virtual void MergeTrajectory(G4VTrajectory* secondTrajectory);
  
  /// Output stream
  friend std::ostream& operator<< (std::ostream &out, BDSTrajectoryPrimary const &t);

  /// @{ Accessor.
  const BDSTrajectoryPoint* FirstHit()  const {return firstHit;}
  const BDSTrajectoryPoint* LastPoint() const {return lastPoint;}
  /// @}

  /// Whether this primary trajectory has hit an object (ie physics process invoked).
  /// Judged by casting first hit pointer to Boolean.
  G4bool HasHitSomething() const {return firstHit;}

  /// Whether this primary has scattered on this turn.  It should be
  /// reset at the end of each turn. This is static so it can be done externally.
  static G4bool hasScatteredThisTurn;

protected:
  BDSTrajectoryPoint* firstHit;  ///< Point owned by this class for the first scattering point.
  BDSTrajectoryPoint* lastPoint; ///< Point owned by this class for the last-most point.

private:
  const G4bool storeTrajectoryPoints; ///< Whether to use base class to store all points.
  
  BDSTrajectoryPrimary() = delete; ///< No default constructor required.
};

extern G4Allocator<BDSTrajectoryPrimary> bdsTrajectoryPrimaryAllocator;

inline void* BDSTrajectoryPrimary::operator new(size_t)
{
  void* aTrajectory;
  aTrajectory = (void*)bdsTrajectoryPrimaryAllocator.MallocSingle();
  return aTrajectory;
}

inline void BDSTrajectoryPrimary::operator delete(void* aTrajectory)
{bdsTrajectoryPrimaryAllocator.FreeSingle((BDSTrajectoryPrimary*)aTrajectory);}

#endif
