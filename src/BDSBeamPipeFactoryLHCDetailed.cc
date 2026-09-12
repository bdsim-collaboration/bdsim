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
#include "BDSBeamPipeFactoryLHCDetailed.hh"
#include "BDSBeamPipe.hh"
#include "BDSColours.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSSDType.hh"

#include "globals.hh"                      // geant4 globals / types
#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

#include <cmath>
#include <set>
#include <utility>                         // for std::pair

BDSBeamPipeFactoryLHCDetailed::BDSBeamPipeFactoryLHCDetailed()
{
  coldBoreThickness    = 1.5*CLHEP::mm;
  coolingPipeThickness = 0.53*CLHEP::mm;
  coolingPipeRadius    = 3.7*CLHEP::mm*0.5; // will be overwritten if needs be to fit inside beampipe
  coolingPipeOffset    = 0.0;  //initialised only
  copperSkinThickness  = 75*CLHEP::um;
  CleanUpLHCDetailed();
}

void BDSBeamPipeFactoryLHCDetailed::CleanUp()
{
  CleanUpLHCDetailed();
  BDSBeamPipeFactoryBase::CleanUp();
}

void BDSBeamPipeFactoryLHCDetailed::CleanUpLHCDetailed()
{
  copperSkinSolid     = nullptr; // the copper skin
  screenSolid         = nullptr; // the beam screen (first bit of aperture)
  coolingPipeSolid    = nullptr; // small cooling pipe above and below beam screen
  copperSkinLV        = nullptr;
  screenLV            = nullptr;
  coolingPipeLV       = nullptr;
  copperSkinPV        = nullptr;
  screenPV            = nullptr;
  coolingPipeTopPV    = nullptr;
  coolingPipeBottomPV = nullptr;
  
  InitialiseGeometricalParameters();
}

void BDSBeamPipeFactoryLHCDetailed::InitialiseGeometricalParameters()
{
  vacRadius = 0, vacBoxX = 0, vacBoxY = 0;
  
  cuInnerRadius = 0, cuInnerBoxX = 0, cuInnerBoxY = 0;
  cuOuterRadius = 0, cuOuterBoxX = 0, cuOuterBoxY = 0;
  
  bsInnerRadius = 0, bsInnerBoxX = 0, bsInnerBoxY = 0;
  bsOuterRadius = 0, bsOuterBoxX = 0, bsOuterBoxY = 0;
  
  cbInnerRadius = 0, cbOuterRadius = 0;
  
  containerRadius = 0;
  
  vacHalfLength = 0;
  halfLength = 0;
  
  coolingPipeOffset = 0;
  verticalOrientation = true; // arbitrary, will be overwritten
  buildCoolingPipe = true;
}


G4double BDSBeamPipeFactoryLHCDetailed::GetFullWidthOfCoolingPipe()
{
  return coolingPipeRadius + coolingPipeThickness + 2*lengthSafety;
}

void BDSBeamPipeFactoryLHCDetailed::CalculateGeometricalParameters(G4double aper1,
								   G4double aper2,
								   G4double aper3,
								   G4double beamPipeThickness,
								   G4double length)
{
  // vacuum volume
  vacRadius     = aper3;
  vacBoxX       = aper1;
  vacBoxY       = aper2;
  
  // copper skin geometrical parameters
  cuInnerRadius = vacRadius + 1*CLHEP::um;
  cuInnerBoxX   = vacBoxX   + 1*CLHEP::um;
  cuInnerBoxY   = vacBoxY   + 1*CLHEP::um;
  cuOuterRadius = cuInnerRadius + copperSkinThickness;
  cuOuterBoxX   = cuInnerBoxX   + copperSkinThickness;
  cuOuterBoxY   = cuInnerBoxY   + copperSkinThickness;

  // beam screen geometrical parameters
  bsInnerRadius = cuOuterRadius + 1*CLHEP::um;
  bsInnerBoxX   = cuOuterBoxX   + 1*CLHEP::um;
  bsInnerBoxY   = cuOuterBoxY   + 1*CLHEP::um;
  bsOuterRadius = bsInnerRadius + beamPipeThickness;
  bsOuterBoxX   = bsInnerBoxX   + beamPipeThickness;
  bsOuterBoxY   = bsInnerBoxY   + beamPipeThickness;
  
  // cold bore geometrical parameters
  cbInnerRadius = bsOuterRadius + 1*CLHEP::um;
  cbOuterRadius = cbInnerRadius + coldBoreThickness;

  // container geometrical parameters
  containerRadius = cbOuterRadius + 1*CLHEP::um;  
  
  // general length variable (to avoid mistakes)
  vacHalfLength   = length*0.5 - lengthSafety;
  halfLength      = length*0.5 - 1*CLHEP::um; 
  
  // cooling pipe geometrical parameters
  G4double fullWidthOfCoolingPipe = coolingPipeRadius + coolingPipeThickness + 1*CLHEP::um;
  if (aper1 > aper2)
    {
      coolingPipeOffset = bsOuterBoxY + fullWidthOfCoolingPipe;
      verticalOrientation = false;
    }
  else
    {
      coolingPipeOffset = bsOuterBoxX + fullWidthOfCoolingPipe;
      verticalOrientation = true;
    }

  G4double valueToCompare;
  if (verticalOrientation)
    {valueToCompare = bsOuterBoxX;}
  else
    {valueToCompare = bsInnerBoxY;}

  G4double gapForCoolingPipe = std::abs(cbInnerRadius - valueToCompare);
  if (gapForCoolingPipe < fullWidthOfCoolingPipe + 1*CLHEP::um)
    {buildCoolingPipe = false;}
}
  

BDSBeamPipe* BDSBeamPipeFactoryLHCDetailed::CreateBeamPipe(const G4String& name,
							   G4double    length,
							   G4double    aper1,
							   G4double    aper2,
							   G4double    aper3,
							   G4double    /*aper4*/,
							   G4Material* vacuumMaterial,
							   G4double    beamPipeThickness,
							   G4Material* beamPipeMaterial,
							   const G4String& /*pointsFileIn*/,
							   const G4String& /*pointsUnitIn*/)
{
  // clean up after last usage
  CleanUp();

  // we override the material of the beam pipe
  beamPipeMaterial = BDSMaterials::Instance()->GetMaterial("stainless_steel_316LN_2K");
  
  // calculate geometrical parameters
  CalculateGeometricalParameters(aper1, aper2, aper3, beamPipeThickness, length);

  // build the solids
  //vacuum cylindrical solid (circular cross-section)
  G4VSolid* vacCylSolid = new G4Tubs(name + "_vacuum_cylinder", // name
				     0,                           // inner radius
				     vacRadius,                   // outer radius
				     vacHalfLength,               // half length
				     0,                           // rotation start angle
				     CLHEP::twopi);               // rotation finish angle
  //vacuum box solid (rectangular cross-section)
  G4VSolid* vacRectSolid = new G4Box(name + "_vacuum_box", // name
				     vacBoxX,                // x half width
				     vacBoxY,                // y half width
				     length); // z full width (long for unambiguous intersection)
  allSolids.insert(vacCylSolid);
  allSolids.insert(vacRectSolid);
  
  //intersection of both of these gives the desired shape
  vacuumSolid = new G4IntersectionSolid(name + "_vacuum_solid", // name
					vacCylSolid,              // solid 1
					vacRectSolid);            // solid 2


  //copper skin layer to beamscreen
  //copper skin inner edge for subtraction (actually just like vacuum + lengthSafety)
  G4VSolid* cuInnerCylSolid = new G4Tubs(name + "_cu_inner_cylinder", // name
					 0,                             // inner radius
					 cuInnerRadius,                 // outer radius
					 1.5*length,  // length big for unambiguous subtraction (but < outerlength)
					 0,                             // rotation start angle
					 CLHEP::twopi);                 // rotation finish angle
  //screen inner edge box solid (rectangular cross-section)
  G4VSolid* cuInnerRectSolid = new G4Box(name + "_cu_inner_box", // name
					 cuInnerBoxX,              // x half width
					 cuInnerBoxY,              // y half width
					 1.7*length); // z long for unambiguous intersection
  //screen inner intersection - 1.5*length long which is > half length for unambiguous subtraction later
  G4VSolid* cuInnerSolid = new G4IntersectionSolid(name + "_cu_inner_solid", // name
						   cuInnerCylSolid,            // solid 1
						   cuInnerRectSolid);          // solid 2

  // screen outer edge for subtraction (actually just like vacuum + lengthSafety)
  // halfLength is vacLength - 1um
  G4VSolid* cuOuterCylSolid = new G4Tubs(name + "_cu_outer_cylinder",    // name
					 0,                                // inner radius (0 for unambiguous subtraction)
					 cuOuterRadius,                    // outer radius
					 halfLength,                       // half length
					 0,                                // rotation start angle
					 CLHEP::twopi);                    // rotation finish angle
  //screen outer edge box solid (rectangular cross-section)
  G4VSolid* cuOuterRectSolid = new G4Box(name + "_cu_outer_box", // name
					 cuOuterBoxX,              // x half width
					 cuOuterBoxY,              // y half width
					 length); // z full width (long for unambiguous intersection)
  G4VSolid* cuOuterSolid = new G4IntersectionSolid(name + "_cu_outer_solid", // name
						   cuOuterCylSolid,            // solid 1
						   cuOuterRectSolid);          // solid 2
  copperSkinSolid = new G4SubtractionSolid(name + "_copper_skin_solid",// name
					   cuOuterSolid,                 // this
					   cuInnerSolid);                // minus this
  allSolids.insert(cuInnerCylSolid);
  allSolids.insert(cuInnerRectSolid);
  allSolids.insert(cuInnerSolid);
  allSolids.insert(cuOuterCylSolid);
  allSolids.insert(cuOuterRectSolid);
  allSolids.insert(cuOuterSolid);
  allSolids.insert(copperSkinSolid);
  
  //beampipe cylindrical solid (circular cross-section)
  //beampipe inner edge for subtraction (actually just like vacuum + lengthSafety)
  //using large margin of 1um here to avoid misplacement errors with compound booleans in geant4 (empirical solution)
  G4VSolid* screenInnerCylSolid = new G4Tubs(name + "_screen_inner_cylinder", // name
					     0,                                 // inner radius
					     bsInnerRadius,                     // outer radius
					     1.5*length, // length big for unambiguous subtraction (but < outerlength)
					     0,                                 // rotation start angle
					     CLHEP::twopi);                     // rotation finish angle
  //screen inner edge box solid (rectangular cross-section)
  //using large margin of 1um here to avoid misplacement errors with compound booleans in geant4 (empirical solution)
  G4VSolid* screenInnerRectSolid = new G4Box(name + "_screen_inner_box", // name
					     bsInnerBoxX,                  // x half width
					     bsInnerBoxY,                  // y half width
					     1.7*length); // z long for unambiguous intersection
  //screen inner intersection - 1.5*length long which is > half length for unambiguous subtraction later
  G4VSolid* screenInnerSolid = new G4IntersectionSolid(name + "_screen_inner_solid", // name
						       screenInnerCylSolid,            // solid 1
						       screenInnerRectSolid);          // solid 2

  //screen outer edge for subtraction (actually just like vacuum + lengthSafety)x
  G4VSolid* screenOuterCylSolid = new G4Tubs(name + "_screen_outer_cylinder",// name
					     0,                                // inner radius (0 for unambiguous subtraction)
					     bsOuterRadius,                    // outer radius
					     halfLength,                       // half length
					     0,                                // rotation start angle
					     CLHEP::twopi);                    // rotation finish angle
  //screen outer edge box solid (rectangular cross-section)
  G4VSolid* screenOuterRectSolid = new G4Box(name + "_screen_outer_box", // name
					     bsOuterBoxX,                  // x half width
					     bsOuterBoxY,                  // y half width
					     length); // z full width (long for unambiguous intersection)
  G4VSolid* screenOuterSolid = new G4IntersectionSolid(name + "_screen__outersolid", // name
						       screenOuterCylSolid,            // solid 1
						       screenOuterRectSolid);          // solid 2
  screenSolid = new G4SubtractionSolid(name + "_screen_solid",// name
				       screenOuterSolid,        // this
				       screenInnerSolid);       // minus this
  allSolids.insert(screenInnerCylSolid);
  allSolids.insert(screenInnerRectSolid);
  allSolids.insert(screenInnerSolid);
  allSolids.insert(screenOuterCylSolid);
  allSolids.insert(screenOuterRectSolid);
  allSolids.insert(screenOuterSolid);
  allSolids.insert(screenSolid);

  if (buildCoolingPipe)
    {
      coolingPipeSolid = new G4Tubs(name + "_cooling_pipe_solid",           // name
				    coolingPipeRadius,                        // inner radius
				    coolingPipeRadius + coolingPipeThickness, // outer radius
				    halfLength,                               // half length
				    0,                                        // rotation start angle
				    CLHEP::twopi);                            // rotation finish angle

      allSolids.insert(coolingPipeSolid);
    }
  
  // beampipe - ("coldbore") circular cross-section and sits outisde screen and cooling pipe
  beamPipeSolid = new G4Tubs(name + "_beampipe_solid",         // name
			     cbInnerRadius,                      // inner radius
			     cbOuterRadius,                      // outer radius
			     halfLength,                         // half length
			     0,                                  // rotation start angle
			     CLHEP::twopi);                      // rotation finish angle
  
  //container cylindrical solid (circular cross-section)
  // 'length' is correct full length of object
  containerSolid = new G4Tubs(name + "_container_cylinder", // name
			      0,                              // inner radius
			      containerRadius,                // outer radius
			      length*0.5,                     // half length
			      0,                              // rotation start angle
			      CLHEP::twopi);                  // rotation finish angle

  //container cylindrical solid (circular cross-section)
  containerSubtractionSolid = new G4Tubs(name + "_subtraction_cylinder", // name
					 0,                                // inner radius
					 containerRadius + lengthSafety,   // outer radius
					 2*length,                         // long length for unambiguous subtraction
					 0,                                // rotation start angle
					 CLHEP::twopi);                    // rotation finish angle
  
  return CommonFinalConstruction(name, vacuumMaterial, beamPipeMaterial, length, containerRadius);
}

BDSBeamPipe* BDSBeamPipeFactoryLHCDetailed::CreateBeamPipe(const G4String&      name,
							   G4double             length,
							   const G4ThreeVector& inputFaceNormalIn,
							   const G4ThreeVector& outputFaceNormalIn,
							   G4double      aper1,
							   G4double      aper2,
							   G4double      aper3,
							   G4double      /*aper4*/,
							   G4Material*   vacuumMaterial,
							   G4double      beamPipeThickness,
							   G4Material*   beamPipeMaterial,
							   const G4String& /*pointsFileIn*/,
							   const G4String& /*pointsUnitIn*/)
{
  // clean up after last usage
  CleanUp();

  // calculate geometrical parameters
  CalculateGeometricalParameters(aper1, aper2, aper3, beamPipeThickness, length);
  
  inputFaceNormal  = inputFaceNormalIn;
  outputFaceNormal = outputFaceNormalIn;
  
  G4double contRadius = CreateGeneralAngledSolids(name, length, inputFaceNormal,
						  outputFaceNormal);
  
  return CommonFinalConstruction(name, vacuumMaterial, beamPipeMaterial,
				 length, contRadius);
}

BDSBeamPipe* BDSBeamPipeFactoryLHCDetailed::CommonFinalConstruction(const G4String& name,
								    G4Material* vacuumMaterial,
								    G4Material* beamPipeMaterial,
								    G4double    length,
								    G4double    contRadius)
{
  BDSBeamPipeFactoryBase::CommonConstruction(name, vacuumMaterial,
					     beamPipeMaterial, length);
		    
  // record extents
  BDSExtent ext = BDSExtent(contRadius, contRadius, length*0.5);
  
  // build the BDSBeamPipe instance and return it
  BDSBeamPipe* aPipe = BuildBeamPipeAndRegisterVolumes(ext,contRadius);
  
  // register sensitive volumes
  if (sensitiveBeamPipe)
    {
      aPipe->RegisterSensitiveVolume(screenLV, BDSSDType::energydep);
      aPipe->RegisterSensitiveVolume(copperSkinLV, BDSSDType::energydep);
      if (buildCoolingPipe)
	{aPipe->RegisterSensitiveVolume(coolingPipeLV, BDSSDType::energydep);}
    }
  
  return aPipe;
}

void BDSBeamPipeFactoryLHCDetailed::BuildLogicalVolumes(const G4String& name,
							G4Material* vacuumMaterialIn,
							G4Material* beamPipeMaterialIn)
{
  BDSBeamPipeFactoryBase::BuildLogicalVolumes(name,vacuumMaterialIn,beamPipeMaterialIn);

  // get materials
  G4Material* copper2k = BDSMaterials::Instance()->GetMaterial("cu_2k");
  
  copperSkinLV  = new G4LogicalVolume(copperSkinSolid,
                                      copper2k,
				      name + "_copper_lv");
  
  screenLV      = new G4LogicalVolume(screenSolid,
				      beamPipeMaterialIn,
				      name + "_screen_lv");

  if (buildCoolingPipe)
    {
      coolingPipeLV = new G4LogicalVolume(coolingPipeSolid,
					  beamPipeMaterialIn,
					  name + "_cooling_pipe_lv");
      allLogicalVolumes.insert(coolingPipeLV);
    }
  allLogicalVolumes.insert(copperSkinLV);
  allLogicalVolumes.insert(screenLV);
}

void BDSBeamPipeFactoryLHCDetailed::SetVisAttributes(G4Material* beamPipeMaterialIn,
                                                     G4Material* vacuumMateiralIn)
{
  BDSBeamPipeFactoryBase::SetVisAttributes(beamPipeMaterialIn, vacuumMateiralIn);

  // copper skin
  G4VisAttributes* cuVisAttr   = new G4VisAttributes(*BDSColours::Instance()->GetColour("LHCcopperskin"));
  cuVisAttr->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  cuVisAttr->SetVisibility(true);
  allVisAttributes.insert(cuVisAttr);
  
  // beampipe
  G4VisAttributes* pipeVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("beampipe"));
  pipeVisAttr->SetVisibility(true);
  pipeVisAttr->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  allVisAttributes.insert(pipeVisAttr);

  copperSkinLV->SetVisAttributes(cuVisAttr);
  screenLV->SetVisAttributes(pipeVisAttr);
  if (buildCoolingPipe)
    {coolingPipeLV->SetVisAttributes(pipeVisAttr);}
}

void BDSBeamPipeFactoryLHCDetailed::SetUserLimits(G4double length)
{
  BDSBeamPipeFactoryBase::SetUserLimits(length);
  // get the user limits from the vacuum volume
  // ownership registration done in base class method so no need to register it
  auto ul = vacuumLV->GetUserLimits();

  copperSkinLV->SetUserLimits(ul);
  screenLV->SetUserLimits(ul);
  if (buildCoolingPipe)
    {coolingPipeLV->SetUserLimits(ul);}
}

void BDSBeamPipeFactoryLHCDetailed::PlaceComponents(const G4String& name)
{
  BDSBeamPipeFactoryBase::PlaceComponents(name);

  copperSkinPV = new G4PVPlacement(nullptr,                      // no rotation
				   G4ThreeVector(),              // position
				   copperSkinLV,                 // lv to be placed
				   name + "_copper_skin_pv",     // name
				   containerLV,                  // mother lv to be placed in
				   false,                        // no boolean operation
				   0,                            // copy number
				   checkOverlaps);               // whether to check overlaps
  
  screenPV = new G4PVPlacement(nullptr,               // no rotation
			       G4ThreeVector(),       // position
			       screenLV,              // lv to be placed
			       name + "_screen_pv",   // name
			       containerLV,           // mother lv to be placed in
			       false,                 // no boolean operation
			       0,                     // copy number
			       checkOverlaps);        // whether to check overlaps

  allPhysicalVolumes.insert(copperSkinPV);
  allPhysicalVolumes.insert(screenPV);
  
  if (buildCoolingPipe)
    {
      G4ThreeVector coolingPipeTopPosition;
      G4ThreeVector coolingPipeBottomPosition;
      if (!verticalOrientation)
	{ // horizontal shape -> vertical displacement for pipes
	  coolingPipeTopPosition    = G4ThreeVector(0,coolingPipeOffset,0);
	  coolingPipeBottomPosition = G4ThreeVector(0,-coolingPipeOffset,0);
	}
      else
	{ // vertical shape -> horizontal displacement for pipes
	  coolingPipeTopPosition    = G4ThreeVector(coolingPipeOffset,0,0);
	  coolingPipeBottomPosition = G4ThreeVector(-coolingPipeOffset,0,0);
	}
  
      coolingPipeTopPV = new G4PVPlacement(nullptr,   // no rotation
					   coolingPipeTopPosition,       // position
					   coolingPipeLV,                // lv to be placed
					   name + "_cooling_pipe_top_pv",// name
					   containerLV,                  // mother lv to be placed in
					   false,                        // no boolean operation
					   0,                            // copy number
					   checkOverlaps);               // whether to check overlaps

      coolingPipeBottomPV = new G4PVPlacement(nullptr,   // no rotation
					      coolingPipeBottomPosition,    // position
					      coolingPipeLV,                // lv to be placed
					      name + "_cooling_pipe_bottom_pv", // name
					      containerLV,                  // mother lv to be placed in
					      false,                        // no boolean operation
					      0,                            // copy number
					      checkOverlaps);               // whether to check overlaps

      allPhysicalVolumes.insert(coolingPipeTopPV);
      allPhysicalVolumes.insert(coolingPipeBottomPV);
    }
}
  
/// the angled ones have degeneracy in the geant4 solids they used so we can avoid code duplication
/// by grouping common construction tasks
G4double BDSBeamPipeFactoryLHCDetailed::CreateGeneralAngledSolids(const G4String&      name,
								  G4double             length,
								  const G4ThreeVector& inputface,
								  const G4ThreeVector& outputface)
{ 
  // build the solids
  //vacuum cylindrical solid (circular cross-section)
  G4VSolid* vacCylSolid = new G4CutTubs(name + "_vacuum_cylinder",   // name
					0,                           // inner radius
					vacRadius,                   // outer radius
					vacHalfLength,               // half length
					0,                           // rotation start angle
					CLHEP::twopi,                // rotation finish angle
					inputface,                   // input face normal
					outputface);                 // output face normal
  //vacuum box solid (rectangular cross-section)
  G4VSolid* vacRectSolid = new G4Box(name + "_vacuum_box", // name
				     vacBoxX,              // x half width
				     vacBoxY,              // y half width
				     length); // z full width (long for unambiguous intersection)
  //intersection of both of these gives the desired shape
  vacuumSolid = new G4IntersectionSolid(name + "_vacuum_solid", // name
					vacCylSolid,            // solid 1
					vacRectSolid);          // solid 2
  allSolids.insert(vacCylSolid);
  allSolids.insert(vacRectSolid);

  //copper skin layer to beamscreen
  //copper skin inner edge for subtraction (actually just like vacuum + lengthSafety)
  G4VSolid* cuInnerCylSolid = new G4CutTubs(name + "_cu_inner_cylinder",   // name
					    0,                             // inner radius
					    cuInnerRadius,                 // outer radius
					    1.5*length, // length big for unambiguous subtraction (but < outerlength)
					    0,                             // rotation start angle
					    CLHEP::twopi,                  // rotation finish angle
					    inputface,                     // input face normal
					    outputface);                   // output face normal
  //screen inner edge box solid (rectangular cross-section)
  G4VSolid* cuInnerRectSolid = new G4Box(name + "_cu_inner_box", // name
					 cuInnerBoxX,            // x half width
					 cuInnerBoxY,            // y half width
					 1.7*length);            // z long for unambiguous intersection
  //screen inner intersection - 1.5*length long which is > half length for unambiguous subtraction later
  G4VSolid* cuInnerSolid = new G4IntersectionSolid(name + "_cu_inner_solid", // name
						   cuInnerCylSolid,            // solid 1
						   cuInnerRectSolid);          // solid 2

  //screen outer edge for subtraction (actually just like vacuum + lengthSafety)x
  G4VSolid* cuOuterCylSolid = new G4CutTubs(name + "_cu_outer_cylinder",  // name
					    0,                            // inner radius (0 for unambiguous subtraction)
					    cuOuterRadius,                // outer radius
					    halfLength,                   // half length
					    0,                            // rotation start angle
					    CLHEP::twopi,                 // rotation finish angle
					    inputface,                    // input face normal
					    outputface);                  // output face normal
  //screen outer edge box solid (rectangular cross-section)
  G4VSolid* cuOuterRectSolid = new G4Box(name + "_cu_outer_box", // name
					 cuOuterBoxX,            // x half width
					 cuOuterBoxY,            // y half width
					 length); // z full width (long for unambiguous intersection)
  G4VSolid* cuOuterSolid = new G4IntersectionSolid(name + "_cu_outer_solid", // name
						   cuOuterCylSolid,          // solid 1
						   cuOuterRectSolid);        // solid 2
  copperSkinSolid = new G4SubtractionSolid(name + "_copper_skin_solid",// name
					   cuOuterSolid,               // this
					   cuInnerSolid);              // minus this
  allSolids.insert(cuInnerCylSolid);
  allSolids.insert(cuInnerRectSolid);
  allSolids.insert(cuInnerSolid);
  allSolids.insert(cuOuterCylSolid);
  allSolids.insert(cuOuterRectSolid);
  allSolids.insert(cuOuterSolid);
  allSolids.insert(copperSkinSolid);
  
  //beampipe cylindrical solid (circular cross-section)
  //beampipe inner edge for subtraction (actually just like vacuum + lengthSafety)
  G4VSolid* screenInnerCylSolid = new G4CutTubs(name + "_screen_inner_cylinder", // name
						0,                               // inner radius
						bsInnerRadius,                   // outer radius
						1.5*length, // length big for unambiguous subtraction (but < outerlength)
						0,                               // rotation start angle
						CLHEP::twopi,                    // rotation finish angle
						inputface,                       // input face normal
						outputface);                     // output face normal
  //screen inner edge box solid (rectangular cross-section)
  G4VSolid* screenInnerRectSolid = new G4Box(name + "_screen_inner_box", // name
					     bsInnerBoxX,                // x half width
					     bsInnerBoxY,                // y half width
					     1.7*length); // z long for unambiguous intersection
  //screen inner intersection - 1.5*length long which is > half length for unambiguous subtraction later
  G4VSolid* screenInnerSolid = new G4IntersectionSolid(name + "_screen_inner_solid", // name
						       screenInnerCylSolid,          // solid 1
						       screenInnerRectSolid);        // solid 2

  //screen outer edge for subtraction (actually just like vacuum + lengthSafety)x
  G4VSolid* screenOuterCylSolid = new G4CutTubs(name + "_screen_outer_cylinder",// name
						0,       // inner radius (0 for unambiguous subtraction)
						bsOuterRadius,                  // outer radius
						halfLength,                     // half length
						0,                              // rotation start angle
						CLHEP::twopi,                   // rotation finish angle
						inputface,                      // input face normal
						outputface);                    // output face normal
  //screen outer edge box solid (rectangular cross-section)
  G4VSolid* screenOuterRectSolid = new G4Box(name + "_screen_outer_box", // name
					     bsOuterBoxX,                // x half width
					     bsOuterBoxY,                // y half width
					     length); // z full width (long for unambiguous intersection)
  G4VSolid* screenOuterSolid = new G4IntersectionSolid(name + "_pipe_outer_solid", // name
						       screenOuterCylSolid,        // solid 1
						       screenOuterRectSolid);      // solid 2
  
  screenSolid = new G4SubtractionSolid(name + "_screen_solid",// name
				       screenOuterSolid,      // this
				       screenInnerSolid);     // minus this

  allSolids.insert(screenInnerCylSolid);
  allSolids.insert(screenInnerRectSolid);
  allSolids.insert(screenInnerSolid);
  allSolids.insert(screenOuterCylSolid);
  allSolids.insert(screenOuterRectSolid);
  allSolids.insert(screenOuterSolid);
  allSolids.insert(screenSolid);

  if (buildCoolingPipe)
    {
      coolingPipeSolid = new G4CutTubs(name + "_cooling_pipe_solid",             // name
				       coolingPipeRadius,                        // inner radius
				       coolingPipeRadius + coolingPipeThickness, // outer radius
				       halfLength,                               // half length
				       0,                                        // rotation start angle
				       CLHEP::twopi,                             // rotation finish angle
				       inputface,                                // input face normal
				       outputface);                              // output face normal
      
      allSolids.insert(coolingPipeSolid);
    }
  
  // beampipe - ("coldbore") circular cross-section and sits outisde screen and cooling pipe
  beamPipeSolid = new G4CutTubs(name + "_beampipe_solid", // name
				cbInnerRadius,            // inner radius
				cbOuterRadius,            // outer radius
				halfLength,               // half length
				0,                        // rotation start angle
				CLHEP::twopi,             // rotation finish angle
				inputface,                // input face normal
				outputface);              // output face normal

  //container cylindrical solid (circular cross-section)
  // 'length' is correct full length of object
  containerSolid = new G4CutTubs(name + "_container_cylinder", // name
				 0,                            // inner radius
				 containerRadius,              // outer radius
				 length*0.5,                   // half length
				 0,                            // rotation start angle
				 CLHEP::twopi,                 // rotation finish angle
				 inputface,                    // input face normal
				 outputface);                  // output face normal

  //container cylindrical solid (circular cross-section)
  containerSubtractionSolid = new G4Tubs(name + "_subtraction_cylinder", // name
					 0,                              // inner radius
					 containerRadius + lengthSafety, // outer radius
					 2*length,                       // long length for unambiguous subtraction
					 0,                              // rotation start angle
					 CLHEP::twopi);                  // rotation finish angle

  // return the container radius as everything is calculated in this function
  return containerRadius;
}
