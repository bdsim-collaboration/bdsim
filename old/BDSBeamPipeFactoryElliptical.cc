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
#include "BDSBeamPipeFactoryCircular.hh"
#include "BDSBeamPipeFactoryElliptical.hh"
#include "BDSBeamPipe.hh"
#include "BDSExtent.hh"
#include "BDSUtilities.hh"

#include "globals.hh"                 // geant4 globals / types
#include "G4EllipticalTube.hh"
#include "G4CutTubs.hh"
#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4VSolid.hh"

#include <cmath>                           // sin, cos, fabs
#include <set>

BDSBeamPipeFactoryElliptical::BDSBeamPipeFactoryElliptical()
{;}

BDSBeamPipe* BDSBeamPipeFactoryElliptical::CreateBeamPipe(const G4String& nameIn,
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

  if (aper1In == aper2In)
    {//optimise geometry by using circular solids
      BDSBeamPipeFactoryCircular* cf = new BDSBeamPipeFactoryCircular();
      auto result = cf->CreateBeamPipe(nameIn, lengthIn, aper2In, aper2In, aper3In, aper4In,
				       vacuumMaterialIn, beamPipeThicknessIn, beamPipeMaterialIn);
      delete cf;
      return result;
    }
  
  // build the solids
  vacuumSolid = new G4EllipticalTube(nameIn + "_vacuum_solid",       // name
				     aper1In,                        // x half width
				     aper2In,                        // y half width
				     (lengthIn*0.5)-lengthSafety); // half length

  G4VSolid* beamPipeSolidInner; // construct rectangular beam pipe by subtracting an inner
  G4VSolid* beamPipeSolidOuter; // box from an outer one - only way
  // beamPipeSolidInner will be the inner edge of the metal beampipe
  // therefore it has to be the width of the aperture + lengthSafety
  beamPipeSolidInner = new G4EllipticalTube(nameIn + "_pipe_solid_inner", // name
					    aper1In + lengthSafetyLarge,  // x half width - length safety to avoid overlaps
					    aper2In + lengthSafetyLarge,  // y half width
					    lengthIn);                    // length - full length for unambiguous subtraction
  // beamPipeSolidOuter will be the outer edge of the metal beampipe
  // therefore it has to be the width of the aperture + beampipeThickness
  G4double extraWidth = lengthSafetyLarge + beamPipeThicknessIn;
  beamPipeSolidOuter = new G4EllipticalTube(nameIn + "_pipe_solid_outer", // name
					    aper1In + extraWidth,         // x half width
					    aper2In + extraWidth,         // y half width
					    (lengthIn*0.5)-lengthSafety); // half length - lengthSafety to fit in container
  beamPipeSolid = new G4SubtractionSolid(nameIn + "_pipe_solid",
					 beamPipeSolidOuter,
					 beamPipeSolidInner); // outer minus inner
  
  G4double containerXHalfWidth = aper1In + extraWidth + lengthSafetyLarge;
  G4double containerYHalfWidth = aper2In + extraWidth + lengthSafetyLarge;
  containerSolid = new G4EllipticalTube(nameIn  + "_container_solid",  // name
					containerXHalfWidth,           // x half width
					containerYHalfWidth,           // y half width
					lengthIn*0.5);                 // half length
					
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn,
				 lengthIn, containerXHalfWidth, containerYHalfWidth);
}

BDSBeamPipe* BDSBeamPipeFactoryElliptical::CreateBeamPipe(const G4String&      nameIn,
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
  
  if (aper1In == aper2In)
    {//optimise geometry by using circular solids
      BDSBeamPipeFactoryCircular* cf = new BDSBeamPipeFactoryCircular();
      auto result = cf->CreateBeamPipe(nameIn, lengthIn, inputFaceNormalIn, outputFaceNormalIn,
				       aper2In, aper2In, aper3In, aper4In,
				       vacuumMaterialIn, beamPipeThicknessIn, beamPipeMaterialIn);
      delete cf;
      return result;
    }
  
  inputFaceNormal  = inputFaceNormalIn;
  outputFaceNormal = outputFaceNormalIn;

  G4double containerRadiusX = 0;
  G4double containerRadiusY = 0;
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, aper2In, beamPipeThicknessIn,
			    inputFaceNormal, outputFaceNormal, containerRadiusX, containerRadiusY);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn,
				 lengthIn, containerRadiusX, containerRadiusY);
}

BDSBeamPipe* BDSBeamPipeFactoryElliptical::CommonFinalConstruction(const G4String& nameIn,
								   G4Material* vacuumMaterialIn,
								   G4Material* beamPipeMaterialIn,
								   G4double    lengthIn,
								   G4double    containerRadiusX,
								   G4double    containerRadiusY)
{
  // doesn't have to be angled as it's only used for transverse subtraction
  containerSubtractionSolid = new G4EllipticalTube(nameIn  + "_container_solid", // name
						   containerRadiusX,             // x half width
						   containerRadiusY,             // y half width
						   lengthIn);                    // full length for unambiguous subtraction


  BDSBeamPipeFactoryBase::CommonConstruction(nameIn, vacuumMaterialIn,
					     beamPipeMaterialIn, lengthIn);

  // record extents
  BDSExtent ext = BDSExtent(containerRadiusX, containerRadiusY, lengthIn*0.5);
  
  // calculate radius if a tube were to be place around it
  G4double containerRadius = std::max(containerRadiusX, containerRadiusY);
  
  BDSBeamPipe* aPipe = BuildBeamPipeAndRegisterVolumes(ext, containerRadius);
  
  return aPipe;
}

void BDSBeamPipeFactoryElliptical::CreateGeneralAngledSolids(const G4String&      nameIn,
							     G4double             lengthIn,
							     G4double             aper1In,
							     G4double             aper2In,
							     G4double             beamPipeThicknessIn,
							     const G4ThreeVector& inputfaceIn,
							     const G4ThreeVector& outputfaceIn,
							     G4double&     containerRadiusX,
							     G4double&     containerRadiusY)
{
  // this function will make a longer normal rectangular beampipe and chop it off
  // to make angled faces as required
  // achieve this using the intersection of the normal beampipe (but a little longer)
  // with a large G4CutTubs to get the angled faces.
  // note even if one face is flat, we don't save a boolean operation as the intersection
  // can be on both sides using a G4CutTubs.  Also, keeping one side flat would require
  // shifting the volume from 0 which causes headaches later with SDs.

  // build the solids - vacuum, beampipe and container solids
  // extra solids required for booleans
  G4VSolid* vacuumSolidLong;
  G4VSolid* beamPipeSolidLong;
  G4VSolid* angledFaceSolid;
  G4VSolid* containerSolidLong;
  G4VSolid* angledFaceSolidContainer;

  // build the solid with angled faces for intersection
  G4double angledFaceRadius = (std::max(aper1In,aper2In) + beamPipeThicknessIn)*2.0; //huge for unambiguous intersection

  // check faces of angled volume don't intersect - if it can be built, remaining angled volumes can be built
  CheckAngledVolumeCanBeBuilt(lengthIn, inputfaceIn, outputfaceIn, angledFaceRadius, nameIn);

  angledFaceSolid = new G4CutTubs(nameIn + "_angled_face",       // name
				  0,                             // inner radius
				  angledFaceRadius,              // outer radius
				  (lengthIn*0.5)-lengthSafety, // half length - must fit within container
				  0,                             // rotation start angle
				  CLHEP::twopi,                  // rotation finish angle
				  inputfaceIn,                   // input face normal
				  outputfaceIn);                 // output face normal

  // long length for unambiguous boolean - ensure no gaps in beam pipe geometry
  G4double angledVolumeLength = BDS::CalculateSafeAngledVolumeLength(inputfaceIn, outputfaceIn, lengthIn, angledFaceRadius);

  vacuumSolidLong = new G4EllipticalTube(nameIn + "_vacuum_solid_long", // name
					 aper1In,                       // x half width
					 aper2In,                       // y half width
					 angledVolumeLength);           // long length for unambiguous boolean
  vacuumSolid     = new G4IntersectionSolid(nameIn + "_vacuum_solid",
					    vacuumSolidLong,
					    angledFaceSolid);

  allSolids.insert(angledFaceSolid);
  allSolids.insert(vacuumSolidLong);
  
  G4VSolid* beamPipeSolidInner; // construct rectangular beam pipe by subtracting an inner
  G4VSolid* beamPipeSolidOuter; // box from an outer one - only way
  // beamPipeSolidInner will be the inner edge of the metal beampipe
  // therefore it has to be the width of the aperture + lengthSafety
  beamPipeSolidInner = new G4EllipticalTube(nameIn + "_pipe_solid_inner",// name
					    aper1In + lengthSafetyLarge, // x half width - length safety to avoid overlaps
					    aper2In + lengthSafetyLarge, // y half width
					    angledVolumeLength);         // long length for unambiguous subtraction
  // beamPipeSolidOuter will be the outer edge of the metal beampipe
  // therefore it has to be the width of the aperture + beampipeThickness
  G4double extraWidth = lengthSafetyLarge + beamPipeThicknessIn;
  beamPipeSolidOuter = new G4EllipticalTube(nameIn + "_pipe_solid_outer",// name
					    aper1In + extraWidth,        // x half width
					    aper2In + extraWidth,        // y half width
					    angledVolumeLength);         // long length for unambiguous subtraction
  beamPipeSolidLong = new G4SubtractionSolid(nameIn + "_pipe_solid_long",
					     beamPipeSolidOuter,
					     beamPipeSolidInner); // outer minus inner
  allSolids.insert(beamPipeSolidInner);
  allSolids.insert(beamPipeSolidOuter);
  allSolids.insert(beamPipeSolidLong);
  
  beamPipeSolid = new G4IntersectionSolid(nameIn + "_pipe_solid",
					  beamPipeSolidLong,
					  angledFaceSolid);
  
  containerRadiusX = aper1In + extraWidth + lengthSafety;
  containerRadiusY = aper2In + extraWidth + lengthSafety;
  containerSolidLong = new G4EllipticalTube(nameIn  + "_container_solid_long",// name
					    containerRadiusX,                 // x half width
					    containerRadiusY,                 // y half width
					    angledVolumeLength);              // long length for unambiguous subtraction
  angledFaceSolidContainer = new G4CutTubs(nameIn + "_angled_face_container",// name
					   0,                                // inner radius
					   angledFaceRadius,                 // outer radius
					   (lengthIn*0.5),                   // half length
					   0,                                // rotation start angle
					   CLHEP::twopi,                     // rotation finish angle
					   inputfaceIn,                      // input face normal
					   outputfaceIn);                    // output face normal
  allSolids.insert(containerSolidLong);
  allSolids.insert(angledFaceSolidContainer);
  
  containerSolid = new G4IntersectionSolid(nameIn + "_container_solid",
					   containerSolidLong,
					   angledFaceSolidContainer);
}
