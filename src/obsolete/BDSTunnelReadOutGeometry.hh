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
#ifndef BDSTUNNELREADOUTGEOMETRY_H
#define BDSTUNNELREADOUTGEOMETRY_H

#include "globals.hh"             // geant4 globals / types
#include "G4VReadOutGeometry.hh"
#include "G4VPhysicalVolume.hh"

/**
 * @brief Class that generates readout geometry for tunnel segments.
 *
 * 
 * A la Geant4, this would actually
 * construct a 'parallel' (not to be confused with the G4ParallelWorld
 * class, which is unrelated) world separately and after the main
 * 'mass' world.  However, this is really quite inconvenient so both
 * are constructed at the same time from the same accelerator components
 * and this is a dummy class required by Geant4 that simply gets the 
 * already constructed world and returns it.
 * 
 * @author Laurie Nevay
 */

class BDSTunnelReadOutGeometry: public G4VReadOutGeometry
{
 public:
  BDSTunnelReadOutGeometry(G4String name);

  virtual ~BDSTunnelReadOutGeometry();

private:
  G4VPhysicalVolume* Build();
};

#endif
