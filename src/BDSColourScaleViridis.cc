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
#include "BDSColourScaleViridis.hh"

#include "G4Colour.hh"
#include "G4Types.hh"

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

std::vector<std::array<G4double, 3>> BDSColourScaleViridis::data = {
  {0.267004, 0.004874, 0.329415},
  {0.277018, 0.050344, 0.375715},
  {0.282327, 0.094955, 0.417331},
  {0.282884, 0.13592 , 0.453427},
  {0.278826, 0.17549 , 0.483397},
  {0.270595, 0.214069, 0.507052},
  {0.258965, 0.251537, 0.524736},
  {0.244972, 0.287675, 0.53726 },
  {0.229739, 0.322361, 0.545706},
  {0.214298, 0.355619, 0.551184},
  {0.19943 , 0.387607, 0.554642},
  {0.185556, 0.41857 , 0.556753},
  {0.172719, 0.448791, 0.557885},
  {0.160665, 0.47854 , 0.558115},
  {0.149039, 0.508051, 0.55725 },
  {0.13777 , 0.537492, 0.554906},
  {0.127568, 0.566949, 0.550556},
  {0.120565, 0.596422, 0.543611},
  {0.120638, 0.625828, 0.533488},
  {0.132268, 0.655014, 0.519661},
  {0.157851, 0.683765, 0.501686},
  {0.196571, 0.711827, 0.479221},
  {0.24607 , 0.73891 , 0.452024},
  {0.304148, 0.764704, 0.419943},
  {0.369214, 0.788888, 0.382914},
  {0.440137, 0.811138, 0.340967},
  {0.515992, 0.831158, 0.294279},
  {0.595839, 0.848717, 0.243329},
  {0.678489, 0.863742, 0.189503},
  {0.762373, 0.876424, 0.137064},
  {0.845561, 0.887322, 0.099702},
  {0.926106, 0.89733 , 0.104071}};


BDSColourScaleViridis::BDSColourScaleViridis():
  dataStep(1.0 / (G4double)(data.size()-1))
{;}

BDSColourScaleViridis::~BDSColourScaleViridis()
{;}

G4Colour BDSColourScaleViridis::GetValue(G4double numberFromZeroToOne) const
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
