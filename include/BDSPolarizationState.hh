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
#ifndef BDSPOLARIZATIONSTATE_H
#define BDSPOLARIZATIONSTATE_H
#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include "parser/beam.h"

/**
 * @brief Polarization state to pass to processes
 *
 * @author Siobhan Alden
 */

class BDSPolarizationState
{
public:
  BDSPolarizationState() = delete;
  explicit BDSPolarizationState(const GMAD::Beam& beamDefinition);
  ~BDSPolarizationState() = default;

  inline void SetPolarization(const G4ThreeVector& polarizationIn) {polarization = polarizationIn;}
  inline G4ThreeVector GetPolarization() const {return polarization;}

private:
  G4ThreeVector polarization;
};

#endif
