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
#ifndef BDSHITENERGYDEPOSITIONEXTRA_H
#define BDSHITENERGYDEPOSITIONEXTRA_H

#include "globals.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

/**
 * @brief Extra information recorded for a single piece of energy deposition.
 *
 * @author Laurie Nevay
 */

class BDSHitEnergyDepositionExtra
{
public:
  /// Default (in effect) constructor for energy counter hit. The intention (by a lack of
  /// setter methods is that all information should be provided as instantiation time for
  /// an instance of this class.
  BDSHitEnergyDepositionExtra(G4double preStepKineticEnergy,     // pre step point kinetic energy
			      G4double XIn,                      // global x
			      G4double YIn,                      // global y
			      G4double ZIn,                      // global x
			      G4double xIn,                      // local x
			      G4double yIn,                      // local y
			      G4double zIn,                      // local z
			      G4double globalTimeIn,             // global time
			      G4int    partIDIn,                 // PDG id - particle type
			      G4int    trackID,                  // Track ID
			      G4int    parentID,                 // Parent ID
			      G4int    turnsTakenIn,             // turns taken if circular
			      G4double stepLengthIn,
			      G4int    beamlineIndexIn      = -1,
			      G4int    postProcessTypeIn    = -1,
			      G4int    postProcessSubTypeIn = -1);

  /// Note this should not be inline when we use a G4Allocator.
  ~BDSHitEnergyDepositionExtra();
  
  inline void* operator new(size_t) ;
  inline void operator delete(void *aHit);

  
  G4double preStepKineticEnergy;
  
  /// @{ Global coordinate
  G4double X;
  G4double Y;
  G4double Z;
  /// @}
  
  /// @{ Local coordinate
  G4double x;
  G4double y;
  G4double z;
  /// @}

  G4double globalTime; ///< Time since start of event
  G4int    partID;
  G4int    trackID;
  G4int    parentID;
  G4int    turnsTaken;
  G4double stepLength;
  G4int    beamlineIndex;
  
  G4int postStepProcessType;          ///< Process type of post step point
  G4int postStepProcessSubType;       ///< Process sub type of post step point
  
private:
  /// Private default constructor.  
  BDSHitEnergyDepositionExtra() = delete;
};

typedef G4THitsCollection<BDSHitEnergyDepositionExtra> BDSHitsCollectionEnergyDepositionExtra;
extern G4Allocator<BDSHitEnergyDepositionExtra> BDSAllocatorEnergyDepositionExtra;

inline void* BDSHitEnergyDepositionExtra::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSAllocatorEnergyDepositionExtra.MallocSingle();
  return aHit;
}

inline void BDSHitEnergyDepositionExtra::operator delete(void *aHit)
{
 BDSAllocatorEnergyDepositionExtra.FreeSingle((BDSHitEnergyDepositionExtra*) aHit);
}

#endif
