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
#include "BDSTunnelFactoryBase.hh"
#include "BDSTunnelFactoryCircular.hh"
#include "BDSTunnelInfo.hh"
#include "BDSTunnelSection.hh"

#include "globals.hh"                 // geant4 globals / types
#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4VSolid.hh"

#include <cmath>                           // sin, cos, fabs
#include <set>
#include <utility>                         // for std::pair

BDSTunnelFactoryCircular::BDSTunnelFactoryCircular()
{;}

BDSTunnelSection* BDSTunnelFactoryCircular::CreateTunnelSection(G4String      name,
								G4double      length,
								G4double      tunnelThickness,
								G4double      tunnelSoilThickness,
								G4Material*   tunnelMaterial,
								G4Material*   tunnelSoilMaterial,
								G4bool        tunnelFloor,
								G4double      tunnelFloorOffset,
								G4double      tunnel1,
								G4double      /*tunnel2*/,
								G4bool        visible)
{
  // tidy up things from previous usage if any - base class method
  CleanUp();
  
  // test input parameters - set global options as default if not specified
  TestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial,
		      tunnelSoilMaterial, tunnelFloorOffset, tunnel1);

  // build the solids
  tunnelSolid = new G4Tubs(name + "_tunnel_solid",    // name
			   tunnel1 + lengthSafety,    // inner radius
			   tunnel1 + tunnelThickness, // outer radius
			   length*0.5 - lengthSafety, // z half length
			   0,                         // start angle
			   CLHEP::twopi);             // sweep angle

  G4double soilInnerR = tunnel1 + tunnelThickness + lengthSafety;
  G4double soilOuterR = soilInnerR + tunnelSoilThickness;
  soilSolid   = new G4Tubs(name + "_soil_solid",      // name
			   soilInnerR,                // inner radius
			   soilOuterR,                // outer radius
			   length*0.5 - lengthSafety, // z half length
			   0,                         // start angle
			   CLHEP::twopi);             // sweep angle 

  G4double containerRadius = soilOuterR + lengthSafety;
  
  // build the floor if necessary
  if (tunnelFloor)
    {
      G4double floorBoxRadius = 1.5*tunnel1; // will definitely encompass the tunnel
      G4double floorCylinderRadius = tunnel1 - lengthSafety; // so it fits within the tunnel
      G4double floorBoxDisplacement = tunnelFloorOffset + floorBoxRadius + lengthSafety;
      
      G4VSolid* floorCylinder = new G4Tubs(name + "_floor_cylinder_solid", // name
					   0,                              // inner radius
					   floorCylinderRadius,            // outer radius
					   length*0.5 - lengthSafety,      // z half length
					   0,                              // start angle
					   CLHEP::twopi);                  // sweep angle

      G4VSolid* floorBox      = new G4Box(name + "_floor_box_solid",  // name
					  floorBoxRadius,             // x half width
					  floorBoxRadius,             // y half width
					  length);                    // z half length
      // box z length extra long for unambiguous subtraction
      
      floorSolid = new G4IntersectionSolid(name + "_floor_solid",                     // name
					   floorCylinder,                             // this
					   floorBox,                                  // minus this,
					   0,                                         // rotation matrix
					   G4ThreeVector(0,-floorBoxDisplacement,0)); // translation

      // need to create a container for the tunnel + floor that only just contains it
      // need to do the same trick again - prepare small floor container segment and union
      // it with tunnel container. Container has to be a wee bit bigger so can't use the same
      // solids sadly.

      // floor container cylinder has to definitely overlaps with outside cylinder -> tunnel1*1.1
      G4VSolid* floorContainerCylinder = new G4Tubs(name + "_floor_cont_cyl_solid", // name
						    0,                              // inner radius
						    tunnel1*1.1,                    // outer radius
						    length*0.5,                     // z half length
						    0,                              // start angle
						    CLHEP::twopi);                  // sweep angle

      G4VSolid* floorContainerBox = new G4Box(name + "_floor_cont_box_solid",  // name
					      floorBoxRadius,                  // x half width
					      floorBoxRadius,                  // y half width
					      length);                         // z half length
      // z long for unambiguous intersection

      // calculate box container offset - should be just above floor by lengthsafety (floor actually lowered
      // by length safety a la rest of geometry to fit within its dimensions)
      G4double floorBoxContDisp = floorBoxDisplacement - lengthSafety;
      G4VSolid* floorContainerSolid = new G4IntersectionSolid(name + "_floor_cont_solid",           // name
							      floorContainerCylinder,               // this
							      floorContainerBox,                    // minus this,
							      0,                                    // rotation matrix
							      G4ThreeVector(0,-floorBoxContDisp,0));// translation

      G4VSolid* tunnelContainerSolid = new G4Tubs(name + "_tunnel_solid",    // name
						  tunnel1,                   // inner radius
						  containerRadius,           // outer radius
						  length*0.5,                // z half length
						  0,                         // start angle
						  CLHEP::twopi);             // sweep angle

      containerSolid = new G4UnionSolid(name + "_container_solid", // name
					tunnelContainerSolid,      // this
					floorContainerSolid);      // plus this
    }
  else
    {
      containerSolid = new G4Tubs(name + "_tunnel_container_solid", // name
				  tunnel1,                          // inner radius
				  containerRadius,                  // outer radius,
				  0.5*length,                       // z half length
				  0,                                // start angle
				  CLHEP::twopi);                    // sweep angle
    }
  
  CommonConstruction(name, tunnelMaterial, tunnelSoilMaterial, length, containerRadius, containerRadius, visible);

  return tunnelSection;
}


BDSTunnelSection* BDSTunnelFactoryCircular::CreateTunnelSectionAngled(G4String      name,
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
								      G4double      /*tunnel2*/,
								      G4bool        visible)
{
  // tidy up things from previous usage if any - base class method
  CleanUp();
  
  // test input parameters - set global options as default if not specified
  TestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial,
		      tunnelSoilMaterial, tunnelFloorOffset, tunnel1);
  
  // build the solids
  tunnelSolid = new G4CutTubs(name + "_tunnel_solid",    // name
			      tunnel1,                   // inner radius
			      tunnel1 + tunnelThickness, // outer radius
			      length*0.5 - lengthSafety, // z half length
			      0,                         // start angle
			      CLHEP::twopi,              // sweep angle
			      inputFace,                 // input face normal vector
			      outputFace);               // output face normal vector

  G4double soilInnerR = tunnel1 + tunnelThickness + lengthSafety;
  G4double soilOuterR = soilInnerR + tunnelSoilThickness;
  soilSolid   = new G4CutTubs(name + "_soil_solid",      // name
			      soilInnerR,                // inner radius
			      soilOuterR,                // outer radius
			      length*0.5 - lengthSafety, // z half length
			      0,                         // start angle
			      CLHEP::twopi,              // sweep angle 
			      inputFace,                 // input face normal vector
			      outputFace);               // output face normal vector

  G4double containerRadius = soilOuterR + lengthSafety;
  
  // build the floor if necessary
  if (tunnelFloor)
    {
      // these three lines are a repeat of the same part in the first function (~L211)
      G4double floorBoxRadius = 1.5*tunnel1; // will definitely encompass the tunnel
      G4double floorCylinderRadius = tunnel1 - lengthSafety; // so it fits within the tunnel
      G4double floorBoxDisplacement = tunnelFloorOffset + floorBoxRadius + lengthSafety;
      
      G4VSolid* floorCylinder = new G4CutTubs(name + "_floor_cylinder_solid", // name
					      0,                              // inner radius
					      floorCylinderRadius,            // outer radius
					      length*0.5 - lengthSafety,      // z half length
					      0,                              // start angle
					      CLHEP::twopi,                   // sweep angle
					      inputFace,                      // input face normal vector
					      outputFace);                    // output face normal vector

      G4VSolid* floorBox      = new G4Box(name + "_floor_box_solid",  // name
					  floorBoxRadius,             // x half width
					  floorBoxRadius,             // y half width
					  length);                    // z half length
      // z long for unambiguous intersection

      // register solids
      allSolids.insert(floorCylinder);
      allSolids.insert(floorBox);
      
      floorSolid = new G4IntersectionSolid(name + "_floor_solid",                      // name
					   floorCylinder,                              // this
					   floorBox,                                   // minus this,
					   0,                                          // rotation matrix
					   G4ThreeVector(0,-floorBoxDisplacement,0));  // translation
      
      // floor container cylinder has to definitely overlaps with outside cylinder -> tunnel1*1.1
      G4VSolid* floorContainerCylinder = new G4CutTubs(name + "_floor_cont_cyl_solid", // name
						       0,                              // inner radius
						       tunnel1*1.1,                    // outer radius
						       length*0.5,                     // z half length
						       0,                              // start angle
						       CLHEP::twopi,                   // sweep angle
						       inputFace,                      // input face normal
						       outputFace);                    // output face normal

      G4VSolid* floorContainerBox = new G4Box(name + "_floor_cont_box_solid",  // name
					      1.5*tunnel1,                     // x half width
					      1.5*tunnel1,                     // y half width
					      length);                         // z half length
      // floor container box z long for unambiguous intersection

      // register solids
      allSolids.insert(floorContainerCylinder);
      allSolids.insert(floorContainerBox);

      // calculate box container offset - should be just above floor by lengthsafety (floor actually lowered
      // by length safety a la rest of geometry to fit within its dimensions)
      G4double floorBoxContDisp = floorBoxDisplacement - lengthSafety;
      G4VSolid* floorContainerSolid = new G4IntersectionSolid(name + "_floor_cont_solid",           // name
							      floorContainerCylinder,               // this
							      floorContainerBox,                    // minus this,
							      0,                                    // rotation matrix
							      G4ThreeVector(0,-floorBoxContDisp,0));// translation

      G4VSolid* tunnelContainerSolid = new G4CutTubs(name + "_tunnel_solid",    // name
						     tunnel1 - lengthSafety,    // inner radius
						     containerRadius,           // outer radius
						     length*0.5,                // z half length
						     0,                         // start angle
						     CLHEP::twopi,              // sweep angle
						     inputFace,                 // input face normal vector
						     outputFace);               // output face normal vector

      // register solids
      allSolids.insert(floorContainerSolid);
      allSolids.insert(tunnelContainerSolid);

      containerSolid = new G4UnionSolid(name + "_container_solid", // name
					tunnelContainerSolid,      // this
					floorContainerSolid);      // plus this
      
      G4VSolid* intersectionSolidCylinder = new G4CutTubs(name + "_int_cyl_solid",  // name
							  0,                        // inner radius
							  tunnel1 - 2*lengthSafety, // outer radius
							  length*0.5,               // z half length
							  0,                        // start angle
							  CLHEP::twopi,             // sweep angle
							  inputFace,                // input face normal vector
							  outputFace);              // output face normal vector
      allSolids.insert(intersectionSolidCylinder);

      intersectionSolid = new G4SubtractionSolid(name + "_intersection_solid", // name
						 intersectionSolidCylinder,    // this
						 floorContainerSolid,          // minus this,
						 0,                            // rotation
						 G4ThreeVector(0,-floorBoxContDisp+lengthSafety,0)); // translation
    }
  else
    {
      containerSolid = new G4CutTubs(name + "_tunnel_container_solid", // name
				     tunnel1,                          // inner radius
				     containerRadius,                  // outer radius,
				     length*0.5,                       // z half length
				     0,                                // start angle
				     CLHEP::twopi,                     // sweep angle
				     inputFace,                        // input face normal vector
				     outputFace);                      // output face normal vector

      intersectionSolid = new G4CutTubs(name + "_tunnel_intersection_solid", // name
					0,                                   // inner radius
					tunnel1 - 2*lengthSafety,            // outer radius
					length*0.5,                          // z half length
					0,                                   // start angle
					CLHEP::twopi,                        // sweep angle
					inputFace,                           // input face normal vector
					outputFace);                         // output face normal vector
    }

  CommonConstruction(name, tunnelMaterial, tunnelSoilMaterial, length, containerRadius,
		     containerRadius, visible);

  return tunnelSection;
}

/// functions below here are private to this particular factory
void BDSTunnelFactoryCircular::TestInputParameters(G4double&    length,
						   G4double&    tunnelThickness,
						   G4double&    tunnelSoilThickness,
						   G4Material*& tunnelMaterial,
						   G4Material*& tunnelSoilMaterial,
						   G4double&    tunnelFloorOffset,
						   G4double&    tunnel1)
{
  CommontTestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial, tunnelSoilMaterial);
  
  if (tunnelFloorOffset < 1e-10)
    {tunnelFloorOffset = defaultModel->floorOffset;}
  
  if (tunnel1 < 1e-10)
    {tunnel1 = defaultModel->aper1;}
}
