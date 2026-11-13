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
#ifndef BDSHITENERGYDEPOSITIONGLOBAL_H
#define BDSHITENERGYDEPOSITIONGLOBAL_H

#include "globals.hh"
#include "G4VHit.hh"
#include "G4THitsCollection.hh"

/**
 * @brief Information recorded for a step leaving a volume.
 *
 * @author Laurie Nevay
 */

class BDSHitEnergyDepositionGlobal: public G4VHit
{
public:
  /// Default (in effect) constructor for energy deposition hit. The intention (by a lack of
  /// setter methods is that all information should be provided as instantiation time for
  /// an instance of this class.
  BDSHitEnergyDepositionGlobal(G4double totalEnergyIn,
			       G4double preStepKineticEnergyIn,
			       G4double postStepKineticEnergyIn,
			       G4double stepLengthIn,
			       G4double XIn,
			       G4double YIn,
			       G4double ZIn,
			       G4double TIn,
			       G4int    pdgIDIn,
			       G4int    trackIDIn,
			       G4int    parentIDIn,
			       G4double weightIn,
			       G4int    turnsTakenIn);

  /// Note this should not be inline when we use a G4Allocator.
  virtual ~BDSHitEnergyDepositionGlobal();

  inline G4double TotalEnergyWeighted() const {return weight * totalEnergy;}
  inline G4double KineticEnergyWeighted() const {return weight * postStepKineticEnergy;}
  
  G4double totalEnergy;
  G4double preStepKineticEnergy;
  
  /// Unlike kinetic energy recorded elsewhere, this is from the post-step point
  /// rather than the pre-step point because we want the kinetic energy leaving
  /// the volume.
  G4double postStepKineticEnergy;
  G4double stepLength;
  
  /// @{ Global coordinate
  G4double X;
  G4double Y;
  G4double Z;
  G4double T;
  /// @}
  
  G4int    pdgID;
  G4int    trackID;
  G4int    parentID;
  G4double weight;
  G4int    turnsTaken;

private:
  /// Private default constructor (not implemented) as the constructor.
  BDSHitEnergyDepositionGlobal() = delete;
};

typedef G4THitsCollection<BDSHitEnergyDepositionGlobal> BDSHitsCollectionEnergyDepositionGlobal;

#endif
