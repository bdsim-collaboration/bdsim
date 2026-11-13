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
#ifndef BDSBEAMLINEPLACEMENTBUILDER_H
#define BDSBEAMLINEPLACEMENTBUILDER_H

#include "parser/placement.h"

#include <vector>

class BDSBeamline;
class BDSComponentFactory;
class BDSParticleDefinition;

namespace BDS
{
  /// Construct a BDSBeamline instance with all external geometry. The
  /// geometry is constructed using the geometry factory (and owned by it)
  /// but the beam line is necessary for extent calculation for the world.
  /// The parent beam line is required for making placements w.r.t. it.
  BDSBeamline* BuildPlacementGeometry(const std::vector<GMAD::Placement>& placements,
                                      const BDSBeamline* parentBeamLine,
                                      BDSComponentFactory* componentFactory,
                                      const BDSParticleDefinition* designParticle);
}

#endif
