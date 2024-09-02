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
#ifndef BDSFIELDEMAXIALELECTRIC_H
#define BDSFIELDEMAXIALELECTRIC_H

#include "BDSFieldEMAcceleration.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include <utility>

class BDSCavityInfo;
class BDSMagnetStrength;

/**
 * @brief Circular waveguide transverse magnetic mode
 *
 * @author Stewart Boogert / Lode Vanhecke
 */

class BDSFieldEMAxialElectric: public BDSFieldEMAcceleration
{
public:
  BDSFieldEMAxialElectric() = delete;
  explicit BDSFieldEMAxialElectric(BDSMagnetStrength const* strength);
  
  BDSFieldEMAxialElectric(G4double eFieldAmplitude,
                       G4double radius,
                       G4double length,
                       G4int    m,
                       G4int    n,
                       G4int    p,
                       G4double tphase,
                       G4double zphase,
                       G4bool   travelling,
                       G4double frequency,
                       G4double synchronousTIn);
  
  virtual ~BDSFieldEMAxialElectric(){;}

  /// Accessor to get B and E field.
  virtual std::pair<G4ThreeVector, G4ThreeVector> GetField(const G4ThreeVector& position,
                                                           const G4double       t) const;
  
  virtual G4bool TimeVarying() const {return true;}

  /// Set Maximum E field
  void SetMaxEField(G4double);
  
private:
  G4double eFieldMax;     ///< Maximum field in V/m.
  G4double radius;        ///< Radius a of cavity.
  G4double length;        ///< Length of cavity
  G4int    m;             ///< Azimuthal mode number
  G4int    n;             ///< Radial mode number
  G4int    p;             ///< Longitudinal mode number
  G4double tphase;        ///< Time oscillator phase
  G4double zphase;        ///< Z position phase offset
  G4bool travelling;      ///< Flag for travelling wave field
  G4double frequency;     ///< Cavity frequency  (only required for travelling wave cavity)
  G4double synchronousT;


  G4double voltage;
  G4double ttFactor;

  static const G4double Z0; ///< Impedance of free space.
};

#endif
