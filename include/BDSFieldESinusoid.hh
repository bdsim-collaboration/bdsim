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
#ifndef BDSFIELDESINUSOID_H
#define BDSFIELDESINUSOID_H

#include "BDSFieldE.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSCavityInfo;
class BDSMagnetStrength;

/**
 * @brief A sinusoidal electric (only) field that doesn't vary with position. Uses cosine.
 * 
 * The default phase is 0 so cosine is used to give acceleration by default.
 * 
 * @author Laurie Nevay
 */

class BDSFieldESinusoid: public BDSFieldE
{
public:
  BDSFieldESinusoid() = delete;
  /// Construct with a BDSMagnetStrength instance for convenience.
  explicit BDSFieldESinusoid(BDSMagnetStrength const* strength);

  /// Construct from E amplitude, unit direction vector, frequency (G4Units) and phase.
  BDSFieldESinusoid(G4double eFieldAmplitude,
                    const G4ThreeVector& unitDirectionIn,
                    G4double frequencyIn,
                    G4double phaseOffsetIn,
                    G4double synchronousTIn);

  virtual ~BDSFieldESinusoid(){;}
  
  virtual G4bool TimeVarying() const {return true;}

  /// Accessor for field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
                                 const G4double       t) const;

protected:
  G4double eField; /// Peak amplitude of electric field.
  const G4ThreeVector unitDirection; ///< Unit vector for direction of field.
  G4double angularFrequency; ///< Angular frequency of field.
  G4double phase; ///< Phase.
  G4double synchronousT; ///< Global T offset for 0 phase.
};

#endif
