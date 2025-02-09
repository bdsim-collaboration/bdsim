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

#ifndef BDSFIELDEMSTANDINGAXIALAPPROX_H
#define BDSFIELDEMSTANDINGAXIALAPPROX_H

#include "BDSFieldEMAcceleration.hh"

class BDSMagnetStrength;

class BDSFieldEMAxialStandingApprox : public BDSFieldEMAcceleration {
public:
  BDSFieldEMAxialStandingApprox() = delete;

  explicit BDSFieldEMAxialStandingApprox(BDSMagnetStrength const *strength);

  BDSFieldEMAxialStandingApprox(G4double cavity_cell_length,
                                G4double cavity_cell_voltage,
                                G4int cavity_cell_number,
                                G4double cavity_cell_phase_advance,
                                G4double cavity_phase,
                                G4double synchronousT);

  G4double GetEz(G4double z, G4double t) const;
  G4double GetEz_tderiv(G4double z, G4double t) const;
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

  // calculated from input
  G4double eFieldAmplitude;
  G4double cavity_length;
  G4double frequency;
  G4double transitTime;
  std::vector<G4double> cavity_zeroes;
  G4double cell_length;
};
#endif
