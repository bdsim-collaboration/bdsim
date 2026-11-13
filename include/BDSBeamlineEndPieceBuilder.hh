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
#ifndef BDSBEAMLINEENDPIECEBUILDER_H
#define BDSBEAMLINEENDPIECEBUILDER_H

class BDSBeamline;

namespace BDS
{
  /// Loop over the flat beam line in BDSAcceleratorModel and inspect each
  /// element seeing if it has end pieces to be placed and if they will fit
  /// without overlapping.  If they will, append them to a separate beam line
  /// of end pieces that can be placed in the world volume.
  BDSBeamline* BuildEndPieceBeamline(const BDSBeamline* beamline,
				     const G4bool circularMachine);
}

#endif
