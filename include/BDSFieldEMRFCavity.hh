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
#ifndef BDSFIELDEMRFCAVITY_H
#define BDSFIELDEMRFCAVITY_H

#include "BDSFieldEM.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include <utility>

class BDSCavityInfo;
class BDSMagnetStrength;

/**
 * @brief Pill box cavity electromagnetic field.
 *
 * @author Stuart Walker
 */

class BDSFieldEMRFCavity: public BDSFieldEM
{
public:
  BDSFieldEMRFCavity() = delete;
  explicit BDSFieldEMRFCavity(BDSMagnetStrength const* strength);
  
  BDSFieldEMRFCavity(G4double eFieldAmplitude,
                     G4double frequency,
                     G4double phaseOffset,
                     G4double cavityRadius,
                     G4double synchronousTIn);
  
  virtual ~BDSFieldEMRFCavity(){;}

  /// Accessor to get B and E field.
  virtual std::pair<G4ThreeVector, G4ThreeVector> GetField(const G4ThreeVector& position,
                                                           const G4double       t) const;
  
  virtual G4bool TimeVarying() const {return true;}
  
  /// General function put here as it represents the equations in this class.
  static G4double TransitTimeFactor(G4double frequency,
                                    G4double phase,
                                    G4double zLength,
                                    G4double beta);
  
private:
  G4double eFieldMax;    ///< Maximum field in V/m.
  G4double phase;        ///< Phase offset of the oscillator.
  G4double cavityRadius; ///< Radius at maximum extent of cavity.
  G4double synchronousT;

  static const G4double j0FirstZero; ///< X coordinate of first 0 point for bessel J0.
  static const G4double Z0; ///< Impedance of free space.
  const G4double normalisedCavityRadius; ///< Pre-calculated normalised calculated radius w.r.t. bessel first 0.
  const G4double angularFrequency; ///< Angular frequency calculated from frequency - cached to avoid repeated calculation.
};

#endif
