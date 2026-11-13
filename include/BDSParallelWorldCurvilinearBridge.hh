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
#ifndef BDSPARALLELWORLDCURVILINEARBRIDGE_H
#define BDSPARALLELWORLDCURVILINEARBRIDGE_H

#include "G4String.hh"
#include "G4VUserParallelWorld.hh"

class G4VisAttributes;

/**
 * @brief A parallel world for bridiging curvilinear volumes.
 *
 * Specifically, this curvilinear world is filled with small volumes
 * that cover the gaps in the curvilinear world. This provides a fall
 * back in case we find the world volume in a look up where we would
 * really like to find curvilinear coordinates.
 *
 * @author Laurie Nevay
 */

class BDSParallelWorldCurvilinearBridge: public G4VUserParallelWorld
{
public:
  explicit BDSParallelWorldCurvilinearBridge(G4String name);
  virtual ~BDSParallelWorldCurvilinearBridge();

  /// Construct the required parallel world geometry. This must
  /// overload the pure virtual method in G4VUserParallelWorld.
  virtual void Construct();

private:
  /// No default constructor.
  BDSParallelWorldCurvilinearBridge() = delete;

  /// Just the input part of the name.
  G4String suffix;
  
  /// Visualisation attributes for the world volume
  G4VisAttributes* clbWorldVis;
};

#endif
