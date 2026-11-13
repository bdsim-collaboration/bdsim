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
#ifndef BDSTRAJECTORYPOINTHIT_H
#define BDSTRAJECTORYPOINTHIT_H
#include "BDSTrajectoryPoint.hh"
#include "G4Types.hh"

class BDSTrajectory;

/**
 * @brief A summary trajectory object of a loss point.
 * 
 * A much simpler trajectory with one point and a few extra variables
 * so we can correctly fill out loss points as sampler like information.
 *
 * @author Laurie Nevay
 */

class BDSTrajectoryPointHit
{
public:
  BDSTrajectoryPointHit() = delete;
  BDSTrajectoryPointHit(const BDSTrajectoryPointHit& other) = delete;

  BDSTrajectoryPointHit(const BDSTrajectory*      trajectory,
			const BDSTrajectoryPoint* pointIn);
  
  BDSTrajectoryPointHit(G4int pdgIDIn,
                        G4int trackIDIn,
                        G4int parentIDIn,
                        const BDSTrajectoryPoint* pointIn);
  
  ~BDSTrajectoryPointHit(){;} // point deleted elsewhere
  
  // logic taken from BDSExtent - implement only one operator; rest come for free
  /// @{ Comparison operator.
  inline G4bool operator< (const BDSTrajectoryPointHit& other) const;
  inline G4bool operator> (const BDSTrajectoryPointHit& other) const {return other < (*this);}
  inline G4bool operator<=(const BDSTrajectoryPointHit& other) const {return !((*this) > other);}
  inline G4bool operator>=(const BDSTrajectoryPointHit& other) const {return !((*this) < other);}
  /// @}
  
  G4int pdgID;
  G4int trackID;
  G4int parentID;
  const BDSTrajectoryPoint* point;
};

inline G4bool BDSTrajectoryPointHit::operator< (const BDSTrajectoryPointHit& other) const
{
  return point->operator<(*other.point);
}

#endif
