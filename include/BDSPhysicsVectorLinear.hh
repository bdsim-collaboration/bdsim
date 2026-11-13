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
#ifndef BDSPHYSICSVECTORLINEAR_H
#define BDSPHYSICSVECTORLINEAR_H

#include "G4Types.hh"

#include <vector>

/**
 *  @brief A vector of some values vs Ek that are linearly interpolated.
 *
 *  The class G4PhysicsVectorLinear provides similar functionality but is
 *  more complex and suited to reading in from a text file which is too
 *  complex for what we want here. This is just a very simple 2 column
 *  lookup with linear interpolation.
 *
 *  @author Laurie Nevay
 */

class BDSPhysicsVectorLinear
{
public:
  BDSPhysicsVectorLinear() = delete;
  /// This constructor may throw an exception if the vectors mismatch in size or are length 1.
  BDSPhysicsVectorLinear(const std::vector<G4double>& kineticEnergyIn,
                         const std::vector<G4double>& values);
  ~BDSPhysicsVectorLinear() = default;

  /// Get the linearly interpolated value for a given kinetic energy.
  G4double Value(G4double eK) const;

private:
  std::vector<G4double> kineticEnergy;
  std::vector<G4double> values;
  G4double eKMin;
  G4double eKMax;
  G4double eKMinValue;
  G4double eKMaxValue;
};

#endif
