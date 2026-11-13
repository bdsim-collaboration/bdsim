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
#ifndef BDSTUNNELINFO
#define BDSTUNNELINFO

#include "BDSTunnelType.hh"
#include "globals.hh"         // geant4 types / globals

class BDSExtent;
class G4Material;

/**
 * @brief Holder struct of all information required to
 * create a section of tunnel. 
 *
 * This reduces the number of 
 * arguments to all constructors plus aggregates common tasks
 * in the component factory.
 * 
 * @author Laurie Nevay
 */

class BDSTunnelInfo
{
public:
  /// Default constructor, not needed.
  BDSTunnelInfo() = delete;

  /// Extra constructor to assign all members at once.
  BDSTunnelInfo(BDSTunnelType typeIn,
		G4double      thicknessIn,
		G4double      soilThicknessIn,
		G4Material*   materialIn,
		G4Material*   soilMaterialIn,
		G4bool        buildFloorIn,
		G4double      floorOffsetIn,
		G4double      aper1In,
		G4double      aper2In,
		G4bool        sensitiveIn,
		G4bool        visible = true);

  /// Similar constructor, with the exception that the materials are
  /// provided in name form and looked up by this class to be converted
  /// into actual G4Material pointers. The type is also provided as a
  /// string and interpreted to a type.
  BDSTunnelInfo(G4String typeIn,
		G4double thicknessIn,
		G4double soilThicknessIn,
		G4String materialIn,
		G4String soilMaterialIn,
		G4bool   buildFloorIn,
		G4double floorOffsetIn,
		G4double aper1In,
		G4double aper2In,
		G4bool   sensitiveIn,
		G4bool   visible = true);

  BDSExtent IndicativeExtent() const;

  BDSTunnelType type;
  G4double      thickness;
  G4double      soilThickness;
  G4Material*   material;
  G4Material*   soilMaterial;
  G4bool        buildFloor;
  G4double      floorOffset;

  /// Tunnel aperture / shape parameter 1
  G4double      aper1;

  /// Tunnel aperture / shape parameter 2
  G4double      aper2;
  
  /// Is the tunnel sensitive?
  G4bool        sensitive;

  /// Is the tunnel visible?
  G4bool        visible;
};

#endif
