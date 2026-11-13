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
#ifndef BDSTRAJECTORYPOINTION_H
#define BDSTRAJECTORYPOINTION_H
#include "globals.hh"
#include "G4Allocator.hh"

/**
 * @brief Extra information recorded for a single piece of energy deposition.
 *
 * @author Siobhan Alden
 */

class BDSTrajectoryPointIon
{
public:
  /// Default (in effect) constructor for energy counter hit. The intention (by a lack of
  /// setter methods is that all information should be provided as instantiation time for
  /// an instance of this class.
  BDSTrajectoryPointIon(G4bool isIonIn,
			G4int ionAIn,
			G4int ionZIn,
			G4int nElectronsIn);

  ~BDSTrajectoryPointIon();

  inline void* operator new(size_t) ;
  inline void operator delete(void *aHit);

  G4bool 	isIon;
  G4int  	ionA;
  G4int		ionZ;
  G4int		nElectrons;
  
private:
  /// Private default constructor.  
  BDSTrajectoryPointIon() = delete;
};

extern G4Allocator<BDSTrajectoryPointIon> BDSAllocatorTrajectoryPointIon;

inline void* BDSTrajectoryPointIon::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSAllocatorTrajectoryPointIon.MallocSingle();
  return aHit;
}

inline void BDSTrajectoryPointIon::operator delete(void *aHit)
{
 BDSAllocatorTrajectoryPointIon.FreeSingle((BDSTrajectoryPointIon*) aHit);
}

#endif
