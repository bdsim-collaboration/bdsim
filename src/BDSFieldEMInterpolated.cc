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
#include "BDSFieldEMInterpolated.hh"
#include "BDSInterpolator.hh"

#include <algorithm>
#include <limits>

BDSFieldEMInterpolated::BDSFieldEMInterpolated(const BDSInterpolator* eInterpolator,
					       const BDSInterpolator* bInterpolator,
					       const G4Transform3D&   offset,
					       G4double               eScalingIn,
					       G4double               bScalingIn):
  BDSFieldEM(offset),
  timeVarying(false),
  eScaling(eScalingIn),
  bScaling(bScalingIn),
  smallestSpatialStep(std::numeric_limits<double>::max())
{
  if (eInterpolator)
    {
      smallestSpatialStep = std::min(smallestSpatialStep, eInterpolator->SmallestSpatialStep());
      timeVarying = timeVarying | eInterpolator->TimeVarying();
    }
  if (bInterpolator)
    {
      smallestSpatialStep = std::min(smallestSpatialStep, bInterpolator->SmallestSpatialStep());
      timeVarying = timeVarying | bInterpolator->TimeVarying();
    }
}
