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
#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipeFactoryElliptical.hh"
#include "BDSBeamPipeFactoryRectEllipse.hh"
#include "BDSBeamPipe.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4EllipticalTube.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4VSolid.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <set>
#include <string>
#include <utility>                         // for std::pair

BDSBeamPipeFactoryRectEllipse::BDSBeamPipeFactoryRectEllipse()
{;}

BDSBeamPipe* BDSBeamPipeFactoryRectEllipse::CreateBeamPipe(const G4String& nameIn,
							   G4double    lengthIn,
							   G4double    aper1In,
							   G4double    aper2In,
							   G4double    aper3In,
							   G4double    aper4In,
							   G4Material* vacuumMaterialIn,
							   G4double    beamPipeThicknessIn,
							   G4Material* beamPipeMaterialIn,
							   const G4String& /*pointsFileIn*/,
							   const G4String& /*pointsUnitIn*/)
{
  // clean up after last usage
  CleanUp();

  // CERN use rectellipse for everything because you know, why use different aperture types
  // tolerate the case where it's a) equal rect and ellipse parameters resulting in an ellipse
  // and b) the ellipse params are less than rect ones so also just an ellipse
  if (((aper1In == aper3In) && (aper2In == aper4In)) ||
      ( (aper3In < aper1In) && (aper4In < aper2In) ) )
    {
      BDSBeamPipeFactoryElliptical* ef = new BDSBeamPipeFactoryElliptical();
      BDSBeamPipe* result = ef->CreateBeamPipe(nameIn, lengthIn, aper1In, aper2In, aper3In, aper4In,
					       vacuumMaterialIn, beamPipeThicknessIn, beamPipeMaterialIn);
      delete ef;
      return result;
    }
  
  // build the solids
  //vacuum cylindrical solid (circular cross-section)
  G4VSolid* vacCylSolid = new G4EllipticalTube(nameIn + "_vacuum_ellipsoid", // name
					       aper3In,                      // horizontal semi-axis
					       aper4In,                      // vertical semi-axis
					       lengthIn*0.5-lengthSafety); // half length
  //vacuum box solid (rectangular cross-section)
  G4VSolid* vacRectSolid = new G4Box(nameIn + "_vacuum_box", // name
				     aper1In,                // x half width
				     aper2In,                // y half width
				     4*lengthIn); // z full width (long for unambiguous intersection)
  allSolids.insert(vacCylSolid);
  allSolids.insert(vacRectSolid);
  //intersection of both of these gives the desired shape
  vacuumSolid = new G4IntersectionSolid(nameIn + "_vacuum_solid", // name
					vacCylSolid,              // solid 1
					vacRectSolid);            // solid 2

  //beampipe solid
  //beampipe inner edge for subtraction (actually just like vacuum + lengthSafety)
  G4VSolid* bpInnerCylSolid = new G4EllipticalTube(nameIn + "_pipe_inner_ellipsoid",// name
						   aper3In + lengthSafetyLarge,     // horizontal semi-axis
						   aper4In + lengthSafetyLarge,     // vertical semi-axis
						   1.5*lengthIn); // length big for unambiguous subtraction (but < outerlength)
  //beampipe inner edge box solid (rectangular cross-section)
  G4VSolid* bpInnerRectSolid = new G4Box(nameIn + "_pipe_inner_box", // name
					 aper1In + lengthSafetyLarge,// x half width
					 aper2In + lengthSafetyLarge,// y half width
					 1.7*lengthIn); // z long for unambiguous intersection
  //beampipe inner intersection - 1.5*length long which is > half length for unambiguous subtraction later
  G4VSolid* bpInnerSolid = new G4IntersectionSolid(nameIn + "_pipe_inner_solid", // name
						   bpInnerCylSolid,              // solid 1
						   bpInnerRectSolid);            // solid 2

  //beampipe outer edge for subtraction (actually just like vacuum + lengthSafety)
  G4double extraWidth = beamPipeThicknessIn + lengthSafetyLarge;
  G4VSolid* bpOuterCylSolid = new G4EllipticalTube(nameIn + "_pipe_inner_ellipsoid",// name
						   aper3In + extraWidth,   // horizontal semi-axis
						   aper4In + extraWidth,   // hotizontal semi-axis
						   (lengthIn*0.5)-lengthSafety);  // half length
  //beampipe outer edge box solid (rectangular cross-section)
  G4VSolid* bpOuterRectSolid = new G4Box(nameIn + "_pipe_inner_box", // name
					 aper1In + extraWidth,       // x half width
					 aper2In + extraWidth,       // y half width
					 lengthIn); // z full width (long for unambiguous intersection)
  G4VSolid* bpOuterSolid = new G4IntersectionSolid(nameIn + "_pipe_inner_solid", // name
						   bpOuterCylSolid,              // solid 1
						   bpOuterRectSolid);            // solid 2

  allSolids.insert(bpInnerCylSolid);
  allSolids.insert(bpInnerRectSolid);
  allSolids.insert(bpInnerSolid);
  allSolids.insert(bpOuterCylSolid);
  allSolids.insert(bpOuterRectSolid);
  allSolids.insert(bpOuterSolid);
  
  //beampipe final subtraction between outer and inner edge
  beamPipeSolid = new G4SubtractionSolid(nameIn + "_pipe_solid",  // name
					 bpOuterSolid,            // this
  					 bpInnerSolid);           // minus this
  
  //container cylindrical solid (circular cross-section)
  G4VSolid* contCylSolid = new G4EllipticalTube(nameIn + "_vacuum_ellipsoid", // name
						aper3In + extraWidth + lengthSafetyLarge, // horizontal semi-axis
						aper4In + extraWidth + lengthSafetyLarge, // vertical semi-axis
						lengthIn*0.5); // half length
  //vacuum box solid (rectangular cross-section)
  G4VSolid* contRectSolid = new G4Box(nameIn + "_vacuum_box",                   // name
				      aper1In + extraWidth + lengthSafetyLarge, // x half width
				      aper2In + extraWidth + lengthSafetyLarge, // y half width
				      lengthIn); // z full width (long for unambiguous intersection)

  allSolids.insert(contCylSolid);
  allSolids.insert(contRectSolid);
  
  //intersection of both of these gives the desired shape
  containerSolid = new G4IntersectionSolid(nameIn + "_vacuum_solid", // name
					   contCylSolid,             // solid 1
					   contRectSolid);           // solid 2

  G4double width  = aper3In + extraWidth + lengthSafetyLarge;
  G4double height = aper2In + extraWidth + lengthSafetyLarge;

  CreateContainerSubtractionSolid(nameIn, lengthIn, beamPipeThicknessIn, aper1In, aper2In, aper3In, aper4In);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn, width, height);
}

BDSBeamPipe* BDSBeamPipeFactoryRectEllipse::CreateBeamPipe(const G4String&      nameIn,
							   G4double             lengthIn,
							   const G4ThreeVector& inputFaceNormalIn,
							   const G4ThreeVector& outputFaceNormalIn,
							   G4double      aper1In,
							   G4double      aper2In,
							   G4double      aper3In,
							   G4double      aper4In,
							   G4Material*   vacuumMaterialIn,
							   G4double      beamPipeThicknessIn,
							   G4Material*   beamPipeMaterialIn,
							   const G4String& /*pointsFileIn*/,
							   const G4String& /*pointsUnitIn*/)
{
  // clean up after last usage
  CleanUp();

  // CERN use rectellipse for everything because you know, why use different aperture types
  // tolerate the case where it's a) equal rect and ellipse parameters resulting in an ellipse
  // and b) the ellipse params are less than rect ones so also just an ellipse
  if (((aper1In == aper3In) && (aper2In == aper4In)) ||
      ( (aper3In < aper1In) && (aper4In < aper2In) ) )
    {
      BDSBeamPipeFactoryElliptical* ef = new BDSBeamPipeFactoryElliptical();
      BDSBeamPipe* result = ef->CreateBeamPipe(nameIn, lengthIn, inputFaceNormalIn, outputFaceNormalIn,
					       aper1In, aper2In, aper3In, aper4In,
					       vacuumMaterialIn, beamPipeThicknessIn, beamPipeMaterialIn);
      delete ef;
      return result;
    }
  
  inputFaceNormal  = inputFaceNormalIn;
  outputFaceNormal = outputFaceNormalIn;

  G4double width  = std::max(aper1In,aper3In) + beamPipeThicknessIn + lengthSafetyLarge;
  G4double height = std::max(aper2In,aper4In) + beamPipeThicknessIn + lengthSafetyLarge;
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, aper2In, aper3In, aper4In,
			    beamPipeThicknessIn, inputFaceNormal, outputFaceNormal);
  CreateContainerSubtractionSolid(nameIn, lengthIn, beamPipeThicknessIn, aper1In,
				  aper2In, aper3In, aper4In);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn,
				 lengthIn, width, height);
}

BDSBeamPipe* BDSBeamPipeFactoryRectEllipse::CommonFinalConstruction(const G4String& nameIn,
								    G4Material* vacuumMaterialIn,
								    G4Material* beamPipeMaterialIn,
								    G4double    lengthIn,
								    G4double    containerWidthIn,
								    G4double    containerHeightIn)
{
  BDSBeamPipeFactoryBase::CommonConstruction(nameIn, vacuumMaterialIn,
					     beamPipeMaterialIn, lengthIn);
  
  // record extents
  BDSExtent ext = BDSExtent(containerWidthIn, containerHeightIn, lengthIn*0.5);
  
  // build the BDSBeamPipe instance and return it
  return BuildBeamPipeAndRegisterVolumes(ext,containerWidthIn);
}

/// the angled ones have degeneracy in the geant4 solids they used so we can avoid code duplication
/// by grouping common construction tasks
void BDSBeamPipeFactoryRectEllipse::CreateGeneralAngledSolids(const G4String&      nameIn,
							      G4double             lengthIn,
							      G4double             aper1In,
							      G4double             aper2In,
							      G4double             aper3In,
							      G4double             aper4In,
							      G4double             beamPipeThicknessIn,
							      const G4ThreeVector& inputfaceIn,
							      const G4ThreeVector& outputfaceIn)
{
  // long length for unambiguous boolean - ensure no gaps in beam pipe geometry
  G4double angledVolumeLength = BDS::CalculateSafeAngledVolumeLength(inputfaceIn, outputfaceIn, lengthIn, aper1In);

  // build the solids
  // we can get the rectangular ellipse as in the straight case with the intersection of
  // an elliptical tube (always solid) and then we can use another intersection solid
  // with a larger (wider and taller) G4CutTubs to get the angled faces.
  //vacuum cylindrical solid (circular cross-section)
  G4VSolid* vacCylSolid = new G4EllipticalTube(nameIn + "_vacuum_ellipsoid",// name
					       aper3In,                     // horizontal semi-axis
					       aper4In,                     // vertical semi-axis
					       angledVolumeLength);         // long length for unambiguous intersection
  //vacuum box solid (rectangular cross-section)
  G4VSolid* vacRectSolid = new G4Box(nameIn + "_vacuum_box", // name
				     aper1In,                // x half width
				     aper2In,                // y half width
				     1.1*angledVolumeLength);    // long length for unambiguous intersection

  //intersection of both of these gives the desired shape
  G4VSolid* longVacuumSolid = new G4IntersectionSolid(nameIn + "_vacuum_long_solid", // name
						      vacCylSolid,                   // solid 1
						      vacRectSolid);                 // solid 2
  //prepare angled face large cylinder for intersection get angled faces
  //we can actually use this for the beampipe too later on - whew
  G4double angledFaceRadius = (std::max(std::max(aper1In,aper2In),std::max(aper3In,aper4In)) + beamPipeThicknessIn) * 1.1;

  // check faces of angled volume don't intersect - if it can be built, remaining angled volumes can be built
  CheckAngledVolumeCanBeBuilt(lengthIn, inputfaceIn, outputfaceIn, angledFaceRadius, nameIn);

  G4VSolid* vacuumAngledSolid = new G4CutTubs(nameIn + "_pipe_angled_faces",     // name
					      0,                                 // inner radius
					      angledFaceRadius,                  // outer radius
					      (lengthIn*0.5) - (lengthSafety), // accurate length
					      0,                                 // rotation start angle
					      CLHEP::twopi,                      // rotation sweep angle
					      inputfaceIn,                       // input face normal
					      outputfaceIn);                     // output face normal

  allSolids.insert(vacCylSolid);
  allSolids.insert(vacRectSolid);
  allSolids.insert(longVacuumSolid);
  allSolids.insert(vacuumAngledSolid);
  
  vacuumSolid = new G4IntersectionSolid(nameIn + "_vacuum_solid", // name
					longVacuumSolid,          // solid 1
					vacuumAngledSolid);       // solid 2

  //beampipe cylindrical solid (circular cross-section)
  //beampipe inner edge for subtraction (actually just like vacuum + lengthSafety)
  if (beamPipeThicknessIn <= lengthSafetyLarge)
    {
      G4String message = "beam pipe thickness in element \"" + nameIn
                         + "\" is thinner than minimum " + std::to_string(lengthSafetyLarge / CLHEP::mm) + "mm.";
      throw BDSException(__METHOD_NAME__, message);
    }
  G4VSolid* bpInnerCylSolid = new G4EllipticalTube(nameIn + "_pipe_inner_ellipsoid", // name
						   aper3In + lengthSafetyLarge,      // horizontal semi-axis
						   aper4In + lengthSafetyLarge,      // vertical semi-axis
                           angledVolumeLength); // length big for unambiguous subtraction (but < outerlength)
  //beampipe inner edge box solid (rectangular cross-section)
  G4VSolid* bpInnerRectSolid = new G4Box(nameIn + "_pipe_inner_box",  // name
					 aper1In + lengthSafetyLarge, // x half width
					 aper2In + lengthSafetyLarge, // y half width
					 1.1*angledVolumeLength);         // long length for unambiguous intersection
  //beampipe inner intersection - 1.5*length long which is > half length for unambiguous subtraction later
  G4VSolid* bpInnerSolid = new G4IntersectionSolid(nameIn + "_pipe_inner_solid", // name
						   bpInnerCylSolid,              // solid 1
						   bpInnerRectSolid);            // solid 2

  //beampipe outer edge for subtraction (actually just like vacuum + lengthSafety)
  //this length should be less than bpInnerSolid above but longer than the actual length for later intersection
  G4double extraWidth = beamPipeThicknessIn + lengthSafetyLarge;
  G4VSolid* bpOuterCylSolid = new G4EllipticalTube(nameIn + "_pipe_inner_ellipsoid", // name
						   aper3In + extraWidth,             // horizontal semi-axis
						   aper4In + extraWidth,             // vertical semi-axis
                                                   angledVolumeLength);                        // length
  //beampipe outer edge box solid (rectangular cross-section)
  G4VSolid* bpOuterRectSolid = new G4Box(nameIn + "_pipe_inner_box", // name
					 aper1In + extraWidth,       // x half width
					 aper2In + extraWidth,       // y half width
					 1.1*angledVolumeLength);        // long length for unambiguous intersection
  G4VSolid* bpOuterSolid = new G4IntersectionSolid(nameIn + "_pipe_inner_solid", // name
						   bpOuterCylSolid,              // solid 1
						   bpOuterRectSolid);            // solid 2
  //correct solid shape but too long
  G4VSolid* longBeamPipeSolid = new G4SubtractionSolid(nameIn + "_long_pipe_solid",  // name
						       bpOuterSolid,            // this
						       bpInnerSolid);           // minus this

  allSolids.insert(bpInnerCylSolid);
  allSolids.insert(bpInnerRectSolid);
  allSolids.insert(bpInnerSolid);
  allSolids.insert(bpOuterCylSolid);
  allSolids.insert(bpOuterRectSolid);
  allSolids.insert(bpOuterSolid);
  
  //final beampipe solid with correct shape and angled faces
  beamPipeSolid = new G4IntersectionSolid(nameIn + "_pipe_solid", // name
					  longBeamPipeSolid,      // solid1
					  vacuumAngledSolid);     // solid2

  //container solid
  //container cylindrical solid (circular cross-section)
  G4VSolid* contCylSolid = new G4EllipticalTube(nameIn + "_vacuum_ellipsoid",             // name
						aper3In + extraWidth + lengthSafetyLarge, // horizontal semi-axis
						aper4In + extraWidth + lengthSafetyLarge, // vertical semi-axis
						angledVolumeLength);                      // length
  // length both long and different from next solid for unamibiguous intersection
  
  //vacuum box solid (rectangular cross-section)
  G4VSolid* contRectSolid = new G4Box(nameIn + "_vacuum_box",                   // name
				      aper1In + extraWidth + lengthSafetyLarge, // x half width
				      aper2In + extraWidth + lengthSafetyLarge, // y half width
                                      1.1*angledVolumeLength); // z full width (long for unambiguous intersection)
  //intersection of both of these gives the desired shape
  G4VSolid* longContainerSolid = new G4IntersectionSolid(nameIn + "_long_container_solid", // name
							 contCylSolid,                     // solid 1
							 contRectSolid);                   // solid 2

  allSolids.insert(contCylSolid);
  allSolids.insert(contRectSolid);
  allSolids.insert(longContainerSolid);

  containerSolid = new G4IntersectionSolid(nameIn + "_container_solid", // name
					   longContainerSolid,          // solid 1
					   vacuumAngledSolid);          // solid 2
}

void BDSBeamPipeFactoryRectEllipse::CreateContainerSubtractionSolid(const G4String& nameIn,
								    G4double& lengthIn,
								    G4double& beamPipeThicknessIn,
								    G4double& aper1In,
								    G4double& aper2In,
								    G4double& aper3In,
								    G4double& aper4In)
{
  //container cylindrical solid (circular cross-section)
  G4VSolid* contSubCylSolid = new G4EllipticalTube(nameIn + "_subtraction_ellipsoid", // name
						   aper3In + beamPipeThicknessIn + 3*lengthSafetyLarge, // horizontal semi-axis
						   aper4In + beamPipeThicknessIn + 3*lengthSafetyLarge, // vertical semi-axis
						   2*lengthIn);                  // long length for unambiguous subtraction
  //vacuum box solid (rectangular cross-section)
  G4VSolid* contSubRectSolid = new G4Box(nameIn + "_subtraction_box",                         // name
					 aper1In + beamPipeThicknessIn + 2*lengthSafetyLarge, // x half width
					 aper2In + beamPipeThicknessIn + 2*lengthSafetyLarge, // y half width
					 1.7*lengthIn); // z full width (long for unambiguous intersection)
  allSolids.insert(contSubCylSolid);
  allSolids.insert(contSubRectSolid);
  
  //intersection of both of these gives the desired shape
  containerSubtractionSolid = new G4IntersectionSolid(nameIn + "_subtraction_solid", // name
						      contSubCylSolid,               // solid 1
						      contSubRectSolid);             // solid 2
}
