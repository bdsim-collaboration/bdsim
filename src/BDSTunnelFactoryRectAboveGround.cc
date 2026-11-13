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
#include "BDSTunnelFactoryRectAboveGround.hh"
#include "BDSTunnelInfo.hh"
#include "BDSUtilities.hh"

#include "globals.hh"                 // geant4 globals / types
#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4UnionSolid.hh"
#include "G4VSolid.hh"

#include <cmath>                           // sin, cos, fabs
#include <set>
#include <utility>                         // for std::pair

class BDSTunnelSection;

BDSTunnelFactoryRectAboveGround::BDSTunnelFactoryRectAboveGround()
{
  slabSolid        = nullptr;
  slabXHalfWidth   = 0;
  slabYHalfWidth   = 1*CLHEP::m;
  containerXRadius = 0;
  containerYRadius = 0;
}

void BDSTunnelFactoryRectAboveGround::CleanUp()
{
  BDSTunnelFactoryBase::CleanUp();
  containerXRadius = 0;
  containerYRadius = 0;
  slabXHalfWidth   = 0;
}

BDSTunnelSection* BDSTunnelFactoryRectAboveGround::CreateTunnelSection(G4String    name,
								       G4double    length,
								       G4double    tunnelThickness,
								       G4double    tunnelSoilThickness,
								       G4Material* tunnelMaterial,
								       G4Material* tunnelSoilMaterial,
								       G4bool      /*tunnelFloor*/,
								       G4double    /*tunnelFloorOffset*/,
								       G4double    tunnel1,
								       G4double    tunnel2,
								       G4bool      visible)
{
  // tidy up things from previous usage if any - base class method
  CleanUp();
  
  // test input parameters - set global options as default if not specified
  TestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial,
		      tunnelSoilMaterial, tunnel1, tunnel2);

  // build the solids
  // work out how wide the slab needs to be
  slabXHalfWidth = std::max(tunnel1, tunnel2) * 2;

  // lengthSafety ensures clean union with floor plane
  G4double yDisp = tunnel2 + slabYHalfWidth + lengthSafety;
  
  G4ThreeVector slabDisplacement = G4ThreeVector(0,-yDisp,0);
  
  slabSolid = new G4Box(name + "_slab_solid",       // name
			slabXHalfWidth,             // x half width
			slabYHalfWidth,             // y half width
			0.5*length - lengthSafety); // z half width		
  
  G4VSolid* tunnelOuterSolid = new G4Box(name + "_tunnel_outer_solid", // name
					 tunnel1 + tunnelThickness,    // x radius
					 tunnel2 + tunnelThickness,    // y radius
					 0.5*length - lengthSafety);   // z half length (to fit in container)

  G4VSolid* tunnelRectSolid = new G4UnionSolid(name + "_tunnel_outer_solid", // name,
					       tunnelOuterSolid,
					       slabSolid,
					       0,
					       slabDisplacement);
  
  G4VSolid* tunnelInnerSolid = new G4Box(name + "_tunnel_outer_solid", // name
					 tunnel1 + lengthSafety,       // x radius
					 tunnel2 + lengthSafety,       // y radius
					 length); // z half length - long for unambiguous subtraction

  tunnelSolid = new G4SubtractionSolid(name + "_tunnel_solid",// name
				       tunnelRectSolid,           // this
				       tunnelInnerSolid);          // minus this

  // register solids
  allSolids.insert(tunnelOuterSolid);
  allSolids.insert(tunnelRectSolid);
  allSolids.insert(tunnelInnerSolid);
  allSolids.insert(tunnelRectSolid);

  containerSolid = BuildContainerStraight(name, length, tunnel1, tunnel2,
					  tunnelThickness, slabDisplacement);

  CommonConstruction(name, tunnelMaterial, tunnelSoilMaterial, length, containerXRadius, containerYRadius, visible);
  
  return tunnelSection;
}

BDSTunnelSection* BDSTunnelFactoryRectAboveGround::CreateTunnelSectionAngled(G4String      name,
									     G4double      length,
									     G4ThreeVector inputFace,
									     G4ThreeVector outputFace,
									     G4double      tunnelThickness,
									     G4double      tunnelSoilThickness,
									     G4Material*   tunnelMaterial,
									     G4Material*   tunnelSoilMaterial,
									     G4bool        /*tunnelFloor*/,
									     G4double      /*tunnelFloorOffset*/,
									     G4double      tunnel1,
									     G4double      tunnel2,
									     G4bool        visible)
{
  // tidy up things from previous usage if any - base class method
  CleanUp();
  
  // test input parameters - set global options as default if not specified
  TestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial,
		      tunnelSoilMaterial, tunnel1, tunnel2);
  
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

  // build the solids
  // work out how wide the slab needs to be
  slabXHalfWidth = std::max(tunnel1, tunnel2) * 2;

  // lengthSafety ensures clean union with floor plane
  G4double yDisp = tunnel2 + slabYHalfWidth + lengthSafety;
  
  G4ThreeVector slabDisplacement = G4ThreeVector(0,-yDisp,0);
  
  slabSolid = new G4Box(name + "_slab_solid",       // name
			slabXHalfWidth,             // x half width
			slabYHalfWidth,             // y half width
			1.2*length - lengthSafety); // z half width (long for uabig sub.)

  
  // tunnel
  G4VSolid* tunnelOuterSolid = new G4Box(name + "_tunnel_outer_solid", // name
					 tunnel1 + tunnelThickness,    // x radius
					 tunnel2 + tunnelThickness,    // y radius
					 length);                      // z half length

  G4VSolid* tunnelRectSolid = new G4UnionSolid(name + "_tunnel_outer_solid", // name,
					       tunnelOuterSolid,
					       slabSolid,
					       0,
					       slabDisplacement);
  
  G4VSolid* tunnelInnerSolid = new G4Box(name + "_tunnel_outer_solid", // name
					 tunnel1 + lengthSafety,       // x radius
					 tunnel2 + lengthSafety,       // y radius
					 length*1.5); // z half length - long for unambiguous subtraction

  G4VSolid* tunnelSolidUnAngled = new G4SubtractionSolid(name + "_tunnel_square_solid", // name
							 tunnelRectSolid,               // this
							 tunnelInnerSolid);             // minus this

  // register solids
  allSolids.insert(tunnelOuterSolid);
  allSolids.insert(tunnelRectSolid);
  allSolids.insert(tunnelInnerSolid);
  allSolids.insert(tunnelSolidUnAngled);

  // cut off the faces with the angled face solid
  tunnelSolid = new G4IntersectionSolid(name + "_tunnel_solid", // name
					tunnelSolidUnAngled,
					faceSolid);

  G4VSolid* straightContainer = BuildContainerStraight(name, length*2.0, tunnel1, tunnel2,
						       tunnelThickness, slabDisplacement);

  containerSolid = new G4IntersectionSolid(name + "_tunnel_container_solid", // name
					   straightContainer,
					   faceSolid);
  
  CommonConstruction(name, tunnelMaterial, tunnelSoilMaterial, length, containerXRadius,
		     containerYRadius, visible);
  
  return tunnelSection;
}

/// functions below here are private to this particular factory
void BDSTunnelFactoryRectAboveGround::TestInputParameters(G4double&    length,
							  G4double&    tunnelThickness,
							  G4double&    tunnelSoilThickness,
							  G4Material*& tunnelMaterial,
							  G4Material*& tunnelSoilMaterial,
							  G4double&    tunnel1,
							  G4double&    tunnel2)
{
  CommontTestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial, tunnelSoilMaterial);
  
  if (!BDS::IsFinite(tunnel1))
    {tunnel1 = defaultModel->aper1;}

  if (!BDS::IsFinite(tunnel2))
    {tunnel2 = defaultModel->aper2;}
}

G4VSolid* BDSTunnelFactoryRectAboveGround::BuildContainerStraight(G4String name,
								  G4double lengthIn,
								  G4double tunnel1,
								  G4double tunnel2,
								  G4double tunnelThickness,
								  G4ThreeVector slabDisplacement)
{
  G4VSolid* containerSolidL; // local
  
  containerXRadius = slabXHalfWidth + lengthSafety;
  containerYRadius = tunnel2 + tunnelThickness + lengthSafety;
  
  // have to do a subtraction
  G4VSolid* tunnelContainerOuterSlab = new G4Box(name + "_cont_slab_solid",     // name
						 slabXHalfWidth + lengthSafety, // x half width
						 slabYHalfWidth + lengthSafety, // y half width
						 0.5*lengthIn);                   // z half width
  G4VSolid* tunnelContainerOuterTunnel = new G4Box(name + "_cont_tunnel_solid",              // name
						   tunnel1 + tunnelThickness + lengthSafety, // x radius
						   tunnel2 + tunnelThickness + lengthSafety, // y radius
						   0.5*lengthIn);                              // z half length
  G4VSolid* tunnelContainerOuter = new G4UnionSolid(name + "_cont_outer_solid",
						    tunnelContainerOuterTunnel,
						    tunnelContainerOuterSlab,
						    0,
						    slabDisplacement);
  
  G4VSolid* tunnelContainerInner = new G4Box(name + "_tunnel_cont_solid_inner", // name
					     tunnel1,                           // x radius
					     tunnel2,                           // y radius
					     lengthIn);
  
  // regsiter solids
  allSolids.insert(tunnelContainerOuterSlab);
  allSolids.insert(tunnelContainerOuterTunnel);
  allSolids.insert(tunnelContainerOuter);
  allSolids.insert(tunnelContainerInner);
  
  containerSolidL = new G4SubtractionSolid(name + "_tunnel_cont_solid", // name
					   tunnelContainerOuter,        // this
					   tunnelContainerInner);       // minus this
  
  return containerSolidL;
}
