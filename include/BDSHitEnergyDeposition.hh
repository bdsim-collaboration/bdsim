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
#ifndef BDSHITENERGYDEPOSITION_H
#define BDSHITENERGYDEPOSITION_H

#include "BDSHitEnergyDepositionExtra.hh"

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

/**
 * @brief Information recorded for a single piece of energy deposition.
 *
 */

class BDSHitEnergyDeposition: public G4VHit
{
public:
  /// Default (in effect) constructor for energy counter hit. The intention (by a lack of
  /// setter methods is that all information should be provided as instantiation time for
  /// an instance of this class.
  BDSHitEnergyDeposition(G4double energyIn,    // energy in this 'hit'
			 G4double sHitIn,
			 G4double weightIn,
			 G4bool   storeExtras,
			 G4double preStepKineticEnergy = 0, // pre step point kinetic energy
			 G4double XIn                  = 0,    // global x
			 G4double YIn                  = 0,    // global y
			 G4double ZIn                  = 0,    // global x
			 G4double xIn                  = 0,    // local x
			 G4double yIn                  = 0,    // local y
			 G4double zIn                  = 0,    // local z
			 G4double globalTimeIn         = 0,    // global time
			 G4int    partIDIn             = 0,    // PDG id - particle type
			 G4int    trackID              = -1,   // Track ID
			 G4int    parentID             = -1,   // Parent ID
			 G4int    turnsTakenIn         = 1,    // turns taken if circular
			 G4double stepLengthIn         = 0,
			 G4int    beamlineIndexIn      = -1,
			 G4int    postStepProcessType  = -1,
			 G4int    postStepProcessSubType = -1,
			 G4int    subElementIDIn         = -1);

  /// Note this should not be inline when we use a G4Allocator.
  virtual ~BDSHitEnergyDeposition();
  
  inline void* operator new(size_t) ;
  inline void operator delete(void *aHit);

  inline G4double GetEnergy()          const {return energy;}
  inline G4double GetWeight()          const {return weight;} 
  inline G4double GetSHit()            const {return sHit;}
  inline G4double GetEnergyWeighted()  const {return weight * energy;}
	inline G4int GetSubElementID() 			 const {return subElementID;}


  /// @{ Accessor for extra piece of information.
  inline G4double GetPreStepKineticEnergy() const {return extra ? extra->preStepKineticEnergy : 0;}
  inline G4double GetX()               const {return extra ? extra->X : 0;} 
  inline G4double GetY()               const {return extra ? extra->Y : 0;}
  inline G4double GetZ()               const {return extra ? extra->Z : 0;}
  inline G4double Getx()               const {return extra ? extra->x : 0;} 
  inline G4double Gety()               const {return extra ? extra->y : 0;}
  inline G4double Getz()               const {return extra ? extra->z : 0;} 
  inline G4double GetGlobalTime()      const {return extra ? extra->globalTime : 0;}
  inline G4int    GetPartID()          const {return extra ? extra->partID : 0;}
  inline G4int    GetTrackID()         const {return extra ? extra->trackID : 0;}
  inline G4int    GetParentID()        const {return extra ? extra->parentID : 0;}
  inline G4int    GetTurnsTaken()      const {return extra ? extra->turnsTaken : 0;}
  inline G4double GetStepLength()      const {return extra ? extra->stepLength : 0;}
  inline G4int    GetBeamlineIndex()   const {return extra ? extra->beamlineIndex : 0;}
  inline G4int    GetPostStepProcessType() const {return extra ? extra->postStepProcessType : -1;}
  inline G4int    GetPostStepProcessSubType() const {return extra ? extra->postStepProcessSubType : -1;}
  /// @}
  
private:
  /// Private default constructor (not implemented) as the constructor
  /// provided as defaults for everything which in effect provides a default
  /// constructor
  BDSHitEnergyDeposition();

  G4double energy;

  /// The position this energy deposition is assigned to. As we represent
  /// a Monte Carlo of discrete processes, this is randomly scattered along
  /// the length of the track this energy deposition corresponds to (uniform
  /// random distribution).
  G4double sHit;

  G4double weight;

	G4int    subElementID;

  BDSHitEnergyDepositionExtra* extra;
};

typedef G4THitsCollection<BDSHitEnergyDeposition> BDSHitsCollectionEnergyDeposition;
extern G4Allocator<BDSHitEnergyDeposition> BDSAllocatorEnergyDeposition;

inline void* BDSHitEnergyDeposition::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSAllocatorEnergyDeposition.MallocSingle();
  return aHit;
}

inline void BDSHitEnergyDeposition::operator delete(void *aHit)
{
 BDSAllocatorEnergyDeposition.FreeSingle((BDSHitEnergyDeposition*) aHit);
}

#endif
