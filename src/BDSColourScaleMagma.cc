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
#include "BDSColourScaleMagma.hh"

#include "G4Colour.hh"
#include "G4Types.hh"

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

std::vector<std::array<G4double, 3>> BDSColourScaleMagma::data = {
  {1.46200e-03, 4.66000e-04, 1.38660e-02},
  {1.37080e-02, 1.17710e-02, 6.86670e-02},
  {3.96080e-02, 3.10900e-02, 1.33515e-01},
  {7.42570e-02, 5.20170e-02, 2.02660e-01},
  {1.13094e-01, 6.54920e-02, 2.76784e-01},
  {1.59018e-01, 6.83540e-02, 3.52688e-01},
  {2.11718e-01, 6.19920e-02, 4.18647e-01},
  {2.65447e-01, 6.02370e-02, 4.61840e-01},
  {3.16654e-01, 7.16900e-02, 4.85380e-01},
  {3.66012e-01, 9.03140e-02, 4.97960e-01},
  {4.14709e-01, 1.10431e-01, 5.04662e-01},
  {4.63508e-01, 1.29893e-01, 5.07652e-01},
  {5.12831e-01, 1.48179e-01, 5.07648e-01},
  {5.62866e-01, 1.65368e-01, 5.04692e-01},
  {6.13617e-01, 1.81811e-01, 4.98536e-01},
  {6.64915e-01, 1.98075e-01, 4.88836e-01},
  {7.16387e-01, 2.14982e-01, 4.75290e-01},
  {7.67398e-01, 2.33705e-01, 4.57755e-01},
  {8.16914e-01, 2.55895e-01, 4.36461e-01},
  {8.63320e-01, 2.83729e-01, 4.12403e-01},
  {9.04281e-01, 3.19610e-01, 3.88137e-01},
  {9.37221e-01, 3.64929e-01, 3.68567e-01},
  {9.60949e-01, 4.18323e-01, 3.59630e-01},
  {9.76690e-01, 4.76226e-01, 3.64466e-01},
  {9.86700e-01, 5.35582e-01, 3.82210e-01},
  {9.92785e-01, 5.94891e-01, 4.10283e-01},
  {9.96096e-01, 6.53659e-01, 4.46213e-01},
  {9.97325e-01, 7.11848e-01, 4.88154e-01},
  {9.96898e-01, 7.69591e-01, 5.34892e-01},
  {9.95131e-01, 8.27052e-01, 5.85701e-01},
  {9.92440e-01, 8.84330e-01, 6.40099e-01},
  {9.89434e-01, 9.41470e-01, 6.97519e-01}
};


BDSColourScaleMagma::BDSColourScaleMagma():
  dataStep(1.0 / (G4double)(data.size()-1))
{;}

BDSColourScaleMagma::~BDSColourScaleMagma()
{;}

G4Colour BDSColourScaleMagma::GetValue(G4double numberFromZeroToOne) const
{
  numberFromZeroToOne = std::min(numberFromZeroToOne, 1.0); // ensure <= 1
  numberFromZeroToOne = std::max(0.0, numberFromZeroToOne); // ensure >= 0
  // (x - xMin) / xStep
  // xMin is 0 by definition
  G4double arrayCoords = numberFromZeroToOne / dataStep;
  G4int arrayIndex = (G4int) std::floor(arrayCoords);

  const auto& value = data.at(arrayIndex);
  return G4Colour(value[0], value[1], value[2]);
}
