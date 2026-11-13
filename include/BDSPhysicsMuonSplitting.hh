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
#ifndef BDSPHYSICSMUONSPLITTING_H
#define BDSPHYSICSMUONSPLITTING_H

#include "BDSSingleUse.hh"

#include "G4Types.hh"
#include "G4VPhysicsConstructor.hh"

/**
 * @brief High energy muon processes.
 *
 * @author Laurie Nevay
 */

class BDSPhysicsMuonSplitting: public G4VPhysicsConstructor, public BDSSingleUse
{
public:
  BDSPhysicsMuonSplitting() = delete;
  explicit BDSPhysicsMuonSplitting(G4int splittingFactorIn,
                                   G4double splittingThresholdEKIn,
                                   G4int splittingFactor2In,
                                   G4double splittingThresholdEK2In,
                                   G4bool excludeWeight1ParticlesIn = false,
                                   G4double muonSplittingExclusionWeightIn = 1e99);
  virtual ~BDSPhysicsMuonSplitting();

  /// Construct all leptons, photons (inc optical), and pion +- just in case.
  virtual void ConstructParticle();

  /// Construct and attach the processes to the relevant particles.
  virtual void ConstructProcess();

private:
  G4int splittingFactor;
  G4double splittingThresholdEK;
  G4int splittingFactor2;
  G4double splittingThresholdEK2;
  G4bool excludeWeight1Particles;
  G4double muonSplittingExclusionWeight;
};
#endif
