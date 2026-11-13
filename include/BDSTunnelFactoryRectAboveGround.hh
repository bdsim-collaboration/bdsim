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
#ifndef BDSTUNNELFACTORYRECTABOVEGROUND_H
#define BDSTUNNELFACTORYRECTABOVEGROUND_H

#include "BDSTunnelFactoryBase.hh"

#include "globals.hh" // geant4 globals / types
#include "G4ThreeVector.hh"

class BDSTunnelSection;
class G4VSolid;

/**
 * @brief Factory for rectangular tunnel segments above ground.
 * 
 * There is no soil. Builds tunnel, floor (optional) and concrete 
 * slab that it all sits on.
 * 
 * @author Laurie Nevay
 */

class BDSTunnelFactoryRectAboveGround: public BDSTunnelFactoryBase
{
public:
  BDSTunnelFactoryRectAboveGround();
  virtual ~BDSTunnelFactoryRectAboveGround(){;}
  
  /// Create a tunnel section with flat input and output faces.
  virtual BDSTunnelSection* CreateTunnelSection(G4String    name,
						G4double    length,
						G4double    tunnelThickness,
						G4double    tunnelSoilThickness,
						G4Material* tunnelMaterial,
						G4Material* tunnelSoilMaterial,
						G4bool      tunnelFloor,
						G4double    tunnelFloorOffset,
						G4double    tunnel1,
						G4double    tunnel2,
						G4bool      visible);

  /// Create a tunnel section with an angled input and output face.
  virtual BDSTunnelSection* CreateTunnelSectionAngled(G4String      name,
						      G4double      length,
						      G4ThreeVector inputFace,
						      G4ThreeVector outputFace,
						      G4double      tunnelThickness,
						      G4double      tunnelSoilThickness,
						      G4Material*   tunnelMaterial,
						      G4Material*   tunnelSoilMaterial,
						      G4bool        tunnelFloor,
						      G4double      tunnelFloorOffset,
						      G4double      tunnel1,
						      G4double      tunnel2,
						      G4bool        visible);
							       
private:
  /// Test input parameters - this is only for basic viability not in comparison to
  /// the accelerator model or other components
  void TestInputParameters(G4double&    length,
			   G4double&    tunnelThickness,
			   G4double&    tunnelSoilThickness,
			   G4Material*& tunnelMaterial,
			   G4Material*& tunnelSoildMaterial,
			   G4double&    tunnel1,
			   G4double&    tunnel2);

  virtual void CleanUp();
  
  G4VSolid* BuildContainerStraight(G4String name,
				   G4double lengthIn,
				   G4double tunnel1,
				   G4double tunnel2,
				   G4double tunnelThickness,
				   G4ThreeVector slabDisplacement);

  G4VSolid* slabSolid;
  G4double  slabXHalfWidth;
  G4double  slabYHalfWidth;

  G4double containerXRadius;
  G4double containerYRadius;

};
  
#endif
