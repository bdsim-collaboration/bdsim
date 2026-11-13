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
#ifndef BDSTUNNELFACTORYBASE_H
#define BDSTUNNELFACTORYBASE_H

#include "BDSFactoryBase.hh"
#include "BDSTunnelSection.hh"

#include "globals.hh"  // geant4 globals / types
#include "G4ThreeVector.hh"

#include <set>

class G4LogicalVolume;
class G4Material;
class G4VisAttributes;
class G4VSolid;

class BDSGeometryComponent;
class BDSTunnelInfo;

/**
 * @brief Abstract base class for tunnel factory classes
 * 
 * Abstract base class that defines the interface of each factory
 * to build any type of tunnel required. There are four general
 * possibilities apart from the shape (each derived class will give
 * a different cross-section) and these dictate the angled faces.
 * 
 * Uses Geant4 default units by default, ie mm, rad (not metres).
 *
 * NOTE - no derived classes record the 'angle' parameter for 
 * BDSAcceleratorComponent correctly in the BDSTunnelSection instances.
 * This is only acceptable as the tunnel beam line is manually made
 * not using the internal accumulation of coordinates of BDSBeamline.
 * It's also in part necessary as each tunnel segment doesn't obey
 * the no double bending rule of accerlator components so can't be
 * used with angle and arc length accurately.
 * 
 * @author Laurie Nevay
 */

class BDSTunnelFactoryBase: public BDSFactoryBase
{
public:
  /// Create a tunnel section with flat input and output faces. Pure virtual.
  virtual BDSTunnelSection* CreateTunnelSection(G4String      name,
						G4double      length,
						G4double      tunnelThickness,
						G4double      tunnelSoilThickness,
						G4Material*   tunnelMaterial,
						G4Material*   tunnelSoilMaterial,
						G4bool        tunnelFloor,
						G4double      tunnelFloorOffset,
						G4double      tunnel1,
						G4double      tunnel2,
						G4bool        visible) = 0;

  /// Create a tunnel section with an angled input face and flat output face. Note,
  /// this is implemented in this base class as a dispatch to the AngledInOut function.
  virtual BDSTunnelSection* CreateTunnelSectionAngledIn(G4String      name,
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
							G4bool        visible);

  /// Create a tunnel section with an angled output face and flat input face. Note,
  /// this is implemented in this base class as a dispatch to the AngledInOut function.
  virtual BDSTunnelSection* CreateTunnelSectionAngledOut(G4String      name,
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
							 G4bool        visible);

  /// Calculates the face normal vectors that all construction uses and then passes
  /// along to the also public function CreateTunnelSectionAngled. This allows flexibility
  /// in the way the tunnel segments are created - whether in a fairly flat 2d approach along
  /// the beam line with only bending the horizontal plane (angles) or in 3d (vectors).
  virtual BDSTunnelSection* CreateTunnelSectionAngledInOut(G4String      name,
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
							   G4bool        visible);

  /// Create a tunnel section with an angled input and output face. Pure virtual.
  virtual BDSTunnelSection* CreateTunnelSectionAngled(G4String       name,
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
						      G4bool         visible) = 0;

  /// Virtual base destructor
  virtual ~BDSTunnelFactoryBase() {;}

protected:
  /// protected default constructor so only derived classes can use it
  BDSTunnelFactoryBase();

  /// General basic viability tests for input parameters - these are only basic tests
  /// and not dependent on the accelerator model, other components or specific tunnel model
  /// in the derived factory class
  void CommontTestInputParameters(G4double&    length,
				  G4double&    tunnelThickness,
				  G4double&    tunnelSoilThickness,
				  G4Material*& tunnelMaterial,
				  G4Material*& tunnelSoilMaterial);
  
  virtual void CommonConstruction(G4String    name,
				  G4Material* tunnelMaterial,
				  G4Material* tunnelSoilMaterial,
				  G4double    length,
				  G4double    containerXRadius,
				  G4double    contianerYRadius,
				  G4bool      visible);

  /// Build logical volumes from solids + materials
  virtual void BuildLogicalVolumes(G4String    name,
				   G4Material* tunnelMaterial,
				   G4Material* tunnelSoilMaterial);

  /// Set the visual attributes for each logical volume
  virtual void SetVisAttributes(G4bool visible);
  
  /// Set user limits for all logical volumes in the tunnel section
  virtual void SetUserLimits();
  
  /// Prepare the assembled geometry component
  virtual void PrepareGeometryComponent(G4double containerXRadius,
					G4double containerYRadius,
					G4double containerZRadius);

  /// Prepare the output BDSTunnelSection instance
  virtual void PrepareTunnelSection(G4String name,
				    G4double chordLength);

  /// Set the sensitive volumes
  virtual void SetSensitiveVolumes();
  
  /// Place components in container volume
  virtual void PlaceComponents(G4String name);

  /// Reset factory members for next usage - avoids previously
  /// constructed parts being accidently used in new object
  virtual void CleanUp();
    
  BDSGeometryComponent* tunnelComponent;
  BDSTunnelSection*     tunnelSection;
  
  G4VSolid*        containerSolid;
  G4VSolid*        tunnelSolid;
  G4VSolid*        soilSolid;
  G4VSolid*        floorSolid;
  G4VSolid*        intersectionSolid;
  G4LogicalVolume* containerLV;
  G4LogicalVolume* tunnelLV;
  G4LogicalVolume* soilLV;
  G4LogicalVolume* floorLV;
  G4ThreeVector    floorDisplacement;
  BDSTunnelInfo*   defaultModel;

  /// The total angle by which the coordinates turn by (horizontally)
  /// after going through the tunnel - only really needed to fulfill
  /// BDSAcceleratorComponent inheritance - unsed further downstream.
  G4double         cumulativeAngle;
};

#endif
