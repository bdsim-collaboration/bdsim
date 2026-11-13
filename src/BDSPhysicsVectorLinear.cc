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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSPhysicsVectorLinear.hh"

#include "G4Types.hh"

#include <algorithm>
#include <iterator>
#include <vector>


BDSPhysicsVectorLinear::BDSPhysicsVectorLinear(const std::vector<G4double>& kineticEnergyIn,
                                               const std::vector<G4double>& valuesIn):
  kineticEnergy(kineticEnergyIn),
  values(valuesIn),
  eKMin(kineticEnergy[0]),
  eKMax(kineticEnergy.back()),
  eKMinValue(values[0]),
  eKMaxValue(values.back())
{
  if (kineticEnergy.size() != valuesIn.size())
    {throw BDSException(__METHOD_NAME__, "kinetic energy vector and values vector mismatch in size.");}
  if (kineticEnergy.size() < 2)
    {throw BDSException(__METHOD_NAME__, "must be at least 2 points in Ek vs value vectors.");}
}

G4double BDSPhysicsVectorLinear::Value(G4double eK) const
{
  if (eK < eKMin)
    {return eKMinValue;}
  else if (eK > eKMax)
    {return eKMaxValue;}
  else
    {
      auto indexGreaterThanOrEqual = std::lower_bound(kineticEnergy.begin(), kineticEnergy.end(), eK);
      G4int index2 = (G4int)std::distance(kineticEnergy.begin(), indexGreaterThanOrEqual);
      G4int index1 = index2 - 1;
      G4double dx = (eK - kineticEnergy[index1]) / (kineticEnergy[index2] - kineticEnergy[index1]);
      G4double result = values[index1]*(1.0 - dx) + values[index2]*dx;
      return result;
    }
}
