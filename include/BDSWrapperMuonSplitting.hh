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
#ifndef BDSWRAPPERMUONSPLITTING_H
#define BDSWRAPPERMUONSPLITTING_H
#include "BDSWrapperProcess.hh"

#include "G4Types.hh"

class BDSPhysicsVectorLinear;
class G4Step;
class G4Track;
class G4VParticleChange;
class G4VProcess;

/**
 * @brief Wrapper process to produce more muons by resampling the process.
 *
 * Wrap a process. If that process post-step do-it particle change produces a
 * muon, then resample it until we get the desired number of muons. Keep only the
 * new muons. Put together the original secondaries (that are not muons) with the
 * now N muons and weight each muon by w_i * 1/N.
 *
 * Possibility of 2 splitting factors at threshold energies with linear interpolation
 * of the splitting factor in between (rounded to the nearest integer). The splitting
 * ramps up from 0.8x the 1st threshold in kinetic energy. This is a crude linear way
 * to compensate for a possibly exponential in spectra as we don't know the original
 * function.
 *
 * Optional flag for excluding weight = 1 incoming particles for the case of heavy
 * cross-section biasing used separately to this class (e.g. decay).
 *
 * @author Laurie Nevay
 */

class BDSWrapperMuonSplitting: public BDSWrapperProcess
{
public:
  BDSWrapperMuonSplitting() = delete;
  BDSWrapperMuonSplitting(G4VProcess* originalProcess,
                          G4int    splittingFactorIn,
                          G4double splittingThresholdEKIn  = 0,
                          G4int    splittingFactor2In      = 1,
                          G4double splittingThresholdEK2In = 0,
                          G4bool   excludeWeight1Particles = false,
                          G4double muonSplittingExclusionWeightIn = 1e99);
  virtual ~BDSWrapperMuonSplitting();
  
  /// Do the splitting operation.
  virtual G4VParticleChange* PostStepDoIt(const G4Track& track,
                                          const G4Step& step);
  
  /// Counter for understanding occurence.
  static G4int nCallsThisEvent;
  
private:
  G4int splittingFactor;
  G4double splittingThresholdEK;
  G4int splittingFactor2;
  G4double splittingThresholdEK2;
  G4bool excludeWeight1Particles;
  G4double muonSplittingExclusionWeight;
  BDSPhysicsVectorLinear* splitting;
};

#endif
