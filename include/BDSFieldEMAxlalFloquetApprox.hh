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

#ifndef BDSFIELDEMAXLALFLOQUETAPPROX_H
#define BBDSFIELDEMAXLALFLOQUETAPPROX_H

#include "BDSFieldEMAcceleration.hh"

class BDSMagnetStrength;

class BDSFieldEMAxialFloquetApprox : public BDSFieldEMAcceleration {
public:
  BDSFieldEMAxialFloquetApprox() = delete;

  explicit BDSFieldEMAxialFloquetApprox(BDSMagnetStrength const *strength);

  BDSFieldEMAxialFloquetApprox(G4double cavity_cell_length,
                               G4double cavity_cell_voltage,
                               G4int cavity_cell_number,
                               G4double cavity_cell_phase_advance,
                               G4double cavity_phase,
                               G4double synchronousT);

  virtual std::pair<G4ThreeVector, G4ThreeVector> GetField(const G4ThreeVector& position,
                                                           const G4double       t) const;

  virtual G4bool TimeVarying() const {return true;}

private:
  G4double cavity_cell_voltage;
  G4double cavity_phase;
  G4int cavity_cell_number;
  G4double cavity_cell_phase_advance;
  G4double cavity_cell_length;
  G4double synchronousT;
};

#endif