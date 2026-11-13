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
#include "BDSTunnelFactoryElliptical.hh"
#include "BDSTunnelInfo.hh"

#include "globals.hh"                 // geant4 globals / types
#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4EllipticalTube.hh"
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

class BDSTunnelSection;

BDSTunnelFactoryElliptical::BDSTunnelFactoryElliptical()
{;}

BDSTunnelSection* BDSTunnelFactoryElliptical::CreateTunnelSection(G4String      name,
								  G4double      length,
								  G4double      tunnelThickness,
								  G4double      tunnelSoilThickness,
								  G4Material*   tunnelMaterial,
								  G4Material*   tunnelSoilMaterial,
								  G4bool        tunnelFloor,
								  G4double      tunnelFloorOffset,
								  G4double      tunnel1,
								  G4double      tunnel2,
								  G4bool        visible)
{
  // tidy up things from previous usage if any - base class method
  CleanUp();
  
  // test input parameters - set global options as default if not specified
  TestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial,
		      tunnelSoilMaterial, tunnelFloorOffset, tunnel1, tunnel2);

  // build the solids
  G4VSolid* tunnelOuterSolid = new G4EllipticalTube(name + "_tunnel_outer_solid", // name
						    tunnel1 + tunnelThickness,    // x radius
						    tunnel2 + tunnelThickness,    // y radius
						    0.5*length - lengthSafety);   // z half length (to fit in container)
  
  G4VSolid* tunnelInnerSolid = new G4EllipticalTube(name + "_tunnel_outer_solid", // name
						    tunnel1 + lengthSafety,       // x radius
						    tunnel2 + lengthSafety,       // y radius
						    length); // z half length - long for unambiguous subtraction

  // register solids
  allSolids.insert(tunnelOuterSolid);
  allSolids.insert(tunnelInnerSolid);
  
  tunnelSolid = new G4SubtractionSolid(name + "_tunnel_solid", // name
				       tunnelOuterSolid,      // this
				       tunnelInnerSolid);     // minus this


  G4double soil1R = tunnel1 + tunnelThickness + lengthSafety;
  G4double soil2R = tunnel2 + tunnelThickness + lengthSafety;

  G4double soilOuterRadius = std::max(soil1R, soil2R) + tunnelSoilThickness;
  
  G4VSolid* soilOuterSolid = new G4Tubs(name + "_soil_outer_solid",   // name
					0,                            // inner radius
					soilOuterRadius,              // outer radius
					0.5*length - lengthSafety,    // z half angle
					0,                            // start angle
					CLHEP::twopi);                // sweep angle
  
  G4VSolid* soilInnerSolid = new G4EllipticalTube(name + "_soil_outer_solid",   // name
						  soil1R + lengthSafety,        // x radius
						  soil2R + lengthSafety,        // y radius
						  length); // z half length - long for unambiguous subtraction

  // register solids
  allSolids.insert(soilOuterSolid);
  allSolids.insert(soilInnerSolid);

  soilSolid = new G4SubtractionSolid(name + "_soil_solid", // name
				     soilOuterSolid,      // this
				     soilInnerSolid);     // minus this
  
  G4double containerXRadius = soil1R + tunnelSoilThickness + lengthSafety;
  G4double containerYRadius = soil2R + tunnelSoilThickness + lengthSafety;
  
  // build the floor if necessary
  if (tunnelFloor)
    {
      G4double floorBoxRadius = 1.5 * std::max(tunnel1,tunnel2); // will definitely encompass the tunnel
      G4double floorBoxDisplacement = tunnelFloorOffset + floorBoxRadius + lengthSafety;
      
      G4VSolid* floorEllipse = new G4EllipticalTube(name + "_floor_ellipse_solid", // name
						    tunnel1,                       // x radius
						    tunnel2,                       // y radius
						    length*0.5 - lengthSafety);    // z half length

      G4VSolid* floorBox     = new G4Box(name + "_floor_box_solid",  // name
					 floorBoxRadius,             // x half width
					 floorBoxRadius,             // y half width
					 length);                    // z half length
      // box z length extra long for unambiguous subtraction

      // register solids
      allSolids.insert(floorEllipse);
      allSolids.insert(floorBox);
      
      floorSolid = new G4IntersectionSolid(name + "_floor_solid",                     // name
					   floorEllipse,                              // this
					   floorBox,                                  // minus this,
					   0,                                         // rotation matrix
					   G4ThreeVector(0,-floorBoxDisplacement,0)); // translation

      // need to create a container for the tunnel + floor that only just contains it
      // need to do the same trick again - prepare small floor container segment and union
      // it with tunnel container. Container has to be a wee bit bigger so can't use the same
      // solids sadly.

      // floor container cylinder has to definitely overlaps with outside cylinder -> tunnel1*1.1
      G4VSolid* floorContainerEllipse = new G4EllipticalTube(name + "_floor_cont_ell_solid", // name
							     tunnel1*1.1,                    // x radius
							     tunnel2*1.1,                    // y radius
							     length*0.5);                    // z half length
      
      // z long for unambiguous intersection

      // calculate box container offset - should be just above floor by lengthsafety (floor actually lowered
      // by length safety a la rest of geometry to fit within its dimensions)
      G4double floorBoxContDisp = floorBoxDisplacement - lengthSafety;
      G4VSolid* floorContainerSolid = new G4IntersectionSolid(name + "_floor_cont_solid",           // name
							      floorContainerEllipse,                // this
							      floorBox,                             // minus this,
							      0,                                    // rotation matrix
							      G4ThreeVector(0,-floorBoxContDisp,0));// translation

      G4VSolid* tunnelContainerSolidOuter = new G4Tubs(name + "_tunnel_cont_solid_outer",     // name
						       0,                                     // inner radius
						       soilOuterRadius + lengthSafety,        // outer radius
						       0.5*length,                            // z half angle
						       0,                                     // start angle
						       CLHEP::twopi);                         // sweep angle

      G4VSolid* tunnelContainerSolidInner = new G4EllipticalTube(name + "_tunnel_cont_solid_inner", // name
								 tunnel1,                           // x radius
								 tunnel2,                           // y radius
								 length*0.5);

      G4VSolid* tunnelContainerSolid = new G4SubtractionSolid(name + "_tunnel_cont_solid", // name
							      tunnelContainerSolidOuter,   // this
							      tunnelContainerSolidInner);  // minus this

      // register solids
      allSolids.insert(floorContainerEllipse);
      allSolids.insert(floorContainerSolid);
      allSolids.insert(tunnelContainerSolidOuter);
      allSolids.insert(tunnelContainerSolidInner);
      allSolids.insert(tunnelContainerSolid);

      containerSolid = new G4UnionSolid(name + "_container_solid", // name
					tunnelContainerSolid,      // this
					floorContainerSolid);      // plus this
    }
  else
    {
      // have to do a subtraction with an elliptical tube
      G4VSolid* tunnelContainerSolidOuter = new G4Tubs(name + "_tunnel_cont_solid_outer",     // name
						       0,                                     // inner radius
						       soilOuterRadius + lengthSafety,        // outer radius
						       0.5*length,                            // z half angle
						       0,                                     // start angle
						       CLHEP::twopi);                         // sweep angle
      
      G4VSolid* tunnelContainerSolidInner = new G4EllipticalTube(name + "_tunnel_cont_solid_inner", // name
								 tunnel1,                           // x radius
								 tunnel2,                           // y radius
								 length*0.5);

      // register solids
      allSolids.insert(tunnelContainerSolidOuter);
      allSolids.insert(tunnelContainerSolidInner);

      containerSolid = new G4SubtractionSolid(name + "_tunnel_cont_solid", // name
					      tunnelContainerSolidOuter,   // this
					      tunnelContainerSolidInner);  // minus this
    }
  
  CommonConstruction(name, tunnelMaterial, tunnelSoilMaterial, length, containerXRadius, containerYRadius, visible);
  
  return tunnelSection; // member variable geometry component that's assembled in base class
}


BDSTunnelSection* BDSTunnelFactoryElliptical::CreateTunnelSectionAngled(G4String      name,
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
									G4bool        visible)
{
  // tidy up things from previous usage if any - base class method
  CleanUp();
  
  // test input parameters - set global options as default if not specified
  TestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial,
		      tunnelSoilMaterial, tunnelFloorOffset, tunnel1, tunnel2);
  
  // build the solids
  // create an intersection cut tubs to get the faces - make it bigger than everything else
  // then make elliptical solids longer than they need to be
  G4double intersectionRadius = ( std::max(tunnel1,tunnel2) + tunnelThickness + tunnelSoilThickness ) * 3;
  G4VSolid* faceSolid = new G4CutTubs(name + "_face_intersection_solid", // name
				      0,                                 // inner radius
				      intersectionRadius,                // outer radius
				      length*0.5 - lengthSafety,         // z half length
				      0,                                 // start angle
				      CLHEP::twopi,                      // sweep angle
				      inputFace,                         // input face normal vector
				      outputFace);                       // output face normal vector


  // tunnel
  G4VSolid* tunnelOuterSolid = new G4EllipticalTube(name + "_tunnel_outer_solid", // name
						    tunnel1 + tunnelThickness,    // x radius
						    tunnel2 + tunnelThickness,    // y radius
						    length);                      // z half length
  
  G4VSolid* tunnelInnerSolid = new G4EllipticalTube(name + "_tunnel_outer_solid", // name
						    tunnel1 + lengthSafety,       // x radius
						    tunnel2 + lengthSafety,       // y radius
						    length*1.5); // z half length - long for unambiguous subtraction

  G4VSolid* tunnelSolidUnAngled = new G4SubtractionSolid(name + "_tunnel_square_solid", // name
							 tunnelOuterSolid,      // this
							 tunnelInnerSolid);     // minus this

  // register solids
  allSolids.insert(faceSolid);
  allSolids.insert(tunnelOuterSolid);
  allSolids.insert(tunnelInnerSolid);
  allSolids.insert(tunnelSolidUnAngled);
  
  // cut off the faces with the angled face solid
  tunnelSolid = new G4IntersectionSolid(name + "_tunnel_solid", // name
					tunnelSolidUnAngled,
					faceSolid);

  // soil solid
  G4double soil1R = tunnel1 + tunnelThickness + lengthSafety;
  G4double soil2R = tunnel2 + tunnelThickness + lengthSafety;

  G4double soilOuterRadius = std::max(soil1R, soil2R) + tunnelSoilThickness;

  G4VSolid* soilOuterSolid = new G4Tubs(name + "_soil_outer_solid",   // name
					0,                            // inner radius
					soilOuterRadius,              // outer radius
					length,                       // z half angle
					0,                            // start angle
					CLHEP::twopi);                // sweep angle
  
  G4VSolid* soilInnerSolid = new G4EllipticalTube(name + "_soil_outer_solid",   // name
						  soil1R + lengthSafety,        // x radius
						  soil2R + lengthSafety,        // y radius
						  1.5*length); // z half length - long for unambiguous subtraction

  G4VSolid* soilSolidUnAngled = new G4SubtractionSolid(name + "_soil_square_solid", // name
						       soilOuterSolid,      // this
						       soilInnerSolid);     // minus this

  // register solids
  allSolids.insert(soilOuterSolid);
  allSolids.insert(soilInnerSolid);
  allSolids.insert(soilSolidUnAngled);
  
  soilSolid = new G4IntersectionSolid(name + "_soil_soild", // name
				      soilSolidUnAngled,
				      faceSolid);

  G4double containerXRadius = soil1R + tunnelSoilThickness + lengthSafety;
  G4double containerYRadius = soil2R + tunnelSoilThickness + lengthSafety;
  
  // build the floor if necessary
  if (tunnelFloor)
    {
      // these three lines are a repeat of the same part in the first function (~L211)
      G4double floorBoxRadius = 1.5 * std::max(tunnel1,tunnel2); // will definitely encompass the tunnel
      G4double floorBoxDisplacement = tunnelFloorOffset + floorBoxRadius + lengthSafety;

      G4VSolid* floorEllipse = new G4EllipticalTube(name + "_floor_ellipse_solid", // name
						    tunnel1,                       // x radius
						    tunnel2,                       // y radius
						    length);                       // z half length
      // z long for unamibiguous intersection

      // with the floor do the intersection first to avoid complications with the displacement
      // of the subtraction box later
      G4VSolid* floorEllipseAngled = new G4IntersectionSolid(name + "_floor_ell_angled_solid", // name
							     floorEllipse,
							     faceSolid);

      G4VSolid* floorBox     = new G4Box(name + "_floor_box_solid",  // name
					 floorBoxRadius,             // x half width
					 floorBoxRadius,             // y half width
					 length*1.2);                // z half length
      // box z length extra long for unambiguous subtraction

      // register solids
      allSolids.insert(floorEllipse);
      allSolids.insert(floorBox);

      floorSolid = new G4IntersectionSolid(name + "_floor_solid",                     // name
					   floorEllipseAngled,                        // this
					   floorBox,                                  // minus this,
					   0,                                         // rotation matrix
					   G4ThreeVector(0,-floorBoxDisplacement,0)); // translation

      // floor container cylinder has to definitely overlaps with outside cylinder -> tunnel1*1.1
      G4VSolid* floorContainerEllAng = new G4EllipticalTube(name + "_floor_cont_ell_solid", // name
							    tunnel1*1.1,                    // x radius
							    tunnel2*1.1,                    // y radius
							    length*1.3);                    // z half length
      // z long for unambiguous intersection

      G4VSolid* floorContainerEllipseAngled = new G4IntersectionSolid(name + "+floor_cont_ell_ang_solid", // name
								      floorContainerEllAng,
								      faceSolid);

      // calculate box container offset - should be just above floor by lengthsafety (floor actually lowered
      // by length safety a la rest of geometry to fit within its dimensions)
      G4double floorBoxContDisp = floorBoxDisplacement - lengthSafety;
      G4VSolid* floorContainerSolid = new G4IntersectionSolid(name + "_floor_cont_solid",           // name
							      floorContainerEllipseAngled,          // this
							      floorBox,                             // minus this,
							      0,                                    // rotation matrix
							      G4ThreeVector(0,-floorBoxContDisp,0));// translation

      G4VSolid* tunnelContainerOuterSquare = new G4Tubs(name + "_tunnel_cont_solid_outer",     // name
							0,                                     // inner radius
							soilOuterRadius + lengthSafety,        // outer radius
							length,                                // z half angle
							0,                                     // start angle
							CLHEP::twopi);                         // sweep angle

      G4VSolid* tunnelContainerSolidOuterAngled = new G4IntersectionSolid(name + "_tunnel_cont_out_ang_solid", // name
									  tunnelContainerOuterSquare,
									  faceSolid);

      G4VSolid* tunnelContainerSolidInner = new G4EllipticalTube(name + "_tunnel_cont_solid_inner", // name
								 tunnel1,                           // x radius
								 tunnel2,                           // y radius
								 length*2.4);

      G4VSolid* tunnelContainerSolid = new G4SubtractionSolid(name + "_tunnel_cont_solid",       // name
							      tunnelContainerSolidOuterAngled,   // this
							      tunnelContainerSolidInner);        // minus this

      // register solids
      allSolids.insert(floorContainerEllAng);
      allSolids.insert(floorContainerEllipseAngled);
      allSolids.insert(floorContainerSolid);
      allSolids.insert(tunnelContainerOuterSquare);
      allSolids.insert(tunnelContainerSolidOuterAngled);
      allSolids.insert(tunnelContainerSolidInner);
      allSolids.insert(tunnelContainerSolid);

      containerSolid = new G4UnionSolid(name + "_container_solid", // name
					tunnelContainerSolid,      // this
					floorContainerSolid);      // plus this
      
    }
  else
    {
      G4VSolid* tunnelContainerOuterSquare = new G4Tubs(name + "_tunnel_cont_solid_outer",     // name
							0,                                     // inner radius
							soilOuterRadius + lengthSafety,        // outer radius
							length,                                // z half angle
							0,                                     // start angle
							CLHEP::twopi);                         // sweep angle

      G4VSolid* tunnelContainerSolidOuterAngled = new G4IntersectionSolid(name + "_tunnel_cont_out_ang_solid", // name
									  tunnelContainerOuterSquare,
									  faceSolid);

      G4VSolid* tunnelContainerSolidInner = new G4EllipticalTube(name + "_tunnel_cont_solid_inner", // name
								 tunnel1,                           // x radius
								 tunnel2,                           // y radius
								 length*0.5);

      // register solids
      allSolids.insert(tunnelContainerOuterSquare);
      allSolids.insert(tunnelContainerSolidOuterAngled);
      allSolids.insert(tunnelContainerSolidInner);
      
      containerSolid = new G4SubtractionSolid(name + "_tunnel_cont_solid",       // name
					      tunnelContainerSolidOuterAngled,   // this
					      tunnelContainerSolidInner);        // minus this
    }
  
  CommonConstruction(name, tunnelMaterial, tunnelSoilMaterial, length, containerXRadius,
		     containerYRadius, visible);

  return tunnelSection;
}

/// functions below here are private to this particular factory
void BDSTunnelFactoryElliptical::TestInputParameters(G4double&    length,
						     G4double&    tunnelThickness,
						     G4double&    tunnelSoilThickness,
						     G4Material*& tunnelMaterial,
						     G4Material*& tunnelSoilMaterial,
						     G4double&    tunnelFloorOffset,
						     G4double&    tunnel1,
						     G4double&    tunnel2)
{
  CommontTestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial, tunnelSoilMaterial);
  
  if (tunnelFloorOffset < 1e-10)
    {tunnelFloorOffset = defaultModel->floorOffset;}
  
  if (tunnel1 < 1e-10)
    {tunnel1 = defaultModel->aper1;}

  if (tunnel2 < 1e-10)
    {tunnel2 = defaultModel->aper2;}
}
