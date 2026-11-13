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
#ifndef BDSTELEPORTER_H
#define BDSTELEPORTER_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"

class BDSBeamline;
class BDSFieldInfo;

/**
 * @brief An element that unnaturally shifts the beam across its length - a fudge volume.
 * 
 * In some circular lattices, the input lattice doesn't match perfectly at the end, typically
 * due to numerical rounding issues accumulated over thousands of components. This is not
 * because of the geometry construction, but purely in the input MADX or gmad description.
 * Normal tracking programs don't show this offset as they really track through a sequence
 * of maps or functions and just apply them in a loop - it is defacto periodic. However, 
 * with real 3D geometry in global cartesian coordinates, we must artificially make the
 * lattice periodic. Enter the teleporter. This transports particles across its length
 * while maintaining the exact same transverse coordinates, which is normally unphysical.
 * 
 * @author Laurie Nevay
 */

class BDSTeleporter: public BDSAcceleratorComponent
{
public:
  BDSTeleporter(const G4double length,
		const G4double horizontaWidth,
		BDSFieldInfo*  vacuumFieldInfoIn);
  virtual ~BDSTeleporter();

private:
  /// Private default constructor to force the use of the supplied one.
  BDSTeleporter() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSTeleporter& operator=(const BDSTeleporter&) = delete;
  BDSTeleporter(BDSTeleporter&) = delete;
  /// @}
  
  /// Overridden method from BDSAcceleratorComponent that dictates the construction.
  virtual void Build();

  /// Overridden method from BDSAcceleratorComponent that creates the container volume.
  /// This is the only piece of geometry for the teleporter.
  virtual void BuildContainerLogicalVolume();

  /// The full horizontal width of the teleporter.
  G4double horizontalWidth;
  /// Recipe for teleporter 'field'.
  BDSFieldInfo* vacuumFieldInfo;
};

namespace BDS
{
  /// Calculate the difference between the beginning and end of a beam line. Writes
  /// the length to the argument passed by reference.
  G4Transform3D CalculateTeleporterDelta(const BDSBeamline* beamline,
					 G4double& teleporterLength);
}

#endif
