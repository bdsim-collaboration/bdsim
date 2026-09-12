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
#include "BDSAppropriateTubs.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBeamPipeType.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterFactoryCylindrical.hh"
#include "BDSMagnetOuterFactoryLHC.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMaterials.hh"
#include "BDSSDType.hh"
#include "BDSUtilities.hh"                 // for calculateorientation

#include "globals.hh"                      // geant4 globals / types
//#include "G4Box.hh"
#include "G4CutTubs.hh"
//#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4UnionSolid.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <utility>
#include <vector>

/// Beam pipe axis separation at 1.9K
const G4double BDSMagnetOuterFactoryLHC::beamSeparation = 194.00*CLHEP::mm;

BDSMagnetOuterFactoryLHC::BDSMagnetOuterFactoryLHC(G4bool isLeftOffsetIn):
  isLeftOffset(isLeftOffsetIn)
{
  cylindrical = new BDSMagnetOuterFactoryCylindrical();
}

BDSMagnetOuterFactoryLHC::~BDSMagnetOuterFactoryLHC()
{
  delete cylindrical;
}

BDSMagnetOuter* BDSMagnetOuterFactoryLHC::CreateSectorBend(G4String           name,
                                                           G4double           length,
                                                           const BDSBeamPipe* beamPipe,
                                                           G4double           containerLength,
                                                           const BDSMagnetOuterInfo* recipe)
{
  return CreateLHCDipole(name, length, beamPipe, containerLength, recipe, YokeColour::dipole);
}

BDSMagnetOuter* BDSMagnetOuterFactoryLHC::CreateLHCDipole(const G4String&    name,
                                                          G4double           length,
                                                          const BDSBeamPipe* beamPipe,
                                                          G4double           containerLength,
                                                          const BDSMagnetOuterInfo* recipe,
                                                          YokeColour         colourIn)
{
  CleanUp();

  G4double horizontalWidth  = recipe->horizontalWidth;
  G4double angleIn          = recipe->angleIn;
  G4double angleOut         = recipe->angleOut;
  
  G4bool flatFaces = (!BDS::IsFinite(angleIn)) && (!BDS::IsFinite(angleOut));
  
  // note this geometry does not respond to horizontalWidth - it's hard coded to the
  // design of a sector bend for the lhc.  TestInputParameters requires it though
  // to be the same check for the other methods

  // test input parameters - set global options as default if not specified
  TestInputParameters(beamPipe,horizontalWidth);

  // nominal lhc beampipe parameters for reference
  // aper1 = 4.404cm / 2
  // aper2 = 3.428cm / 2
  // aper3 = 4.404cm / 2
  // containerRadius -> 24.599mm for lhc beampipe with these parameters
  
  // geometrical constants
  // [1] LHC design report - Chapter 7, fig 7.3
  // [2] LHC design report - Chapter 7, fig 7.1
  G4double beamPipeAxisSeparation = beamSeparation;
  G4double massShift              = 0.5 * beamPipeAxisSeparation; // central shift to geometry
  //G4double collarBoxHalfHeight    = 60*CLHEP::mm;                 // [1] by visual inspection
  G4double collarBoxHalfWidth     = 22*CLHEP::mm;                 // fits between outer coils

  // radii
  G4double containerInnerRadius   = beamPipe->GetContainerRadius()+lengthSafetyLarge;
  G4double innerCoilInnerRadius   = containerInnerRadius + lengthSafetyLarge;
  G4double innerCoilInnerRadiusF  = 24.601*CLHEP::mm; // the fixed radius for the second pipe - F for fixed
  G4double innerCoilOuterRadius   = 42*CLHEP::mm;                        // [2] by visual inspection
  G4double outerCoilInnerRadius   = innerCoilOuterRadius + lengthSafetyLarge;
  G4double outerCoilInnerRadiusF  = innerCoilOuterRadius + lengthSafetyLarge; // doesn't change
  G4double outerCoilOuterRadius   = 60*CLHEP::mm;                        // [2] by visual inspection
  G4double collarInnerRadius      = outerCoilOuterRadius + lengthSafetyLarge;
  G4double collarInnerRadiusF     = outerCoilOuterRadius + lengthSafetyLarge;
  G4double collarOuterRadius      = 101.18*CLHEP::mm;                    // [1] - at 293K but don't have 1.9K measurement
  G4double yokeOuterRadius        = 570.0*0.5*CLHEP::mm;                 // [2] - at 293K but don't have 1.9K measurement
  G4double magnetContainerRadius  = yokeOuterRadius + lengthSafetyLarge;

  // angular setup of coils
  // these angles were calculated by visually analysing the coil layout graph in [2]
  G4double poleInnerFullAngle = 33./180.*2; //33 degrees half angle in radians
  G4double poleOuterFullAngle = 100./180.*2; //80 degrees half angle in radians
  G4double coilInnerFullAngle = CLHEP::pi - poleInnerFullAngle - 1e-3; // 1e-3 is small margin to avoid overlap
  G4double coilOuterFullAngle = CLHEP::pi - poleOuterFullAngle - 1e-3;

  // whether to build various components around active beam pipe depending on how wide it is
  // these ONLY apply to the components around the active beampipe
  G4bool buildInnerCoil           = true;
  G4bool buildOuterCoil           = true;
  G4bool buildCollar              = true; // collar around the active beam pipe
  if (innerCoilInnerRadius > innerCoilOuterRadius)
    {buildInnerCoil = false;}
  if ((innerCoilInnerRadius > outerCoilInnerRadius) && (innerCoilInnerRadius < outerCoilOuterRadius))
    {outerCoilInnerRadius = containerInnerRadius + lengthSafety;}
  if (innerCoilInnerRadius > outerCoilOuterRadius)
    {buildOuterCoil = false;}
  // this still uses the boxHalfWidth but just as good as the collar annulli overlap slightly in the middle
  // and this will protect against this
  if ((innerCoilInnerRadius > collarInnerRadius) && (innerCoilInnerRadius < (massShift - collarBoxHalfWidth)))
    {collarInnerRadius = containerInnerRadius + lengthSafety;}
  if (innerCoilInnerRadius > (massShift - collarBoxHalfWidth))
    {buildCollar = false;}
  if (innerCoilInnerRadius > collarOuterRadius)
    {// pipe is too big to use with this geometry!
      throw BDSException(__METHOD_NAME__, "error in component \"" + name + "\"\n" +
			 "this beam pipe is too big to use with the LHC dipole geometry\n" +
			 "Please consider using a different magnet geometry for this particular magnet");
    }

  G4ThreeVector dipolePosition; // translation of whole assembly relative to centre of active pipe
  if (isLeftOffset)
    {
      dipolePosition = G4ThreeVector(massShift,0.,0.);
      beamPipeAxisSeparation  *= -1;
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "dipole to the left" << G4endl;
#endif
    }
  else
    {
      dipolePosition = G4ThreeVector(-massShift,0.,0.);
      //massShift     *= -1;
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "dipole to the right" << G4endl;
#endif
    }

  // calculate some geometrical parameters
  G4double angle    = angleIn + angleOut; // total angle
  G4int orientation = BDS::CalculateOrientation(-angle);
  std::pair<G4ThreeVector,G4ThreeVector> faces = BDS::CalculateFaces(angleIn,angleOut);
  inputFaceNormal  = faces.first;
  outputFaceNormal = faces.second;

  // lengths at different points transversely - dependent on left or right geometry as well as angle +ve or -ve
  G4double centralHalfLength      = length*0.5 - orientation*0.5*beamPipeAxisSeparation*tan(fabs(angle*0.5)); // central axis of outer cylinder
  // note container length is defined along the main beam axis - here our container is offset so the container length is also slightly different
  G4double centralContainerLength = containerLength - orientation*beamPipeAxisSeparation*tan(fabs(angle*0.5)); // central axis of outer cylinder
  //G4double centralContainerLength = 2*(centralHalfLength + lengthSafetyLarge) + 10*CLHEP::mm;
  G4double secondBPHalfLength     = length*0.5 - orientation*beamPipeAxisSeparation*tan(fabs(angle*0.5));     // central axis of second beampipe

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "all calculated parameters: " << G4endl;
  G4cout << "container inner radius:  " << containerInnerRadius << G4endl;
  G4cout << "inner coil inner radius: " << innerCoilInnerRadius << G4endl;
  G4cout << "inner coil outer radius: " << innerCoilOuterRadius << G4endl;
  G4cout << "outer coil inner radius: " << outerCoilInnerRadius << G4endl;
  G4cout << "outer coil outer radius: " << outerCoilOuterRadius << G4endl;
  G4cout << "collar inner radius:     " << collarInnerRadius    << G4endl;
  G4cout << "collar outer radius:     " << collarOuterRadius    << G4endl;
  G4cout << "yoke outer radius:       " << yokeOuterRadius      << G4endl;
#endif
  
  G4VSolid* containerSolidOuter = BDS::AppropriateTubs(name + "_container_solid_outer",
						       0,                           // inner radius
						       yokeOuterRadius,             // outer radius
						       centralHalfLength,           // half length
						       0,                           // rotation start angle
						       CLHEP::twopi,
						       inputFaceNormal,             // input face normal
						       outputFaceNormal,
						       flatFaces);
  allSolids.insert(containerSolidOuter);
  G4VSolid* containerSolidInner;
  if (beamPipe->ContainerIsCircular())
    {
      //circular beampipe so we can simply use its radius
      //container is similar but slightly wider and hollow (to allow placement of beampipe)
      //have to do subtraction as cuttubs aperture is central and the beampipe (active one) is not here
      containerSolidInner = new G4Tubs(name + "_container_solid_inner",  // name
				       0,                                 // inner radius
				       containerInnerRadius,              // outer radius
				       length,                            // full length for unambiguous subtraction
				       0,                                 // rotation start angle
				       CLHEP::twopi);
      allSolids.insert(containerSolidInner);
    }
  else
    {containerSolidInner = beamPipe->GetContainerSubtractionSolid();}
  containerSolid = new G4SubtractionSolid(name + "_outer_container_solid",// name
					  containerSolidOuter,            // outer bit
					  containerSolidInner,            // subtract this from it
					  nullptr,                        // rotation
					  -dipolePosition);               // translation
  
  // make the whole magnet container solid
  BuildMagnetContainerSolidAngled(name, centralContainerLength, magnetContainerRadius, flatFaces);
  // make the logical volume too manually as we don't use the BDSMagnetOuterFactoryBase method for this
  
  // Unlike other magnets, our container is tight-fitting so we fill with empty vacuum
  G4Material* worldMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->EmptyMaterial());
  magnetContainerLV = new G4LogicalVolume(magnetContainerSolid,
					  worldMaterial,
					  name + "_container_lv");
  
  containerLV = new G4LogicalVolume(containerSolid,
				    worldMaterial,
				    name + "_outer_container_lv");
    
  // coil solids
  G4VSolid*        coil1Inner   = nullptr;
  G4VSolid*        coil1Outer   = nullptr;
  G4VSolid*        coil2Inner   = nullptr;
  G4VSolid*        coil2Outer   = nullptr;
  G4VSolid*        coil3Inner   = nullptr;
  G4VSolid*        coil3Outer   = nullptr;
  G4VSolid*        coil4Inner   = nullptr;
  G4VSolid*        coil4Outer   = nullptr;
  G4LogicalVolume* coil1InnerLV = nullptr;
  G4LogicalVolume* coil1OuterLV = nullptr;
  G4LogicalVolume* coil2InnerLV = nullptr;
  G4LogicalVolume* coil2OuterLV = nullptr;
  G4LogicalVolume* coil3InnerLV = nullptr;
  G4LogicalVolume* coil3OuterLV = nullptr;
  G4LogicalVolume* coil4InnerLV = nullptr;
  G4LogicalVolume* coil4OuterLV = nullptr;
  G4PVPlacement*   coil1InnerPV = nullptr;
  G4PVPlacement*   coil1OuterPV = nullptr;
  G4PVPlacement*   coil2InnerPV = nullptr;
  G4PVPlacement*   coil2OuterPV = nullptr;
  G4PVPlacement*   coil3InnerPV = nullptr;
  G4PVPlacement*   coil3OuterPV = nullptr;
  G4PVPlacement*   coil4InnerPV = nullptr;
  G4PVPlacement*   coil4OuterPV = nullptr;
  
  G4Material*      nbti                    = BDSMaterials::Instance()->GetMaterial("NbTi.1");
  G4Material*      stainlesssteel_316LN_2K = BDSMaterials::Instance()->GetMaterial("stainless_steel_316LN_2K");
  G4Material*      stainlesssteel_304L_2K  = BDSMaterials::Instance()->GetMaterial("stainless_steel_304L_2K");
  
  G4VisAttributes* coilVisAtt   = new G4VisAttributes(*BDSColours::Instance()->GetColour("LHCcoil"));
  coilVisAtt->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  allVisAttributes.insert(coilVisAtt);
  G4VSolid*        collar1PoleTopInnerSolid     = nullptr;
  G4VSolid*        collar1PoleBottomInnerSolid  = nullptr;
  G4VSolid*        collar1PoleTopOuterSolid     = nullptr;
  G4VSolid*        collar1PoleBottomOuterSolid  = nullptr;
  G4LogicalVolume* collar1PoleTopInnerLV        = nullptr;
  G4LogicalVolume* collar1PoleBottomInnerLV     = nullptr;
  G4LogicalVolume* collar1PoleTopOuterLV        = nullptr;
  G4LogicalVolume* collar1PoleBottomOuterLV     = nullptr;
  G4PVPlacement*   collar1PoleTopInnerPV        = nullptr;
  G4PVPlacement*   collar1PoleBottomInnerPV     = nullptr;
  G4PVPlacement*   collar1PoleTopOuterPV        = nullptr;
  G4PVPlacement*   collar1PoleBottomOuterPV     = nullptr;
  G4VSolid*        collar2PoleTopInnerSolid     = nullptr;
  G4VSolid*        collar2PoleBottomInnerSolid  = nullptr;
  G4VSolid*        collar2PoleTopOuterSolid     = nullptr;
  G4VSolid*        collar2PoleBottomOuterSolid  = nullptr;
  G4LogicalVolume* collar2PoleTopInnerLV        = nullptr;
  G4LogicalVolume* collar2PoleBottomInnerLV     = nullptr;
  G4LogicalVolume* collar2PoleTopOuterLV        = nullptr;
  G4LogicalVolume* collar2PoleBottomOuterLV     = nullptr;
  G4PVPlacement*   collar2PoleTopInnerPV        = nullptr;
  G4PVPlacement*   collar2PoleBottomInnerPV     = nullptr;
  G4PVPlacement*   collar2PoleTopOuterPV        = nullptr;
  G4PVPlacement*   collar2PoleBottomOuterPV     = nullptr;
  G4LogicalVolume* collarsLV                    = nullptr;
  G4PVPlacement*   collarsPV                    = nullptr;
  G4LogicalVolume* secondBPLV                   = nullptr;
  G4PVPlacement*   secondBPPV                   = nullptr;

  // use base class yokeLV and yokePV members (reset in CleanUp())
  
  G4VisAttributes* collarVisAtt = new G4VisAttributes(*BDSColours::Instance()->GetColour("LHCcollar"));
  collarVisAtt->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  allVisAttributes.insert(collarVisAtt);

  //buildInnerCoil = false;
  if (buildInnerCoil)
    {
      coil1Inner = BDS::AppropriateTubs(name+"_coil1_inner_solid",                // name
					innerCoilInnerRadius + lengthSafetyLarge, // inner radius
					innerCoilOuterRadius - lengthSafetyLarge, // outer radius
					length*0.5 - lengthSafetyLarge,           // length
					-coilInnerFullAngle*0.5,                  // start angle
					coilInnerFullAngle,                       // sweep angle
					inputFaceNormal,                          // input face normal
					outputFaceNormal,                         // output face normal
					flatFaces);
      coil2Inner = BDS::AppropriateTubs(name+"_coil2_inner_solid",                // name
					innerCoilInnerRadius + lengthSafetyLarge, // inner radius
					innerCoilOuterRadius - lengthSafetyLarge, // outer radius
					length*0.5 - lengthSafetyLarge,           // length
					CLHEP::pi-coilInnerFullAngle*0.5,         // start angle
					coilInnerFullAngle,                       // sweep angle
					inputFaceNormal,                          // input face normal
					outputFaceNormal,                         // output face normal
					flatFaces);
      coil1InnerLV =  new G4LogicalVolume(coil1Inner,
					  nbti,
					  name+"_coil1_Inner_lv");
      coil2InnerLV =  new G4LogicalVolume(coil2Inner,
					  nbti,
					  name+"_coil2_Inner_lv");
      coil1InnerLV->SetVisAttributes(coilVisAtt);
      coil2InnerLV->SetVisAttributes(coilVisAtt);

      coil1InnerPV = new G4PVPlacement(nullptr,                      // rotation
				       -dipolePosition,        // position
				       coil1InnerLV,           // its logical volume
				       name+"_coil1_inner_pv", // its name
				       containerLV,            // its mother  volume
				       false,                  // no boolean operation
				       0, 
				       checkOverlaps);
      coil2InnerPV = new G4PVPlacement(nullptr,                      // rotation
				       -dipolePosition,        // position
				       coil2InnerLV,           // its logical volume
				       name+"_coil2_inner_pv", // its name
				       containerLV,            // its mother  volume
				       false,                  // no boolean operation
				       0, 
				       checkOverlaps);

      allSolids.insert(coil1Inner);
      allSolids.insert(coil2Inner);
      allLogicalVolumes.insert(coil1InnerLV);
      allLogicalVolumes.insert(coil2InnerLV);
      allPhysicalVolumes.insert(coil1InnerPV);
      allPhysicalVolumes.insert(coil2InnerPV);

      collar1PoleTopInnerSolid    = BDS::AppropriateTubs(name+"_collar1_pole_inner_top",           // name
							 innerCoilInnerRadius + lengthSafetyLarge, // inner radius
							 innerCoilOuterRadius - lengthSafetyLarge, // outer radius
							 length*0.5 - lengthSafetyLarge,           // length
							 CLHEP::pi*0.5-poleInnerFullAngle*0.5,     // start angle
							 poleInnerFullAngle,                       // sweep angle
							 inputFaceNormal,                          // input face normal
							 outputFaceNormal,                         // output face normal
							 flatFaces);
      collar1PoleBottomInnerSolid = BDS::AppropriateTubs(name+"_collar1_pole_inner_bottom",        // name
							 innerCoilInnerRadius + lengthSafetyLarge, // inner radius
							 innerCoilOuterRadius - lengthSafetyLarge, // outer radius
							 length*0.5 - lengthSafetyLarge,           // length
							 CLHEP::pi*1.5-poleInnerFullAngle*0.5,     // start angle
							 poleInnerFullAngle,                       // sweep angle
							 inputFaceNormal,                          // input face normal
							 outputFaceNormal,                         // output face normal
							 flatFaces);
      collar1PoleTopInnerLV    = new G4LogicalVolume(collar1PoleTopInnerSolid,
                                                     stainlesssteel_316LN_2K,
						     name+"_collar1_pole_top_inner_lv");
      collar1PoleBottomInnerLV = new G4LogicalVolume(collar1PoleBottomInnerSolid,
                                                     stainlesssteel_316LN_2K,
						     name+"_collar1_pole_bottom_inner_lv");

      collar1PoleTopInnerLV->SetVisAttributes(collarVisAtt);
      collar1PoleBottomInnerLV->SetVisAttributes(collarVisAtt);

      collar1PoleTopInnerPV = new G4PVPlacement(nullptr,                          // rotation
						-dipolePosition,            // position
						collar1PoleTopInnerLV,      // its logical volume
						name+"_collar1_pole_top_inner_pv",// its name
						containerLV,                // its mother  volume
						false,                      // no boolean operation
						0,
						checkOverlaps);
      collar1PoleBottomInnerPV = new G4PVPlacement(nullptr,                          // rotation
						   -dipolePosition,            // position
						   collar1PoleBottomInnerLV,   // its logical volume
						   name+"_collar1_pole_top_inner_pv",// its name
						   containerLV,                // its mother  volume
						   false,                      // no boolean operation
						   0,
						   checkOverlaps);

      allSolids.insert(collar1PoleTopInnerSolid);
      allSolids.insert(collar1PoleBottomInnerSolid);
      allLogicalVolumes.insert(collar1PoleTopInnerLV);
      allLogicalVolumes.insert(collar1PoleBottomInnerLV);
      allPhysicalVolumes.insert(collar1PoleTopInnerPV);
      allPhysicalVolumes.insert(collar1PoleBottomInnerPV);
    }
  
  if (buildOuterCoil)
    {
      coil1Outer = BDS::AppropriateTubs(name+"_coil1_outer_solid",            // name
					outerCoilInnerRadius,                 // inner radius
					outerCoilOuterRadius,                 // outer radius
					length*0.5 - lengthSafetyLarge,       // length
					-coilOuterFullAngle*0.5,              // start angle
					coilOuterFullAngle,                   // sweep angle
					inputFaceNormal,                      // input face normal
					outputFaceNormal,                     // output face normal
					flatFaces);
      coil2Outer = BDS::AppropriateTubs(name+"_coil2_outer_solid",            // name
					outerCoilInnerRadius,                 // inner radius
					outerCoilOuterRadius,                 // outer radius
					length*0.5 - lengthSafetyLarge,       // length
					CLHEP::pi-coilOuterFullAngle*0.5,     // start angle
					coilOuterFullAngle,                   // sweep angle
					inputFaceNormal,                      // input face normal
					outputFaceNormal,                     // output face normal
					flatFaces);
      coil1OuterLV =  new G4LogicalVolume(coil1Outer,
					  nbti,
					  name+"_coil1_Inner_lv");
      coil2OuterLV =  new G4LogicalVolume(coil2Outer,
					  nbti,
					  name+"_coil2_Inner_lv");
      coil1OuterLV->SetVisAttributes(coilVisAtt);
      coil2OuterLV->SetVisAttributes(coilVisAtt);
      
      coil1OuterPV = new G4PVPlacement(nullptr,                      // rotation
				       -dipolePosition,        // position
				       coil1OuterLV,           // its logical volume
				       name+"_coil1_outer_pv", // its name
				       containerLV,            // its mother  volume
				       false,                  // no boolean operation
				       0, 
				       checkOverlaps);
      coil2OuterPV = new G4PVPlacement(nullptr,                      // rotation
				       -dipolePosition,        // position
				       coil2OuterLV,           // its logical volume
				       name+"_coil2_outer_pv", // its name
				       containerLV,            // its mother  volume
				       false,                  // no boolean operation
				       0, 
				       checkOverlaps);

      allSolids.insert(coil1Outer);
      allSolids.insert(coil2Outer);
      allLogicalVolumes.insert(coil1OuterLV);
      allLogicalVolumes.insert(coil2OuterLV);
      allPhysicalVolumes.insert(coil1OuterPV);
      allPhysicalVolumes.insert(coil2OuterPV);
      
      collar1PoleTopOuterSolid    = BDS::AppropriateTubs(name+"_collar1_pole_outer_top",      // name
							 outerCoilInnerRadius,                // inner radius
							 outerCoilOuterRadius,                // outer radius
							 length*0.5 - lengthSafetyLarge,      // length
							 CLHEP::pi*0.5-poleOuterFullAngle*0.5,// start angle
							 poleOuterFullAngle,                  // sweep angle
							 inputFaceNormal,                     // input face normal
							 outputFaceNormal,                    // output face normal
							 flatFaces);
      collar1PoleBottomOuterSolid = BDS::AppropriateTubs(name+"_collar1_pole_outer_bottom",   // name
							 outerCoilInnerRadius,                // inner radius
							 outerCoilOuterRadius,                // outer radius
							 length*0.5 - lengthSafetyLarge,      // length
							 CLHEP::pi*1.5-poleOuterFullAngle*0.5,// start angle
							 poleOuterFullAngle,                  // sweep angle
							 inputFaceNormal,                     // input face normal
							 outputFaceNormal,                    // output face normal
							 flatFaces);
      
      collar1PoleTopOuterLV    = new G4LogicalVolume(collar1PoleTopOuterSolid,
                                                     stainlesssteel_316LN_2K,
						     name+"_collar1_pole_top_outer_lv");
      collar1PoleBottomOuterLV = new G4LogicalVolume(collar1PoleBottomOuterSolid,
                                                     stainlesssteel_316LN_2K,
						     name+"_collar1_pole_bottom_outer_lv");
      
      collar1PoleTopOuterLV->SetVisAttributes(collarVisAtt);
      collar1PoleBottomOuterLV->SetVisAttributes(collarVisAtt);
      
      collar1PoleTopOuterPV = new G4PVPlacement(nullptr,                                // rotation
						-dipolePosition,                  // position
						collar1PoleTopOuterLV,            // its logical volume
						name+"_collar1_pole_top_inner_pv",// its name
						containerLV,                      // its mother  volume
						false,                            // no boolean operation
						0,
						checkOverlaps);
      collar1PoleBottomOuterPV = new G4PVPlacement(nullptr,                                // rotation
						   -dipolePosition,                  // position
						   collar1PoleBottomOuterLV,         // its logical volume
						   name+"_collar1_pole_top_inner_pv",// its name
						   containerLV,                      // its mother  volume
						   false,                            // no boolean operation
						   0,
						   checkOverlaps);
						   
      allSolids.insert(collar1PoleTopOuterSolid);
      allSolids.insert(collar1PoleBottomOuterSolid);
      allLogicalVolumes.insert(collar1PoleTopOuterLV);
      allLogicalVolumes.insert(collar1PoleBottomOuterLV);
      allPhysicalVolumes.insert(collar1PoleTopOuterPV);
      allPhysicalVolumes.insert(collar1PoleBottomOuterPV);
    }
  
  // coils on inactive beam pipe - always built
  coil3Inner = BDS::AppropriateTubs(name+"_coil3_inner_solid",              // name
				    innerCoilInnerRadiusF,                  // inner radius
				    innerCoilOuterRadius,                   // outer radius
				    secondBPHalfLength - lengthSafetyLarge, // length
				    -coilInnerFullAngle*0.5,                // start angle
				    coilInnerFullAngle,                     // sweep angle
				    inputFaceNormal,                        // input face normal
				    outputFaceNormal,                      // output face normal
				    flatFaces);
  coil3Outer = BDS::AppropriateTubs(name+"_coil3_outer_solid",              // name
				    outerCoilInnerRadiusF,                  // inner radius
				    outerCoilOuterRadius,                   // outer radius
				    secondBPHalfLength - lengthSafetyLarge, // length
				    -coilOuterFullAngle*0.5,                // start angle
				    coilOuterFullAngle,                     // sweep angle
				    inputFaceNormal,                        // input face normal
				    outputFaceNormal,                       // output face normal
				    flatFaces);
  coil4Inner = BDS::AppropriateTubs(name+"_coil4_inner_solid",              // name
				    innerCoilInnerRadiusF,                  // inner radius
				    innerCoilOuterRadius,                   // outer radius
				    secondBPHalfLength - lengthSafetyLarge, // length
				    CLHEP::pi-coilInnerFullAngle*0.5,       // start angle
				    coilInnerFullAngle,                     // sweep angle
				    inputFaceNormal,                        // input face normal
				    outputFaceNormal,                       // output face normal
				    flatFaces);
  coil4Outer = BDS::AppropriateTubs(name+"_coil4_outer_solid",              // name
				    outerCoilInnerRadiusF,                  // inner radius
				    outerCoilOuterRadius,                   // outer radius
				    secondBPHalfLength - lengthSafetyLarge, // length
				    CLHEP::pi-coilOuterFullAngle*0.5,       // start angle
				    coilOuterFullAngle,                     // sweep angle
				    inputFaceNormal,                        // input face normal
				    outputFaceNormal,                       // output face normal
				    flatFaces);
  
  coil3InnerLV =  new G4LogicalVolume(coil3Inner,
				      nbti,
				      name+"_coil3_Inner_lv");
  coil3OuterLV =  new G4LogicalVolume(coil3Outer,
				      nbti,
				      name+"_coil3_Inner_lv");
  coil4InnerLV =  new G4LogicalVolume(coil4Inner,
				      nbti,
				      name+"_coil4_Inner_lv");
  coil4OuterLV =  new G4LogicalVolume(coil4Outer,
				      nbti,
				      name+"_coil4_Inner_lv");
  
  coil3InnerLV->SetVisAttributes(coilVisAtt);
  coil3OuterLV->SetVisAttributes(coilVisAtt);
  coil4InnerLV->SetVisAttributes(coilVisAtt);
  coil4OuterLV->SetVisAttributes(coilVisAtt);

  allSolids.insert(coil3Inner);
  allSolids.insert(coil3Outer);
  allSolids.insert(coil4Inner);
  allSolids.insert(coil4Outer);
  allLogicalVolumes.insert(coil3InnerLV);
  allLogicalVolumes.insert(coil3OuterLV);
  allLogicalVolumes.insert(coil4InnerLV);
  allLogicalVolumes.insert(coil4OuterLV);

  
  // coil placement  
  coil3InnerPV = new G4PVPlacement(nullptr,                      // rotation
				   dipolePosition,         // position
				   coil3InnerLV,           // its logical volume
				   name+"_coil3_inner_pv", // its name
				   containerLV,            // its mother  volume
				   false,                  // no boolean operation
				   0, 
				   checkOverlaps);
  coil3OuterPV = new G4PVPlacement(nullptr,                      // rotation
				   dipolePosition,         // position
				   coil3OuterLV,           // its logical volume
				   name+"_coil3_outer_pv", // its name
				   containerLV,            // its mother  volume
				   false,                  // no boolean operation
				   0, 
				   checkOverlaps);
  coil4InnerPV = new G4PVPlacement(nullptr,                      // rotation
				   dipolePosition,         // position
				   coil4InnerLV,           // its logical volume
				   name+"_coil4_inner_pv", // its name
				   containerLV,            // its mother  volume
				   false,                  // no boolean operation
				   0, 
				   checkOverlaps);
  coil4OuterPV = new G4PVPlacement(nullptr,                      // rotation
				   dipolePosition,         // position
				   coil4OuterLV,           // its logical volume
				   name+"_coil4_outer_pv", // its name
				   containerLV,            // its mother  volume
				   false,                  // no boolean operation
				   0, 
				   checkOverlaps);
  allPhysicalVolumes.insert(coil3InnerPV);
  allPhysicalVolumes.insert(coil3OuterPV);
  allPhysicalVolumes.insert(coil4InnerPV);
  allPhysicalVolumes.insert(coil4OuterPV);
  
  // non-magnetic collars
  // collar pole solids  
  collar2PoleTopInnerSolid    = BDS::AppropriateTubs(name+"_collar2_pole_inner_top",         // name
						     innerCoilInnerRadiusF,                  // inner radius
						     innerCoilOuterRadius,                   // outer radius
						     secondBPHalfLength - lengthSafetyLarge, // length
						     CLHEP::pi*0.5-poleInnerFullAngle*0.5,   // start angle
						     poleInnerFullAngle,                     // sweep angle
						     inputFaceNormal,                        // input face normal
						     outputFaceNormal,                       // output face normal
						     flatFaces);
  collar2PoleTopOuterSolid    = BDS::AppropriateTubs(name+"_collar2_pole_outer_top",         // name
						     outerCoilInnerRadiusF,                  // inner radius
						     outerCoilOuterRadius,                   // outer radius
						     secondBPHalfLength - lengthSafetyLarge, // length
						     CLHEP::pi*0.5-poleOuterFullAngle*0.5,   // start angle
						     poleOuterFullAngle,                     // sweep angle
						     inputFaceNormal,                        // input face normal
						     outputFaceNormal,                       // output face normal
						     flatFaces);
  collar2PoleBottomInnerSolid = BDS::AppropriateTubs(name+"_collar2_pole_inner_bottom",      // name
						     innerCoilInnerRadiusF,                  // inner radius
						     innerCoilOuterRadius,                   // outer radius
						     secondBPHalfLength - lengthSafetyLarge, // length
						     CLHEP::pi*1.5-poleInnerFullAngle*0.5,   // start angle
						     poleInnerFullAngle,                     // sweep angle
						     inputFaceNormal,                        // input face normal
						     outputFaceNormal,                       // output face normal
						     flatFaces);
  collar2PoleBottomOuterSolid = BDS::AppropriateTubs(name+"_collar2_pole_outer_bottom",      // name
						     outerCoilInnerRadiusF,                  // inner radius
						     outerCoilOuterRadius,                   // outer radius
						     secondBPHalfLength - lengthSafetyLarge, // length
						     CLHEP::pi*1.5-poleOuterFullAngle*0.5,   // start angle
						     poleOuterFullAngle,                     // sweep angle
						     inputFaceNormal,                        // input face normal
						     outputFaceNormal,                       // output face normal
						     flatFaces);
  
  // collar pole logical volumes
  collar2PoleTopInnerLV    = new G4LogicalVolume(collar2PoleTopInnerSolid,
                                                 stainlesssteel_316LN_2K,
						 name+"_collar2_pole_top_inner_lv");
  collar2PoleTopOuterLV    = new G4LogicalVolume(collar2PoleTopOuterSolid,
                                                 stainlesssteel_316LN_2K,
						 name+"_collar2_pole_top_outer_lv");
  collar2PoleBottomInnerLV = new G4LogicalVolume(collar2PoleBottomInnerSolid,
                                                 stainlesssteel_316LN_2K,
						 name+"_collar2_pole_bottom_inner_lv");
  collar2PoleBottomOuterLV = new G4LogicalVolume(collar2PoleBottomOuterSolid,
                                                 stainlesssteel_316LN_2K,
						 name+"_collar2_pole_bottom_outer_lv");
  
  // collar pole visualisation
  collar2PoleTopInnerLV->SetVisAttributes(collarVisAtt);
  collar2PoleTopOuterLV->SetVisAttributes(collarVisAtt);
  collar2PoleBottomInnerLV->SetVisAttributes(collarVisAtt);
  collar2PoleBottomOuterLV->SetVisAttributes(collarVisAtt);

  allSolids.insert(collar2PoleTopInnerSolid);
  allSolids.insert(collar2PoleTopOuterSolid);
  allSolids.insert(collar2PoleBottomInnerSolid);
  allSolids.insert(collar2PoleBottomOuterSolid);
  allLogicalVolumes.insert(collar2PoleTopInnerLV);
  allLogicalVolumes.insert(collar2PoleTopOuterLV);
  allLogicalVolumes.insert(collar2PoleBottomInnerLV);
  allLogicalVolumes.insert(collar2PoleBottomOuterLV);
  
  // collar pole placement
  collar2PoleTopInnerPV = new G4PVPlacement(nullptr,                                // rotation
					    dipolePosition,                   // position
					    collar2PoleTopInnerLV,            // its logical volume
					    name+"_collar2_pole_top_inner_pv",// its name
					    containerLV,                      // its mother  volume
					    false,                            // no boolean operation
					    0,
					    checkOverlaps);
  collar2PoleTopOuterPV = new G4PVPlacement(nullptr,                                // rotation
					    dipolePosition,                   // position
					    collar2PoleTopOuterLV,            // its logical volume
					    name+"_collar2_pole_top_inner_pv",// its name
					    containerLV,                      // its mother  volume
					    false,                            // no boolean operation
					    0,
					    checkOverlaps);
  collar2PoleBottomInnerPV = new G4PVPlacement(nullptr,                                // rotation
					       dipolePosition,                   // position
					       collar2PoleBottomInnerLV,         // its logical volume
					       name+"_collar2_pole_top_inner_pv",// its name
					       containerLV,                      // its mother  volume
					       false,                            // no boolean operation
					       0,
					       checkOverlaps);
  collar2PoleBottomOuterPV = new G4PVPlacement(nullptr,                                // rotation
					       dipolePosition,                   // position
					       collar2PoleBottomOuterLV,         // its logical volume
					       name+"_collar2_pole_top_inner_pv",// its name
					       containerLV,                      // its mother  volume
					       false,                            // no boolean operation
					       0,
					       checkOverlaps);

  allPhysicalVolumes.insert(collar2PoleTopInnerPV);
  allPhysicalVolumes.insert(collar2PoleTopOuterPV);
  allPhysicalVolumes.insert(collar2PoleBottomInnerPV);
  allPhysicalVolumes.insert(collar2PoleBottomOuterPV);
  
  // outer annulus of collar - two as slightly different lengths
  G4VSolid* collarAnnulus2 = BDS::AppropriateTubs(name+"_collar2_annulus_solid",    // name
						  collarInnerRadiusF,               // inner radius
						  collarOuterRadius,                // outer radius
						  secondBPHalfLength-lengthSafety,  // length
						  0,                                // starting angle
						  CLHEP::twopi,                     // angle of sweep
						  inputFaceNormal,                  // input face normal
						  outputFaceNormal,                 // output face normal
						  flatFaces);
  
  allSolids.insert(collarAnnulus2);
  // make final solid pointer as collar round active beam pipe optional depending on how big active beam pipe is
  G4VSolid* collars = collarAnnulus2;

  //buildCollar = false;
  if (buildCollar)
    {
      // building collar around active pipe
      G4VSolid* collarAnnulus1 = BDS::AppropriateTubs(name+"_collar1_annulus_solid",  // name
					       collarInnerRadius,              // inner radius
					       collarOuterRadius,              // outer radius
					       length*0.5 - lengthSafetyLarge, // length
					       0,                              // starting angle
					       CLHEP::twopi,                   // angle of sweep
					       inputFaceNormal,                // input face normal
					       outputFaceNormal,               // output face normal
					       flatFaces);

      collars = new G4UnionSolid(name + "_collars_solid", // name
				 collarAnnulus2,          // solid1
				 collarAnnulus1,          // solid2
				 nullptr,                 // rotation
				 -2*dipolePosition);      // translation
      allSolids.insert(collarAnnulus1);
      allSolids.insert(collars);
    }

  /*
  // This part seems to not produce any overlapping volumes and no errors but won't render
  // in anything but the raytracer. Requires commented out collarBoxHalfHeight at top of this
  // method to be uncommented, plus new subtraction solid for yoke will need to written
  // ommitted for now
  
  G4VSolid* collarBox      = new G4Box(name + "_collar_box_solid",           // name
				       collarBoxHalfWidth,                   // x half width
				       collarBoxHalfHeight,                  // y half width
				       2*centralHalfLength);                 // z half length
  G4VSolid* collarBoxFaces = new G4CutTubs(name + "_collar_box_faces_solid", // name
					   0,                                // inner radius
					   50*CLHEP::cm,                     // outer radius
					   centralHalfLength - lengthSafety, // length
					   0,                                // starting angle
					   CLHEP::twopi,                     // sweep angle
					   inputFaceNormal,                  // input face normal
					   outputFaceNormal);                // output face normal
  G4VSolid* collarCentralPiece = new G4IntersectionSolid(name + "_collar_central_solid", // name
							 collarBox,                      // solid 1
							 collarBoxFaces);                // solid 2   
  
  G4VSolid* collarTotal = new G4UnionSolid(name + "_collar2_plus_box_solid", // name
					   collars,                          // solid 2
					   collarCentralPiece,               // solid 1
					   0,                                // rotation
					   -dipolePosition);    // translation
  */
  G4VSolid* collarTotal = collars;
  
  collarsLV =  new G4LogicalVolume(collarTotal,
                                   stainlesssteel_316LN_2K,
				   name+"_collars_lv");
  
  // collar annulus visualisation attributes
  collarsLV->SetVisAttributes(collarVisAtt);

  allLogicalVolumes.insert(collarsLV); // register locally
  
  collarsPV = new G4PVPlacement(nullptr,                  // rotation
			       dipolePosition,     // position
			       collarsLV,          // its logical volume
			       name+"_collars_pv", // its name
			       containerLV,        // its mother  volume
			       false,              // no boolean operation
			       0,                  // copy number
			       checkOverlaps);
  allPhysicalVolumes.insert(collarsPV);
  
  // outer iron yoke
  G4VSolid* yokeCylinder = BDS::AppropriateTubs(name+"_yoke_cylinder_solid",     // name
						0.,                              // inner radius
						yokeOuterRadius - lengthSafety,  // outer radius
						centralHalfLength-lengthSafety,  // length
						0,                               // starting angle
						CLHEP::twopi,                    // sweep angle
						inputFaceNormal,                 // input face normal
						outputFaceNormal,                // output face normal
						flatFaces);

  // need to cut hole out for everything inside - note subtraction solid has to be solid
  G4VSolid* yokeSubtractionCylinder = new G4Tubs(name + "_yoke_subtraction_cyl_solid", // name
						 0,                                    // inner radius
						 collarOuterRadius + lengthSafety,     // outer radius
						 length,                               // z half length - long for unambiguous subtraction
						 0,                                    // start angle
						 CLHEP::twopi);                        // sweep angle

  G4VSolid* yokeSubtractionSolid = new G4UnionSolid(name + "_yoke_subtraction_solid",  // name
						    yokeSubtractionCylinder,           // solid 1
						    yokeSubtractionCylinder,           // solid 2
						    nullptr,                           // rotation
						    2*dipolePosition);                 // translation
  
  yokeSolid = new G4SubtractionSolid(name+"_yoke_solid",   // name
				     yokeCylinder,         // from this
				     yokeSubtractionSolid, // subtract this
				     nullptr,              // rotation
				     -dipolePosition);     // translation
  allSolids.insert(yokeSubtractionCylinder);
  allSolids.insert(yokeSubtractionSolid);
  allSolids.insert(yokeSolid);
  yokeLV = new G4LogicalVolume(yokeSolid,
                               stainlesssteel_304L_2K,
			       name+"_yoke_lv");
  
  // yoke visualisation
  G4Colour* yokeColour;
  switch (colourIn)
    {
    case YokeColour::dipole:
      {yokeColour = BDSColours::Instance()->GetColour("LHCyoke"); break;}
    case YokeColour::kicker:
      {yokeColour = recipe->colour; break;}
    default:
      {yokeColour = BDSColours::Instance()->GetColour("LHCyoke"); break;}
    }
  G4VisAttributes* yokeVis = new G4VisAttributes(*yokeColour);
  yokeVis->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  allVisAttributes.insert(yokeVis);
  yokeLV->SetVisAttributes(yokeVis);
  
  allLogicalVolumes.insert(yokeLV); // register locally

  // yoke placement
  yokePV = new G4PVPlacement(nullptr,   // no rotation
			     G4ThreeVector(0,0,0),         // position
			     yokeLV,                       // lv to be placed
			     name + "_yoke_pv",            // name
			     containerLV,                  // mother lv to be placed in
			     false,                        // no boolean operation
			     0,                            // copy number
			     checkOverlaps);
  allPhysicalVolumes.insert(yokePV);

  BDSBeamPipeInfo* defaultModel = BDSGlobalConstants::Instance()->DefaultBeamPipeModel();
  G4Material* vacuumMaterial   = defaultModel->vacuumMaterial;
  
  //use beampipe factories to create another beampipe (note no magnetic field for now...)
  BDSBeamPipe* secondBP;
  if (flatFaces)
    {
      secondBP = BDSBeamPipeFactory::Instance()->CreateBeamPipe(BDSBeamPipeType::lhcdetailed,
								name,
								2*secondBPHalfLength-2*lengthSafety,
								2.202*CLHEP::cm,
								1.714*CLHEP::cm,
								2.202*CLHEP::cm,
								0,
								vacuumMaterial,
								1*CLHEP::mm,
								stainlesssteel_316LN_2K);
    }
  else
    {
      secondBP = BDSBeamPipeFactory::Instance()->CreateBeamPipe(BDSBeamPipeType::lhcdetailed,
								name,
								2 * secondBPHalfLength - 2 * lengthSafety,
								inputFaceNormal,
								outputFaceNormal,
								2.202 * CLHEP::cm,
								1.714 * CLHEP::cm,
								2.202 * CLHEP::cm,
								0,
								vacuumMaterial,
								1 * CLHEP::mm,
								stainlesssteel_316LN_2K);
    }
  
  secondBPLV = secondBP->GetContainerLogicalVolume();
  secondBPPV = new G4PVPlacement(nullptr,   // no rotation
				 dipolePosition,               // position
				 secondBPLV,                   // lv to be placed
				 name + "_second_beampipe_pv", // name
				 containerLV,                  // mother lv to be placed in
				 false,                        // no boolean operation
				 0,                            // copy number
				 checkOverlaps);
  allPhysicalVolumes.insert(secondBPPV);
  
  // visual attributes for container
  containerLV->SetVisAttributes(containerVisAttr);
  magnetContainerLV->SetVisAttributes(containerVisAttr);

  // user limits
  for (auto lv : allLogicalVolumes)
    {lv->SetUserLimits(defaultUserLimits);}
  containerLV->SetUserLimits(defaultUserLimits);
  magnetContainerLV->SetUserLimits(defaultUserLimits);
  
  // record extents
  // container radius is the same for all methods as all cylindrical
  G4double containerRadius = yokeOuterRadius;
  // massShift defined at very beginning of this function
  BDSExtent ext = BDSExtent(-containerRadius, containerRadius,
			    -containerRadius,containerRadius,
			    -length*0.5,length*0.5);

  magnetContainer = new BDSGeometryComponent(magnetContainerSolid,
					     magnetContainerLV,
					     ext, BDSExtent(), dipolePosition);
  
  // build the BDSMagnetOuter instance and return it
  BDSMagnetOuter* outer = new BDSMagnetOuter(containerSolid,
					     containerLV, ext,
					     magnetContainer);

  SetFaceNormals(outer);

  // register objects
  outer->RegisterDaughter(secondBP);
  outer->RegisterLogicalVolume(allLogicalVolumes);
  outer->RegisterSensitiveVolume(allLogicalVolumes, BDSSDType::energydep);
  outer->RegisterPhysicalVolume(allPhysicalVolumes);
  outer->RegisterVisAttributes(allVisAttributes);
  // no rotation matrices used in this one
  // allLogicalVolumes is a local variable and goes out of scope so doesn't
  // need to be emptied or reset here
  
  return outer;
}

BDSMagnetOuter* BDSMagnetOuterFactoryLHC::CreateRectangularBend(G4String     name,
								G4double     length,
								const BDSBeamPipe* beamPipe,
								G4double     containerLength,
								const BDSMagnetOuterInfo* recipe)
{
  return CreateSectorBend(name, length, beamPipe, containerLength, recipe);
}

BDSMagnetOuter* BDSMagnetOuterFactoryLHC::CreateQuadrupole(G4String      name,
							   G4double      length,
							   BDSBeamPipe*  beamPipe,
							   G4double      containerLength,
							   const BDSMagnetOuterInfo* recipe)
{
  G4double horizontalWidth = recipe->horizontalWidth;
  CleanUp();

  // test input parameters - set global options as default if not specified
  TestInputParameters(beamPipe,horizontalWidth);

  // geometrical constants
  // [1] LHC design report - Chapter 7, fig 7.3
  // [2] LHC design report - Chapter 7, fig 7.1
  G4double beamPipeAxisSeparation = beamSeparation;
  G4double massShift              = 0.5 * beamPipeAxisSeparation;   // central shift to geometry
  //G4double collarBoxHalfHeight    = 60*CLHEP::mm;                 // [1] by visual inspection
  G4double collarBoxHalfWidth     = 22*CLHEP::mm;                   // fits between outer coils

  // radii
  G4double containerInnerRadius   = beamPipe->GetContainerRadius()+1*CLHEP::um;
  G4double coilInnerRadius        = containerInnerRadius + lengthSafetyLarge;
  G4double coilInnerRadiusF       = 24.601*CLHEP::mm; // the fixed radius for the second pipe - F for fixed
  G4double coilOuterRadius        = 60*CLHEP::mm;                   // [2] by visual inspection
  G4double collarInnerRadius      = coilOuterRadius + lengthSafetyLarge;
  G4double collarInnerRadiusF     = coilOuterRadius + lengthSafetyLarge;
  G4double collarOuterRadius      = 101.18*CLHEP::mm;               // [1] - at 293K but don't have 1.9K measurement
  G4double yokeOuterRadius        = 570.0*0.5*CLHEP::mm;            // [2] - at 293K but don't have 1.9K measurement
  G4double magnetContainerRadius  = yokeOuterRadius + lengthSafetyLarge;

  // angular setup of coils
  // these angles were calculated by visually analysing the coil layout graph in [2]
  G4double poleFullAngle    = CLHEP::pi/6.; //30 degrees angle in radians
  G4double coilFullAngle    = CLHEP::pi/2. - poleFullAngle - 1e-3; // 1e-3 to prevent overlaps
  G4double coilHalfAngle    = coilFullAngle*0.5;
  G4double coilStartAngle   = -coilHalfAngle;
  G4double poleStartAngle   = coilHalfAngle;
  G4RotationMatrix* coil2rm = new G4RotationMatrix();
  G4RotationMatrix* coil3rm = new G4RotationMatrix();
  G4RotationMatrix* coil4rm = new G4RotationMatrix();
  coil2rm->rotateZ(CLHEP::pi/2.0);
  coil3rm->rotateZ(CLHEP::pi);
  coil4rm->rotateZ(CLHEP::pi*1.5);
  allRotationMatrices.insert(coil2rm);
  allRotationMatrices.insert(coil3rm);
  allRotationMatrices.insert(coil4rm);
  
  // whether to build various components around active beam pipe depending on how wide it is
  // these ONLY apply to the components around the active beampipe
  G4bool buildCoil         = true;
  G4bool buildCollar       = true; // collar around the active beam pipe
  if (coilInnerRadius > coilOuterRadius)
    {buildCoil = false;}
  // this still uses the boxHalfWidth but just as good as the collar annulli overlap slightly in the middle
  // and this will protect against this
  if ((coilInnerRadius > collarInnerRadius) && (coilInnerRadius < (massShift - collarBoxHalfWidth)))
    {collarInnerRadius = containerInnerRadius + lengthSafety;}
  if (coilInnerRadius > (massShift - collarBoxHalfWidth))
    {buildCollar = false;}
  G4bool buildAtAll = containerInnerRadius > (beamPipeAxisSeparation - collarOuterRadius - 1*CLHEP::mm);
  if ((coilInnerRadius > collarOuterRadius) || buildAtAll)
    {// pipe is too big to use with this geometry!
      throw BDSException(__METHOD_NAME__, "error in component \"" + name + "\"\n" +
			 "this beam pipe is too big to use with the LHC dipole geometry\n" +
			 "Please consider using a different magnet geometry for this particular magnet");
    }

  G4ThreeVector dipolePosition; // translation of whole assembly relative to centre of active pipe
  if (isLeftOffset)
    {
      dipolePosition = G4ThreeVector(massShift,0.,0.);
      beamPipeAxisSeparation  *= -1;
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "dipole to the left" << G4endl;
#endif
    }
  else
    {
      dipolePosition = G4ThreeVector(-massShift,0.,0.);
      //massShift     *= -1;
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "dipole to the right" << G4endl;
#endif
    }
  
  if (beamPipe->ContainerIsCircular())
    {
      //circular beampipe so we can simply use its radius
      //container is similar but slightly wider and hollow (to allow placement of beampipe)
      //have to do subtraction as cuttubs aperture is central and the beampipe (active one) is not here
      G4VSolid* containerSolidOuter = new G4Tubs(name + "_contiainer_solid_outer",  // name
						 0,                           // inner radius
						 yokeOuterRadius,             // outer radius
						 length*0.5,                  // half length
						 0,                           // rotation start angle
						 CLHEP::twopi);               // rotation finish angle
						    
      G4VSolid* containerSolidInner = new G4Tubs(name + "_contiainer_solid_inner",  // name
						 0,                                 // inner radius
						 containerInnerRadius,              // outer radius
						 length,                            // full length for unambiguous subtraction
						 0,                                 // rotation start angle
						 CLHEP::twopi);
      allSolids.insert(containerSolidOuter);
      allSolids.insert(containerSolidInner);
      containerSolid = new G4SubtractionSolid(name + "_container_solid",   // name
					      containerSolidOuter,         // outer bit
					      containerSolidInner,         // subtract this from it
					      nullptr,                     // rotation
					      -dipolePosition);            // translation
    }
  else
    {
      //container is similar but slightly wider
      G4VSolid* containerSolidOuter = new G4Tubs(name + "_contiainer_solid_outer",  // name
						 0,                                 // inner radius
						 yokeOuterRadius,                   // outer radius
						 length*0.5,                        // half length
						 0,                                 // rotation start angle
						 CLHEP::twopi);                     // rotation finish angle
      allSolids.insert(containerSolidOuter);
      containerSolid = new G4SubtractionSolid(name + "_container_solid",
					      containerSolidOuter,
					      beamPipe->GetContainerSubtractionSolid(),
					      nullptr,          // rotation
					      -dipolePosition); // translation
    }

  // make the whole magnet container solid
  BuildMagnetContainerSolidStraight(name, containerLength, magnetContainerRadius);
  // make the logical volume too manually as we don't use the BDSMagnetOuterFactoryBase method for this

  // Unlike other magnets, our container is tight-fitting so we fill with empty vacuum
  G4Material* worldMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->EmptyMaterial());

  magnetContainerLV = new G4LogicalVolume(magnetContainerSolid,
					  worldMaterial,
					  name + "_container_lv");
  containerLV = new G4LogicalVolume(containerSolid,
				    worldMaterial,
				    name + "_container_lv");
  
  // coil solids
  // only need one pole & coil per beampipe which can be repeatedly placed
  // just inner radius can be different depending on active beam pipe hence two
  G4VSolid*        coil1     = nullptr;
  G4VSolid*        coil2     = nullptr;
  G4LogicalVolume* coil1LV   = nullptr;
  G4LogicalVolume* coil2LV   = nullptr;
  G4VPhysicalVolume* coil1PV = nullptr;
  G4VPhysicalVolume* coil2PV = nullptr;
  G4VPhysicalVolume* coil3PV = nullptr;
  G4VPhysicalVolume* coil4PV = nullptr;
  G4VPhysicalVolume* coil5PV = nullptr;
  G4VPhysicalVolume* coil6PV = nullptr;
  G4VPhysicalVolume* coil7PV = nullptr;
  G4VPhysicalVolume* coil8PV = nullptr;

  // pole solids are the ones immediately after coilN clockwise
  G4VSolid*        pole1     = nullptr;
  G4VSolid*        pole2     = nullptr;
  G4LogicalVolume* pole1LV   = nullptr;
  G4LogicalVolume* pole2LV   = nullptr;
  G4VPhysicalVolume* pole1PV = nullptr;
  G4VPhysicalVolume* pole2PV = nullptr;
  G4VPhysicalVolume* pole3PV = nullptr;
  G4VPhysicalVolume* pole4PV = nullptr;
  G4VPhysicalVolume* pole5PV = nullptr;
  G4VPhysicalVolume* pole6PV = nullptr;
  G4VPhysicalVolume* pole7PV = nullptr;
  G4VPhysicalVolume* pole8PV = nullptr;

  // collar solid - only need one as can union with itself but offset
  G4VSolid*        collar    = nullptr;
  G4VSolid*        collars   = nullptr; // union of two collars
  G4LogicalVolume* collarsLV = nullptr;

  // materials and visual attributes
  G4Material*      nbti                    = BDSMaterials::Instance()->GetMaterial("NbTi.1");
  G4Material*      stainlesssteel_316LN_2K = BDSMaterials::Instance()->GetMaterial("stainless_steel_316LN_2K");
  G4Material*      stainlesssteel_304L_2K  = BDSMaterials::Instance()->GetMaterial("stainless_steel_304L_2K");
  
  G4VisAttributes* coilVisAtt   = new G4VisAttributes(*BDSColours::Instance()->GetColour("LHCcoil"));
  coilVisAtt->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  G4VisAttributes* collarVisAtt = new G4VisAttributes(*BDSColours::Instance()->GetColour("LHCcollar"));
  collarVisAtt->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  allVisAttributes.insert(coilVisAtt);
  allVisAttributes.insert(collarVisAtt);
  
  if (buildCoil)
    {
      // coil solid
      coil1 = new G4Tubs(name+"_coil1_solid",          // name
			 coilInnerRadius,              // inner radius
			 coilOuterRadius,              // outer radius
			 length*0.5-lengthSafety,    // z half length
			 coilStartAngle,               // start angle
			 coilFullAngle);               // sweep angle
      // coil logical volumes
      coil1LV = new G4LogicalVolume(coil1,             // solid
				    nbti,              // material
				    name+"_coil1_lv"); // name
      coil1LV->SetVisAttributes(coilVisAtt);
      allSolids.insert(coil1);
      allLogicalVolumes.insert(coil1LV);

      // pole solid
      pole1 = new G4Tubs(name+"_pole1_solid",          // name
			 coilInnerRadius,              // inner radius
			 coilOuterRadius,              // outer radius
			 length*0.5 - lengthSafety,    // z half length
			 poleStartAngle,               // start angle
			 poleFullAngle);               // sweep angle
      pole1LV = new G4LogicalVolume(pole1,             // solid
                                    stainlesssteel_316LN_2K,    // material
				    name+"_pole1_lv"); // name
      pole1LV->SetVisAttributes(collarVisAtt);
      allSolids.insert(pole1);
      allLogicalVolumes.insert(pole1LV);

      // coil placements
      coil1PV = new G4PVPlacement(nullptr,            // rotation
				  -dipolePosition,    // position
				  coil1LV,            // logical volume
				  name + "_coil1_pv", // name
				  containerLV,        // mother volume
				  false,              // boolean operation
				  0,                  // copy number
				  checkOverlaps);
      coil2PV = new G4PVPlacement(coil2rm,            // rotation
				  -dipolePosition,    // position
				  coil1LV,            // logical volume
				  name + "_coil2_pv", // name
				  containerLV,        // mother volume
				  false,              // boolean operation
				  0,                  // copy number
				  checkOverlaps);
      coil3PV = new G4PVPlacement(coil3rm,            // rotation
				  -dipolePosition,    // position
				  coil1LV,            // logical volume
				  name + "_coil3_pv", // name
				  containerLV,        // mother volume
				  false,              // boolean operation
				  0,                  // copy number
				  checkOverlaps);
      coil4PV = new G4PVPlacement(coil4rm,            // rotation
				  -dipolePosition,    // position
				  coil1LV,            // logical volume
				  name + "_coil4_pv", // name
				  containerLV,        // mother volume
				  false,              // boolean operation
				  0,                  // copy number
				  checkOverlaps);
      allPhysicalVolumes.insert(coil1PV);
      allPhysicalVolumes.insert(coil2PV);
      allPhysicalVolumes.insert(coil3PV);
      allPhysicalVolumes.insert(coil4PV);

      // pole placements
      pole1PV = new G4PVPlacement(nullptr,            // rotation
				  -dipolePosition,    // position
				  pole1LV,            // logical volume
				  name + "_pole1_pv", // name
				  containerLV,        // mother volume
				  false,              // boolean operation
				  0,                  // copy number
				  checkOverlaps);
      pole2PV = new G4PVPlacement(coil2rm,            // rotation
				  -dipolePosition,    // position
				  pole1LV,            // logical volume
				  name + "_pole2_pv", // name
				  containerLV,        // mother volume
				  false,              // boolean operation
				  0,                  // copy number
				  checkOverlaps);
      pole3PV = new G4PVPlacement(coil3rm,            // rotation
				  -dipolePosition,    // position
				  pole1LV,            // logical volume
				  name + "_pole3_pv", // name
				  containerLV,        // mother volume
				  false,              // boolean operation
				  0,                  // copy number
				  checkOverlaps);
      pole4PV = new G4PVPlacement(coil4rm,            // rotation
				  -dipolePosition,    // position
				  pole1LV,            // logical volume
				  name + "_pole4_pv", // name
				  containerLV,        // mother volume
				  false,              // boolean operation
				  0,                  // copy number
				  checkOverlaps);
      allPhysicalVolumes.insert(pole1PV);
      allPhysicalVolumes.insert(pole2PV);
      allPhysicalVolumes.insert(pole3PV);
      allPhysicalVolumes.insert(pole4PV);
    }
  
  // fixed coil
  coil2   = new G4Tubs(name+"_coil2_solid",          // name
		       coilInnerRadiusF,             // inner radius
		       coilOuterRadius,              // outer radius
		       length*0.5-lengthSafety,    // length
		       coilStartAngle,               // start angle
		       coilFullAngle);               // sweep angle
  coil2LV = new G4LogicalVolume(coil2,               // solid
				nbti,                // material
				name+"_coil2_lv");   // name
  coil2LV->SetVisAttributes(coilVisAtt);
  allSolids.insert(coil2);
  allLogicalVolumes.insert(coil2LV);
  
  // fixed pole
  pole2   = new G4Tubs(name+"_pole2_solid",          // name
		       coilInnerRadiusF,             // inner radius
		       coilOuterRadius,              // outer radius
		       length*0.5 - lengthSafety,    // z half length
		       poleStartAngle,               // start angle
		       poleFullAngle);               // sweep angle
  pole2LV = new G4LogicalVolume(pole2,               // solid
                                stainlesssteel_316LN_2K,      // material
				name+"_pole2_lv");   // name
  pole2LV->SetVisAttributes(collarVisAtt);
  allSolids.insert(pole2);
  allLogicalVolumes.insert(pole2LV);
  
  // fixed coil placements
  coil5PV = new G4PVPlacement(nullptr,            // rotation
			      dipolePosition,     // position
			      coil2LV,            // logical volume
			      name + "_coil5_pv", // name
			      containerLV,        // mother volume
			      false,              // boolean operation
			      0,                  // copy number
			      checkOverlaps);
  coil6PV = new G4PVPlacement(coil2rm,            // rotation
			      dipolePosition,     // position
			      coil2LV,            // logical volume
			      name + "_coil6_pv", // name
			      containerLV,        // mother volume
			      false,              // boolean operation
			      0,                  // copy number
			      checkOverlaps);
  coil7PV = new G4PVPlacement(coil3rm,            // rotation
			      dipolePosition,     // position
			      coil2LV,            // logical volume
			      name + "_coil7_pv", // name
			      containerLV,        // mother volume
			      false,              // boolean operation
			      0,                  // copy number
			      checkOverlaps);
  coil8PV = new G4PVPlacement(coil4rm,            // rotation
			      dipolePosition,     // position
			      coil2LV,            // logical volume
			      name + "_coil8_pv", // name
			      containerLV,        // mother volume
			      false,              // boolean operation
			      0,                  // copy number
			      checkOverlaps);
  allPhysicalVolumes.insert(coil5PV);
  allPhysicalVolumes.insert(coil6PV);
  allPhysicalVolumes.insert(coil7PV);
  allPhysicalVolumes.insert(coil8PV);
  
  // fixed pole placements
  pole5PV = new G4PVPlacement(nullptr,            // rotation
			      dipolePosition,     // position
			      pole2LV,            // logical volume
			      name + "_pole5_pv", // name
			      containerLV,        // mother volume
			      false,              // boolean operation
			      0,                  // copy number
			      checkOverlaps);
  pole6PV = new G4PVPlacement(coil2rm,            // rotation
			      dipolePosition,     // position
			      pole2LV,            // logical volume
			      name + "_pole6_pv", // name
			      containerLV,        // mother volume
			      false,              // boolean operation
			      0,                  // copy number
			      checkOverlaps);
  pole7PV = new G4PVPlacement(coil3rm,            // rotation
			      dipolePosition,     // position
			      pole2LV,            // logical volume
			      name + "_pole7_pv", // name
			      containerLV,        // mother volume
			      false,              // boolean operation
			      0,                  // copy number
			      checkOverlaps);
  pole8PV = new G4PVPlacement(coil4rm,            // rotation
			      dipolePosition,     // position
			      pole2LV,            // logical volume
			      name + "_pole8_pv", // name
			      containerLV,        // mother volume
			      false,              // boolean operation
			      0,                  // copy number
			      checkOverlaps);

  allPhysicalVolumes.insert(pole5PV);
  allPhysicalVolumes.insert(pole6PV);
  allPhysicalVolumes.insert(pole7PV);
  allPhysicalVolumes.insert(pole8PV);
  
  // non-magnetic collars
  // collar pole solid
  collar = new G4Tubs(name+"_collar_solid",        // name
		      collarInnerRadiusF,          // inner radius
		      collarOuterRadius,           // outer radius
		      length*0.5 - lengthSafety, // length
		      0,                           // start angle
		      CLHEP::twopi);               // sweep angle
  allSolids.insert(collar);
  collars = collar;
  if (buildCollar)
    {
      if (collarInnerRadius == collarInnerRadiusF)
	{
	  // can save a solid by doing the union with the existing one if they're the same
	  collars = new G4UnionSolid(name + "_collars_solid",  // name
				     collar,                   // solid 1
				     collar,                   // solid 2
				     nullptr,                  // rotation
				     -2*dipolePosition);       // translation
	}
      else
	{
	  G4VSolid* collar2 = new G4Tubs(name+"_collar2_solid",      // name
					 collarInnerRadius,          // inner radius
					 collarOuterRadius,          // outer radius
					 length*0.5-lengthSafety,    // length
					 0,                          // starting angle
					 CLHEP::twopi);              // angle of sweep
	  allSolids.insert(collar2);
	  collars = new G4UnionSolid(name + "_collars_solid", // name
				     collar,                  // solid 1
				     collar2,                 // solid 2
				     nullptr,                 // rotation
				     -2*dipolePosition);      // translation
	}
      allSolids.insert(collars);
    }
  
  collarsLV = new G4LogicalVolume(collars,
				  stainlesssteel_316LN_2K,
				  name+"_collars_lv");
  collarsLV->SetVisAttributes(collarVisAtt);
  allLogicalVolumes.insert(collarsLV); 

  G4PVPlacement* collarPV = new G4PVPlacement(nullptr,            // rotation
					      dipolePosition,     // position
					      collarsLV,          // its logical volume
					      name+"_collars_pv", // its name
					      containerLV,        // its mother  volume
					      false,              // no boolean operation
					      0,                  // copy number
					      checkOverlaps);
  allPhysicalVolumes.insert(collarPV);

  // prepare a solid to cut a hole in the outer yoke volume (can just use one twice)
  // can't use the existing collar solids as they're not solid - need them to be solid
  G4VSolid* collarSubtractionCylinder = new G4Tubs(name+"_collar_subtraction_solid",  // name
						   0,                                 // inner radius
						   collarOuterRadius + lengthSafety,  // outer radius
						   length,                            // double length for unambiguous subtraction
						   0,                                 // starting angle
						   CLHEP::twopi);                     // sweep angle

  G4VSolid* collarSubtractionCylinders = new G4UnionSolid(name + "_collar_subtraction_cylinders", // name
							  collarSubtractionCylinder,              // solid1
							  collarSubtractionCylinder,              // solid2 (here = solid1)
							  nullptr,                                // rotation
							  2*dipolePosition);                      // translation
 
  // outer iron yoke
  G4VSolid* yokeCylinder = new G4Tubs(name+"_yoke_cylinder_solid",     // name
				      0.,                              // inner radius
				      yokeOuterRadius,                 // outer radius
				      0.5*length-lengthSafety,       // length
				      0,                               // starting angle
				      CLHEP::twopi * CLHEP::rad);      // sweep angle

  yokeSolid = new G4SubtractionSolid(name+"_yoke_solid",             // name
				     yokeCylinder,                   // from this
				     collarSubtractionCylinders,     // subtract this
				     nullptr,
				     -dipolePosition);               
  allSolids.insert(collarSubtractionCylinder);
  allSolids.insert(collarSubtractionCylinders);
  allSolids.insert(yokeCylinder);
  yokeLV = new G4LogicalVolume(yokeSolid,
			       stainlesssteel_304L_2K,
			       name+"_yoke_lv");
  
  // yoke visualisation
  G4VisAttributes* LHCred = new G4VisAttributes(*BDSColours::Instance()->GetColour("LHCyokered"));
  LHCred->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  allVisAttributes.insert(LHCred);
  yokeLV->SetVisAttributes(LHCred);
  
  allLogicalVolumes.insert(yokeLV); // register locally

  // yoke placement
  yokePV = new G4PVPlacement(nullptr,   // no rotation
			     G4ThreeVector(0,0,0),         // position
			     yokeLV,                       // lv to be placed
			     name + "_yoke_pv",            // name
			     containerLV,                  // mother lv to be placed in
			     false,                        // no boolean operation
			     0,                            // copy number
			     checkOverlaps);
  allPhysicalVolumes.insert(yokePV);

  BDSBeamPipeInfo* defaultModel = BDSGlobalConstants::Instance()->DefaultBeamPipeModel();
  G4Material* vacuumMaterial   = defaultModel->vacuumMaterial;
  
  //use beampipe factories to create another beampipe (note no magnetic field for now...)
  BDSBeamPipe* secondBP = BDSBeamPipeFactory::Instance()->CreateBeamPipe(BDSBeamPipeType::lhcdetailed,
									 name,
									 length-2*lengthSafety,
									 2.202*CLHEP::cm,   // aper1
									 1.714*CLHEP::cm,   // aper2
									 2.202*CLHEP::cm,   // aper3
									 0,                 // aper4
									 vacuumMaterial,    // vacuum material
									 1*CLHEP::mm,       // beampipeThickness
									 stainlesssteel_316LN_2K); // beampipe material
  
  G4LogicalVolume* secondBPLV = secondBP->GetContainerLogicalVolume();
  G4PVPlacement* secondBPPV = new G4PVPlacement(nullptr,   // no rotation
						dipolePosition,               // position
						secondBPLV,                   // lv to be placed
						name + "_second_beampipe_pv", // name
						containerLV,                  // mother lv to be placed in
						false,                        // no boolean operation
						0,                            // copy number
						checkOverlaps);
  allPhysicalVolumes.insert(secondBPPV);
  
  // visual attributes for container
  containerLV->SetVisAttributes(containerVisAttr);
  magnetContainerLV->SetVisAttributes(containerVisAttr);
  
  // user limits
  for (auto lv : allLogicalVolumes)
    {lv->SetUserLimits(defaultUserLimits);}
  containerLV->SetUserLimits(defaultUserLimits);
  magnetContainerLV->SetUserLimits(defaultUserLimits);
    
  // record extents
  // container radius is the same for all methods as all cylindrical
  G4double containerRadius = yokeOuterRadius;
  // Here the extent is considered without any offset.
  BDSExtent ext = BDSExtent(-containerRadius,containerRadius,
			    -containerRadius,containerRadius,
			    -length*0.5,length*0.5);
  
  magnetContainer = new BDSGeometryComponent(magnetContainerSolid,
					     magnetContainerLV, ext, BDSExtent(),
					     dipolePosition);
  
  // build the BDSMagnetOuter instance and return it
  BDSMagnetOuter* outer = new BDSMagnetOuter(containerSolid,
					     containerLV, ext,
					     magnetContainer);

  SetFaceNormals(outer);
  
  // register objects
  outer->RegisterDaughter(secondBP);
  outer->RegisterSolid(allSolids);
  outer->RegisterLogicalVolume(allLogicalVolumes);
  outer->RegisterPhysicalVolume(allPhysicalVolumes);
  outer->RegisterVisAttributes(allVisAttributes);
  outer->RegisterRotationMatrix(allRotationMatrices);
  
  return outer;
}

BDSMagnetOuter* BDSMagnetOuterFactoryLHC::CreateSextupole(G4String      name,
							  G4double      length,
							  BDSBeamPipe*  beamPipe,
							  G4double      containerLength,
							  const BDSMagnetOuterInfo* recipe)
{
  return cylindrical->CreateSextupole(name,length,beamPipe,containerLength,recipe);
}

BDSMagnetOuter* BDSMagnetOuterFactoryLHC::CreateOctupole(G4String      name,
							 G4double      length,
							 BDSBeamPipe*  beamPipe,
							 G4double      containerLength,
							 const BDSMagnetOuterInfo* recipe)
{
  return cylindrical->CreateOctupole(name,length,beamPipe,containerLength,recipe);
}

BDSMagnetOuter* BDSMagnetOuterFactoryLHC::CreateDecapole(G4String      name,
							 G4double      length,
							 BDSBeamPipe*  beamPipe,
							 G4double      containerLength,
							 const BDSMagnetOuterInfo* recipe)
{
  return cylindrical->CreateDecapole(name,length,beamPipe,containerLength,recipe);
}

BDSMagnetOuter* BDSMagnetOuterFactoryLHC::CreateSolenoid(G4String      name,
							 G4double      length,
							 BDSBeamPipe*  beamPipe,
							 G4double      containerLength,
							 const BDSMagnetOuterInfo* recipe)
{
  return cylindrical->CreateSolenoid(name,length,beamPipe,containerLength,recipe);
}

BDSMagnetOuter* BDSMagnetOuterFactoryLHC::CreateMultipole(G4String      name,
							  G4double      length,
							  BDSBeamPipe*  beamPipe,
							  G4double      containerLength,
							  const BDSMagnetOuterInfo* recipe)
{
  return cylindrical->CreateMultipole(name,length,beamPipe,containerLength,recipe);
}

BDSMagnetOuter* BDSMagnetOuterFactoryLHC::CreateRfCavity(G4String      name,
							 G4double      length,
							 BDSBeamPipe*  beamPipe,
							 G4double      containerLength,
							 const BDSMagnetOuterInfo* recipe)
{
  return cylindrical->CreateRfCavity(name,length,beamPipe,containerLength,recipe);
}

BDSMagnetOuter* BDSMagnetOuterFactoryLHC::CreateMuonSpoiler(G4String      name,
							    G4double      length,
							    BDSBeamPipe*  beamPipe,
							    G4double      containerLength,
							    const BDSMagnetOuterInfo* recipe)
{
  return cylindrical->CreateMuonSpoiler(name,length,beamPipe,containerLength,recipe);
}

BDSMagnetOuter* BDSMagnetOuterFactoryLHC::CreateKicker(G4String                  name,
						       G4double                  length,
						       const BDSBeamPipe*        beamPipe,
						       G4double                  containerLength,
						       const BDSMagnetOuterInfo* recipe,
						       G4bool                    /*vertical*/)
{
  return CreateLHCDipole(name, length, beamPipe, containerLength, recipe, YokeColour::kicker);
}

/// functions below here are private to this particular factory
void BDSMagnetOuterFactoryLHC::TestInputParameters(const BDSBeamPipe* /*beamPipe*/,
						   G4double&    horizontalWidth)// reference to a pointer
{
  // ensure horizontalWidth is > outerCollarDiameter - hard coded as specific to the lhc design
  if (horizontalWidth < 202*CLHEP::mm )
    {horizontalWidth = 202*CLHEP::mm;}
}
