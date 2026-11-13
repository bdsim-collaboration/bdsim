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
#ifndef BDSTUNNELFACTORY_H
#define BDSTUNNELFACTORY_H

#include "globals.hh"           // geant4 globals / types

#include "BDSTunnelType.hh"

class BDSTunnelFactoryBase;
class BDSTunnelSection;

class G4Material;

/**
 * @brief A singleton class that provides an interface to all tunnel factories.
 *
 *  Each function of a BDSTunnelFactoryBase derived
 * class is implemented here, but with the wrapper for BDSTunnelType
 * which is used in this class to utilise the appropriate factory. Note,
 * this class does not own the other factories or keep references to them - 
 * they are accessed through (global) singleton accessors.
 * 
 * @author Laurie Nevay
 */

class BDSTunnelFactory
{
public:
  static BDSTunnelFactory* Instance(); ///< singleton pattern
  
  ~BDSTunnelFactory();

  /// Create a tunnel section with straight input and output face.
  BDSTunnelSection* CreateTunnelSection(BDSTunnelType tunnelType,
					G4String      name,
					G4double      length,
					G4double      tunnelThickness,
					G4double      tunnelSoilThickness,
					G4Material*   tunnelMaterial,
					G4Material*   tunnelSoilMaterial,
					G4bool        tunnelFloor,
					G4double      tunnelFloorOffset,
					G4double      tunnel1,
					G4double      tunnel2,
					G4bool        visible = true);

  /// Create a tunnel section with an angled input face and flat output face. Note,
  /// this is implemented in this base class as a dispatch to the AngledInOut function.
  BDSTunnelSection* CreateTunnelSectionAngledIn(BDSTunnelType tunnelType,
						G4String      name,
						G4double      length,
						G4double      angleIn,
						G4double      tunnelThickness,
						G4double      tunnelSoilThickness,
						G4Material*   tunnelMaterial,
						G4Material*   tunnelSoilMaterial,
						G4bool        tunnelFloor,
						G4double      tunnelFloorOffset,
						G4double      tunnel1,
						G4double      tunnel2,
						G4bool        visible = true);

  /// Create a tunnel section with an angled output face and flat input face. Note,
  /// this is implemented in this base class as a dispatch to the AngledInOut function.
  BDSTunnelSection* CreateTunnelSectionAngledOut(BDSTunnelType tunnelType,
						 G4String      name,
						 G4double      length,
						 G4double      angleOut,
						 G4double      tunnelThickness,
						 G4double      tunnelSoilThickness,
						 G4Material*   tunnelMaterial,
						 G4Material*   tunnelSoilMaterial,
						 G4bool        tunnelFloor,
						 G4double      tunnelFloorOffset,
						 G4double      tunnel1,
						 G4double      tunnel2,
						 G4bool        visible = true);

  /// Create a tunnel section with an angled input and output face.
  BDSTunnelSection* CreateTunnelSectionAngledInOut(BDSTunnelType tunnelType,
						   G4String      name,
						   G4double      length,
						   G4double      angleIn,
						   G4double      angleOut,
						   G4double      tunnelThickness,
						   G4double      tunnelSoilThickness,
						   G4Material*   tunnelMaterial,
						   G4Material*   tunnelSoilMaterial,
						   G4bool        tunnelFloor,
						   G4double      tunnelFloorOffset,
						   G4double      tunnel1,
						   G4double      tunnel2,
						   G4bool        visible = true);

  /// Create a tunnel section with two angled faces as defined by normal vectors for
  /// each face.
  BDSTunnelSection* CreateTunnelSectionAngled(BDSTunnelType  tunnelType,
					      G4String       name,
					      G4double       length,
					      G4ThreeVector  inputFaceIn,
					      G4ThreeVector  outputFaceIn,
					      G4double       tunnelThickness,
					      G4double       tunnelSoilThickness,
					      G4Material*    tunnelMaterial,
					      G4Material*    tunnelSoilMaterial,
					      G4bool         tunnelFloor,
					      G4double       tunnelFloorOffset,
					      G4double       tunnel1,
					      G4double       tunnel2,
					      G4bool         visible = true);
  
private:
  /// Private constructor as a singleton
  BDSTunnelFactory();

  /// Singleton instance
  static BDSTunnelFactory* instance;

  /// Accessor method to dispatch to appropriate derived factory
  BDSTunnelFactoryBase* GetAppropriateFactory(BDSTunnelType tunnelType);

  /// @{ Factory instance.
  BDSTunnelFactoryBase* circular;
  BDSTunnelFactoryBase* elliptical;
  BDSTunnelFactoryBase* rectangular;
  BDSTunnelFactoryBase* square;
  BDSTunnelFactoryBase* rectaboveground;
  /// @}
};



#endif
