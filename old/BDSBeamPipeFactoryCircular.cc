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
#include "BDSBeamPipeFactoryCircular.hh"
#include "BDSBeamPipe.hh"
#include "BDSExtent.hh"
#include "BDSUtilities.hh"

#include "globals.hh"                      // geant4 globals / types
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"

#include <cmath>
#include <set>
#include <utility>               // for std::pair

BDSBeamPipeFactoryCircular::BDSBeamPipeFactoryCircular()
{;}

BDSBeamPipe* BDSBeamPipeFactoryCircular::CreateBeamPipe(const G4String& nameIn,
							G4double    lengthIn,
							G4double    aper1In,
							G4double    /*aper2In*/,
							G4double    /*aper3In*/,
							G4double    /*aper4In*/,
							G4Material* vacuumMaterialIn,
							G4double    beamPipeThicknessIn,
							G4Material* beamPipeMaterialIn,
							const G4String& /*pointsFileIn*/,
							const G4String& /*pointsUnitIn*/)
{
  // clean up after last usage
  CleanUp();

  // build the solids
  vacuumSolid   = new G4Tubs(nameIn + "_vacuum_solid",      // name
			     0,                             // inner radius
			     aper1In,                       // outer radius
			     lengthIn*0.5 - lengthSafety, // half length
			     0,                             // rotation start angle
			     CLHEP::twopi);                 // rotation finish angle
  
  beamPipeSolid = new G4Tubs(nameIn + "_pipe_solid",        // name
			     aper1In + lengthSafetyLarge,   // inner radius + length safety to avoid overlaps
			     aper1In + lengthSafetyLarge + beamPipeThicknessIn, // outer radius
			     lengthIn*0.5 - lengthSafety, // half length
			     0,                             // rotation start angle
			     CLHEP::twopi);                 // rotation finish angle
  
  G4double containerRadius = aper1In + beamPipeThicknessIn + 2*lengthSafetyLarge;
  containerSolid = new G4Tubs(nameIn + "_container_solid",  // name
			      0,                            // inner radius
			      containerRadius,              // outer radius
			      lengthIn*0.5,                 // half length
			      0,                            // rotation start angle
			      CLHEP::twopi);                // rotation finish angle
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn,
				 lengthIn, containerRadius);
}

BDSBeamPipe* BDSBeamPipeFactoryCircular::CreateBeamPipe(const G4String&      nameIn,
							G4double             lengthIn,
							const G4ThreeVector& inputFaceNormalIn,
							const G4ThreeVector& outputFaceNormalIn,
							G4double      aper1In,
							G4double      /*aper2In*/,
							G4double      /*aper3In*/,
							G4double      /*aper4In */,
							G4Material*   vacuumMaterialIn,
							G4double      beamPipeThicknessIn,
							G4Material*   beamPipeMaterialIn,
							const G4String& /*pointsFileIn*/,
							const G4String& /*pointsUnitIn*/)
{
  // clean up after last usage
  CleanUp();
  inputFaceNormal  = inputFaceNormalIn;
  outputFaceNormal = outputFaceNormalIn;
  
  G4double containerRadius = 0;
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, beamPipeThicknessIn,
			    inputFaceNormal, outputFaceNormal, containerRadius);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn,
				 lengthIn, containerRadius);
}

/// only the solids are unique, once we have those, the logical volumes and placement in the
/// container are the same.  group all this functionality together
BDSBeamPipe* BDSBeamPipeFactoryCircular::CommonFinalConstruction(const G4String& nameIn,
								 G4Material* vacuumMaterialIn,
								 G4Material* beamPipeMaterialIn,
								 G4double    lengthIn,
								 G4double    containerRadiusIn)
{
  // prepare a longer container subtraction solid
  // doesn't have to be angled as it's only used for transverse subtraction
  containerSubtractionSolid = new G4Tubs(nameIn + "_container_sub_solid",// name
					 0,                              // inner radius
					 containerRadiusIn,              // outer radius
					 lengthIn*4,                     // full length for unambiguous subtraction
					 0,                              // rotation start angle
					 CLHEP::twopi);                  // rotation finish angle

  BDSBeamPipeFactoryBase::CommonConstruction(nameIn, vacuumMaterialIn,
					     beamPipeMaterialIn, lengthIn);

  // record extents
  BDSExtent ext = BDSExtent(containerRadiusIn, containerRadiusIn, lengthIn*0.5);

  // true for containerIsCircular - true for this factory
  return BDSBeamPipeFactoryBase::BuildBeamPipeAndRegisterVolumes(ext, containerRadiusIn, true);
}

/// the angled ones have degeneracy in the geant4 solids they used so we can avoid code duplication
/// by grouping common construction tasks
void BDSBeamPipeFactoryCircular::CreateGeneralAngledSolids(const G4String&      nameIn,
							   G4double             lengthIn,
							   G4double             aper1In,
							   G4double             beamPipeThicknessIn,
							   const G4ThreeVector& inputfaceIn,
							   const G4ThreeVector& outputfaceIn,
							   G4double&     containerRadius)
{
  // long length for unambiguous boolean - ensure no gaps in beam pipe geometry
  G4double angledVolumeLength = BDS::CalculateSafeAngledVolumeLength(inputfaceIn, outputfaceIn, lengthIn, aper1In);

  G4double extraWidth = lengthSafetyLarge + beamPipeThicknessIn;
  containerRadius = aper1In + extraWidth + lengthSafetyLarge;

  // check faces of angled container volume don't intersect - if it can be built, remaining angled volumes can be built
  CheckAngledVolumeCanBeBuilt(lengthIn, inputfaceIn, outputfaceIn, containerRadius, nameIn);

  containerSolid = new G4CutTubs(nameIn + "_container_solid",  // name
                                 0,                            // inner radius
                                 containerRadius,              // outer radius
                                 lengthIn*0.5,                 // half length - no -length safety!
                                 0,                            // rotation start angle
                                 CLHEP::twopi,                 // rotation finish angle
                                 inputfaceIn,                  // input face normal
                                 outputfaceIn);                // rotation finish angle

  // build the solids
  vacuumSolid   = new G4CutTubs(nameIn + "_vacuum_solid",      // name
				0,                             // inner radius
				aper1In,                       // outer radius
				lengthIn*0.5 - lengthSafety, // half length
				0,                             // rotation start angle
				CLHEP::twopi,                  // rotation finish angle
				inputfaceIn,                   // input face normal
				outputfaceIn );                // output face normal
  
  // beampipesolid created as subtraction since direct G4CutTubs creation
  // created scattering in sector bends. not really understood but likely
  // fault in G4CutTubs
  G4VSolid* inner = new G4CutTubs(nameIn + "_pipe_inner_solid",  // name
				  0,                             // inner radius
				  aper1In + lengthSafetyLarge,   // outer radius
				  angledVolumeLength,            // long length!
				  0,                             // rotation start angle
				  CLHEP::twopi,                  // rotation finish angle
				  inputfaceIn,                   // input face normal
				  outputfaceIn);                 // output face normal

  G4VSolid* outer = new G4CutTubs(nameIn + "_pipe_outer_solid",  // name
				  0,                             // inner radius + length safety to avoid overlaps
				  aper1In + extraWidth,          // outer radius
				  lengthIn*0.5 - lengthSafety,   // half length
				  0,                             // rotation start angle
				  CLHEP::twopi,                  // rotation finish angle
				  inputfaceIn,                   // input face normal
				  outputfaceIn);                 // output face normal
  allSolids.insert(inner);
  allSolids.insert(outer);
  
  beamPipeSolid = new G4SubtractionSolid(nameIn + "_pipe_solid",
					 outer,
					 inner);
}
