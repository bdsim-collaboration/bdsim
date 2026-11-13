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
#ifndef BDSPARALLELWORLDCURVILINEAR_H
#define BDSPARALLELWORLDCURVILINEAR_H

#include "G4String.hh"
#include "G4VUserParallelWorld.hh"

class G4VisAttributes;

/**
 * @brief A parallel world for curvilinear coordinates.
 *
 * The world isn't used in the main (bdsim.cc) with a parallel world physics
 * process so the boundaries in this world don't affect tracking.
 *
 * @author Laurie Nevay
 */

class BDSParallelWorldCurvilinear: public G4VUserParallelWorld
{
public:
  /// No default constructor.
  BDSParallelWorldCurvilinear() = delete;
  /// Constructor with name of sequence this world is for.
  explicit BDSParallelWorldCurvilinear(const G4String& name);
  virtual ~BDSParallelWorldCurvilinear();

  /// Construct the required parallel world geometry. This must
  /// overload the pure virtual method in G4VUserParallelWorld.
  virtual void Construct();

private:
  /// Just the input part of the name.
  G4String suffix;
  
  /// Visualisation attributes for the world volume
  G4VisAttributes* clWorldVis;
};

#endif
