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
#include "BDSDebug.hh"
#include "BDSExtent.hh"
#include "BDSMagnetOuterFactoryPolesFacetCommon.hh"

#include "globals.hh"  // geant4 globals / types
#include "G4Polyhedra.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"

#include <set>

BDSMagnetOuterFactoryPolesFacetCommon::BDSMagnetOuterFactoryPolesFacetCommon(G4double factorIn):
  BDSMagnetOuterFactoryPolesBase(/*poleStopFactor=*/2),
  polyStartAngle(0),
  poleIntersectionStartAngle(0),
  factor(factorIn)
{;}

BDSMagnetOuterFactoryPolesFacetCommon::~BDSMagnetOuterFactoryPolesFacetCommon()
{;}

void BDSMagnetOuterFactoryPolesFacetCommon::CalculateStartAngles()
{
  polyStartAngle = 0;
  poleIntersectionStartAngle = -segmentAngle*0.5 - CLHEP::halfpi;
}

void BDSMagnetOuterFactoryPolesFacetCommon::CreateYokeAndContainerSolid(const G4String& name,
									G4double        length,
									G4int           order,
									G4double        magnetContainerLength,
								        G4double        magnetContainerRadiusIn)
{
  G4double zPlanesMagCont[2] = {-magnetContainerLength*0.5, magnetContainerLength*0.5};
  G4double zPlanesCont[2]  = {-length*0.5,      length*0.5};
  G4double zPlanes[2]      = {-length*0.5+lengthSafety, length*0.5-lengthSafety};
  G4double zPlanesLong[2]  = {-length,          length}; // for intersections
  G4double innerRadii[2]   = {yokeStartRadius,  yokeStartRadius};
  G4double outerRadii[2]   = {yokeFinishRadius, yokeFinishRadius};
  G4double zeroRadii[2]    = {0,0};
  G4double poleEnd         = yokeStartRadius - lengthSafetyLarge;
  G4double poleEndRadii[2] = {poleEnd,          poleEnd};
  G4int numberOfSides      = (G4int)factor*2*order;
  
  CalculateStartAngles();
  
  // The start angle for G4Polyhedra lies on a vertex / point and is at the top
  // in the positive y direction.
  yokeSolid = new G4Polyhedra(name + "_yoke_solid",    // name
			      polyStartAngle,          // start angle
			      CLHEP::twopi,            // sweep angle
			      numberOfSides,           // number of sides
			      2,                       // number of z planes
			      zPlanes,                 // z plane z coordinates
			      innerRadii,
			      outerRadii);

  // pole intersection solid
  poleIntersectionSolid = new G4Polyhedra(name + "_yoke_intersection_solid", // name
					  poleIntersectionStartAngle,        // start angle
					  CLHEP::twopi,                      // sweep angle
					  numberOfSides,                     // number of sides
					  2,                                 // number of z planes
					  zPlanesLong,                       // z plane z coordinates
					  zeroRadii,
					  poleEndRadii);
  allSolids.insert(poleIntersectionSolid);					  
  
  G4double contInnerRadii[2] = {0, 0}; // solid polyhedra
  G4double contOuterRadii[2] = {yokeFinishRadius + lengthSafety, yokeFinishRadius + lengthSafety};
  G4VSolid* containerOuterSolid = new G4Polyhedra(name + "_container_outer_solid", // name
						  polyStartAngle,                  // start angle
						  CLHEP::twopi,                    // sweep angle
						  numberOfSides,                   // number of sides
						  2,                               // number of z planes
						  zPlanesCont,                     // z plane z coordinates
						  contInnerRadii,
						  contOuterRadii);

  G4VSolid* containerInnerSolid = new G4Tubs(name + "_container_solid",    // name
					     0,                            // start radius
					     poleStartRadius,              // finish radius
					     length,                       // z half length
					     0,                            // start angle
					     CLHEP::twopi);                // sweep angle
  // z long for unambiguous subtraction
  allSolids.insert(containerOuterSolid);
  allSolids.insert(containerInnerSolid);
  
  containerSolid = new G4SubtractionSolid(name + "_container_solid", // name
					  containerOuterSolid,       // this
					  containerInnerSolid);      // minus this with no translation or rotation
  
  G4double magContOuterRadii[2] = {magnetContainerRadiusIn, magnetContainerRadiusIn};
  magnetContainerSolid = new G4Polyhedra(name + "_container_solid", // name
					 polyStartAngle,            // start angle
					 CLHEP::twopi,              // sweep angle
					 numberOfSides,             // number of sides
					 2,                         // number of z planes
					 zPlanesMagCont,            // z plane z coordinates
					 contInnerRadii,
					 magContOuterRadii);


  magContExtent = BDSExtent(magnetContainerRadiusIn,
			    magnetContainerRadiusIn,
			    magnetContainerLength*0.5);
}
