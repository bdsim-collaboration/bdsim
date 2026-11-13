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
#ifndef BDSPARALLELWORLDSPLACEMENTFIELDS_H
#define BDSPARALLELWORLDSPLACEMENTFIELDS_H
#include "G4String.hh"
#include "G4VUserParallelWorld.hh"

#include <vector>

class G4VisAttributes;
class G4VPhysicalVolume;

/**
 * @brief A parallel world for placement field transform shapes.
 *
 * @author Laurie Nevay
 */

class BDSParallelWorldPlacementFields: public G4VUserParallelWorld
{
public:
  BDSParallelWorldPlacementFields() = delete;
  explicit BDSParallelWorldPlacementFields(const G4String& name);
  virtual ~BDSParallelWorldPlacementFields();

  /// Construct the required parallel world geometry. This must
  /// overload the pure virtual method in G4VUserParallelWorld.
  virtual void Construct();

private:
  G4VisAttributes* pfWorldVis;
  
  /// Cache of the placements to clean up at the end.
  std::vector<G4VPhysicalVolume*> placements;
};

#endif
