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
#ifndef BDSTUNNLTYPE_H
#define BDSTUNNLTYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Type definition for tunnels - used for comparison
 * in factory methods
 * 
 * @author Laurie Nevay
 */

struct tunneltypes_def
{
  enum type {circular, elliptical, square, rectangular, ilc, rectaboveground};
};

typedef BDSTypeSafeEnum<tunneltypes_def,int> BDSTunnelType;

namespace BDS
{
  /// Function to determine the enum type of the tunnel (case-insensitive)
  BDSTunnelType DetermineTunnelType(G4String tunnelType);

  /*
  /// function to check relevant aperture values are set.  This is really a dispatch function
  /// for other aperture specific methods below
  void CheckTunnelInfo(BDSTunnelType tunnelTypeIn,
		       G4double&     tunnelOffsetX,
		       G4double&     tunnelOffsetY,
		       G4bool&       tunnelFloor,
		       G4double&     tunnelFloorOffset,
		       G4double&     tunnel1,
		       G4double&     tunnel2);

  /// aperture info check for circular tunnel
  void InfoOKForCircular(G4double& tunnelOffsetX, G4double& tunnelOffsetY, G4bool tunnelFloor, G4double& tunnelFloorOffset,
			 G4double& tunnel1, G4double& tunnel2);

  /// aperture info check for elliptical tunnel
  void InfoOKForElliptical(G4double& tunnelOffsetX, G4double& tunnelOffsetY, G4bool tunnelFloor, G4double& tunnelFloorOffset,
			   G4double& tunnel1, G4double& tunnel2);

  /// aperture info check for rectangular tunnel
  void InfoOKForRectangular(G4double& tunnelOffsetX, G4double& tunnelOffsetY, G4bool tunnelFloor, G4double& tunnelFloorOffset,
			    G4double& tunnel1, G4double& tunnel2);

  /// aperture info check for rectangular tunnel
  void InfoOKForSquare(G4double& tunnelOffsetX, G4double& tunnelOffsetY, G4bool tunnelFloor, G4double& tunnelFloorOffset,
		       G4double& tunnel1, G4double& tunnel2);
  */
}
#endif
