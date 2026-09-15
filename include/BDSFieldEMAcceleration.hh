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

#ifndef BDSFIELDEMACCELERATION_H
#define BDSFIELDEMACCELERATION_H

#include "BDSFieldEM.hh"

#include <vector>


class BDSFieldEMAcceleration : public BDSFieldEM
{
public:
  BDSFieldEMAcceleration();
  ~BDSFieldEMAcceleration();
  G4double Voltage(G4double length, G4int nSteps = 200);
  G4double TransitTimeFactor(G4double length, G4double omega, G4double beta = 1 , G4int nSteps = 200);
  G4double MaxE(G4double length, G4int nSteps = 200);
  G4double MinE(G4double length, G4int nSteps = 200);
  //std::vector<G4double> Cavity_zeroes(G4double length, G4int nSteps = 200);
/*
  G4double Cell_length(std::vector<G4double> Cavity_zeroes);
  G4double Cell_length(G4Double Cell_length);
*/

private:
  static const G4double Z0; ///< Impedance of free space.

};

#endif // BDSFIELDEMACCELERATION_H
