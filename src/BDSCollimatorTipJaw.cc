/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSAcceleratorModel.hh"
#include "BDSCollimatorTipJaw.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSMaterials.hh"
#include "BDSSDType.hh"
#include "BDSUtilities.hh"
#include "BDSTipCollimator.hh"

#include "G4Box.hh"
#include "G4Para.hh"
#include "G4GenericTrap.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"

#include <cmath>
#include <vector>
#include <map>
#include <set>

BDSCollimatorTipJaw::BDSCollimatorTipJaw(const G4String&    nameIn,
                                         G4double    lengthIn,
                                         G4double    horizontalWidthIn,
                                         G4double    xHalfGapIn,
                                         G4double    yHalfHeightIn,
                                         G4double    xSizeLeftIn,
                                         G4double    xSizeRightIn,
                                         G4double    leftJawTiltIn,
                                         G4double    rightJawTiltIn,
                                         G4double    tipThicknessIn,
                                         G4bool      buildLeftJawIn,
                                         G4bool      buildRightJawIn,
                                         G4Material* collimatorMaterialIn,
                                         G4Material* collimatorTipMaterialIn,
                                         G4Material* vacuumMaterialIn,
                                         G4Colour*   colourIn,
                                         G4Colour*   tipColourIn):
BDSTipCollimator(nameIn, lengthIn, horizontalWidthIn, "jcoltip", collimatorMaterialIn, collimatorTipMaterialIn, vacuumMaterialIn,
              xHalfGapIn, yHalfHeightIn, xHalfGapIn, yHalfHeightIn, colourIn, tipColourIn, tipThicknessIn),
  jawSolid(nullptr),
  xSizeLeft(xSizeLeftIn),
  xSizeRight(xSizeRightIn),
  xHalfGap(xHalfGapIn),
  jawTiltLeft(leftJawTiltIn),
  jawTiltRight(rightJawTiltIn),
  tipThickness(tipThicknessIn),
  jawHalfWidth(0),
  yHalfHeight(yHalfHeightIn),
  buildLeftJaw(buildLeftJawIn),
  buildRightJaw(buildRightJawIn),
  buildAperture(true)
{
  jawHalfWidth = 0.5 * (0.5*horizontalWidth - lengthSafetyLarge - xHalfGap);
}

BDSCollimatorTipJaw::~BDSCollimatorTipJaw()
{;}

void BDSCollimatorTipJaw::CheckParameters()
{
  if (!colour)
    {colour = BDSColours::Instance()->GetColour("collimator");}
  if (!tipColour)
    {tipColour = BDSColours::Instance()->GetColour("collimatorTip");}
  
  if (jawHalfWidth < 1e-3) // 1um minimum, could also be negative
    {throw BDSException(__METHOD_NAME__, "horizontalWidth insufficient given xsize of jcoltip \"" + name + "\"");}

  // set half height to half horizontal width if zero - finite height required.
  if (!BDS::IsFinite(yHalfHeight))
    {yHalfHeight = 0.5*horizontalWidth;}

  if (BDS::IsFinite(yHalfHeight) && (yHalfHeight < 1e-3)) // 1um minimum
    {throw BDSException(__METHOD_NAME__, "insufficient ysize for jcoltip \"" + name + "\"");}

  if ((yHalfHeight < 0) || ((yHalfHeight > 0) && (yHalfHeight < 1e-3))) // 1um minimum and not negative
    {throw BDSException(__METHOD_NAME__, "insufficient ysize for jcoltip \"" + name + "\"");}

  if (xSizeLeft < 0)
    {throw BDSException(__METHOD_NAME__, "left jcoltip jaw cannot have negative half aperture size: \"" + name + "\"");}
  if (xSizeRight < 0)
    {throw BDSException(__METHOD_NAME__, "left jcoltip jaw cannot have negative half aperture size: \"" + name + "\"");}

  if (std::abs(xSizeLeft) > 0.5*horizontalWidth)
    {
      G4cerr << __METHOD_NAME__ << "jcoltip \"" << name
             << "\" left jaw offset is greater the element half width, jaw "
             << "will not be constructed" << G4endl;
      buildLeftJaw = false;
    }
  if (std::abs(xSizeRight) > 0.5*horizontalWidth)
    {
      G4cerr << __METHOD_NAME__ << "jcoltip \"" << name
             << "\" right jaw offset is greater the element half width, jaw "
             << "will not be constructed" << G4endl;
      buildRightJaw = false;
    }
  
  if (std::abs(jawTiltLeft) > 0 && std::tan(std::abs(jawTiltLeft)) * chordLength / 2. > std::max(xHalfGap, xSizeLeft))
    {throw BDSException(__METHOD_NAME__, "tilted left jaw not allowed to cross the mid-plane: \"" + name + "\"");}

  if (std::abs(jawTiltRight) > 0 && std::tan(std::abs(jawTiltRight)) * chordLength / 2. > std::max(xHalfGap, xSizeLeft))
    {throw BDSException(__METHOD_NAME__, "tilted right jaw not allowed to cross the mid-plane: \"" + name + "\"");}

  if (!buildLeftJaw && !buildRightJaw)
    {throw BDSException(__METHOD_NAME__, "no jaws being built: \"" + name + "\"");}
  
  if (!BDS::IsFinite(xHalfGap) && !BDS::IsFinite(xSizeLeft) && !BDS::IsFinite(xSizeRight))
    {buildAperture = false;}
}

void BDSCollimatorTipJaw::BuildContainerLogicalVolume()
{
  G4double horizontalHalfWidth = horizontalWidth * 0.5;
  if (jawTiltLeft != 0 || jawTiltRight != 0)
    {
      // The box must encompass everything, so pick the largest absolute angle
      horizontalHalfWidth = horizontalWidth * 0.5 + chordLength * 0.5 * std::sin(std::max(std::abs(jawTiltLeft), std::abs(jawTiltRight)));
    }
  
    // For the case of jaw tilt, adjust the horizontal size, but keep the container length the same
  // This results in small drifts either side of the collimator, but preserves the overall size
  containerSolid = new G4Box(name + "_container_solid",
                             horizontalHalfWidth,
                             yHalfHeight,
                             chordLength*0.5);
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
                                               vacuumMaterial,
                                               name + "_container_lv");
  BDSExtent ext(horizontalHalfWidth, yHalfHeight, chordLength*0.5);
  SetExtent(ext);
}

void BDSCollimatorTipJaw::Build()
{
  CheckParameters();
  BDSAcceleratorComponent::Build();

  // set each jaws half gap default to aperture half size
  G4double leftJawHalfGap = xHalfGap;
  G4double rightJawHalfGap = xHalfGap;

  // update jaw half gap with offsets
  // if one jaw is not constructed, set the opening to xSize/2 for the aperture vacuum volume creation
  if (BDS::IsFinite(xSizeLeft))
    {
      if (buildLeftJaw)
        {leftJawHalfGap = xSizeLeft;}
      else
        {leftJawHalfGap = 0.5 * horizontalWidth;}
    }

  if (BDS::IsFinite(xSizeRight))
    {
      if (buildRightJaw)
        {rightJawHalfGap = xSizeRight;}
      else
        {rightJawHalfGap = 0.5 * horizontalWidth;}
    }

  // jaws have to fit inside containerLogicalVolume so calculate full jaw widths given offsets
  G4double leftJawWidth = 0.5 * horizontalWidth - leftJawHalfGap;
  G4double rightJawWidth = 0.5 * horizontalWidth - rightJawHalfGap;
  G4double vacuumWidth = 0.5 * (leftJawHalfGap + rightJawHalfGap);

  // centre of jaw and vacuum volumes for placements
  G4double leftJawCentre = 0.5*leftJawWidth + leftJawHalfGap;
  G4double rightJawCentre = 0.5*rightJawWidth + rightJawHalfGap;
  G4double vacuumCentre = 0.5*(leftJawHalfGap - rightJawHalfGap);

  G4ThreeVector leftJawPos = G4ThreeVector(leftJawCentre, 0, 0);
  G4ThreeVector rightJawPos = G4ThreeVector(-rightJawCentre, 0, 0);
  G4ThreeVector vacuumOffset = G4ThreeVector(vacuumCentre, 0, 0);

  G4VisAttributes* collimatorVisAttr = new G4VisAttributes(*colour);
  G4VisAttributes* tipVisAttr = new G4VisAttributes(*tipColour);
  
  RegisterVisAttributes(collimatorVisAttr);
  RegisterVisAttributes(tipVisAttr);

  // get appropriate user limits for jaw material
  G4UserLimits* collUserLimits = CollimatorUserLimits();

  // build jaws as appropriate
  if (buildLeftJaw && buildAperture)
    {
      G4VSolid* leftJawSolid = nullptr;
      G4VSolid* leftJawTipSolid = nullptr;
      
      if (jawTiltLeft != 0)
        {
          // Adjust the length of the parallelepiped to match the inside edges in Z
          // Due to the straight parallelepiped edges, it will never match the volume an angled box,
          // so it is chosen to underestimate the volume, but preserve the jaw x-y cutting plane.
          G4double leftHalfLength = chordLength * 0.5 * std::cos(jawTiltLeft);
          
          leftJawSolid = new G4Para(name + "_leftjaw_solid",
                                    (leftJawWidth - tipThickness) * 0.5 - lengthSafety,
                                    yHalfHeight - lengthSafety,
                                    leftHalfLength - lengthSafety,
                                    0,
                                    jawTiltLeft,
                                    0);
                                    
          leftJawTipSolid = new G4Para(name + "_leftjawtip_solid",
                                      tipThickness * 0.5 - lengthSafety,
                                      yHalfHeight - lengthSafety,
                                      leftHalfLength - lengthSafety,
                                      0,
                                      jawTiltLeft,
                                      0);
        }
      else
        {
          leftJawSolid = new G4Box(name + "_leftjaw_solid",
                                   (leftJawWidth - tipThickness) * 0.5 - lengthSafety,
                                   yHalfHeight - lengthSafety,
                                   chordLength * 0.5 - lengthSafety);
                                   
          leftJawTipSolid = new G4Box(name + "_leftjawtip_solid",
                                      tipThickness * 0.5 - lengthSafety,
                                      yHalfHeight - lengthSafety,
                                      chordLength * 0.5 - lengthSafety);
        }
      
      RegisterSolid(leftJawSolid);
      RegisterSolid(leftJawTipSolid);
      
      G4LogicalVolume* leftJawLV = new G4LogicalVolume(leftJawSolid,       // solid
                                                       collimatorMaterial,    // material
                                                       name + "_leftjaw_lv"); // name
      leftJawLV->SetVisAttributes(collimatorVisAttr);

      // user limits - provided by BDSAcceleratorComponent
      leftJawLV->SetUserLimits(collUserLimits);

      // register with base class (BDSGeometryComponent)
      RegisterLogicalVolume(leftJawLV);
      // register it in a set of collimator logical volumes
      BDSAcceleratorModel::Instance()->VolumeSet("collimators")->insert(leftJawLV);
      if (sensitiveOuter)
        {RegisterSensitiveVolume(leftJawLV, BDSSDType::collimatorcomplete);}
      
      G4LogicalVolume* leftJawTipLV = new G4LogicalVolume(leftJawTipSolid,
                                                          collimatorTipMaterial,
                                                          name + "_leftjawtip_lv");
      leftJawTipLV->SetVisAttributes(tipVisAttr);
      leftJawTipLV->SetUserLimits(collUserLimits);
      RegisterLogicalVolume(leftJawTipLV);
      BDSAcceleratorModel::Instance()->VolumeSet("collimators")->insert(leftJawTipLV);
      if (sensitiveOuter)
        {RegisterSensitiveVolume(leftJawTipLV, BDSSDType::collimatorcomplete);}
      
      // place the tip
      G4PVPlacement* leftJawTipPV = new G4PVPlacement(nullptr,           // rotation
                                                      leftJawPos - G4ThreeVector((leftJawWidth - tipThickness) * 0.5, 0, 0), // position
                                                      leftJawTipLV,        // logical volume
                                                      name + "_leftjawtip_pv", // name
                                                      containerLogicalVolume,  // its mother volume
                                                      false,               // no boolean operation
                                                      0,                   // copy number
                                                      checkOverlaps); 
      RegisterPhysicalVolume(leftJawTipPV);
      
      // place the jaw
      G4PVPlacement* leftJawPV = new G4PVPlacement(nullptr,              // rotation
                                                   leftJawPos + G4ThreeVector(tipThickness * 0.5, 0, 0), // position
                                                   leftJawLV,            // logical volume
                                                   name + "_leftjaw_pv", // name
                                                   containerLogicalVolume,  // its mother volume
                                                   false,                // no boolean operation
                                                   1,                    // copy number
                                                   checkOverlaps);
      RegisterPhysicalVolume(leftJawPV);
    }
  if (buildRightJaw && buildAperture)
    {
      G4VSolid* rightJawSolid = nullptr;
      G4VSolid* rightJawTipSolid = nullptr;
      
      if (jawTiltRight != 0)
        {
          // Adjust the length of the parallelepiped to match the inside edges in Z
          // Due to the straight parallelepiped edges, it will never match the volume an angled box,
          // so it is chosen to underestimate the volume, but preserve the jaw x-y cutting plane.
          G4double rightHalfLength = chordLength * 0.5 * std::cos(jawTiltRight);

          rightJawSolid = new G4Para(name + "_rightjaw_solid",
                                     (rightJawWidth - tipThickness) * 0.5 - lengthSafety,
                                     yHalfHeight - lengthSafety,
                                     rightHalfLength  - lengthSafety,
                                     0,
                                     jawTiltRight,
                                     0);
                                     
          rightJawTipSolid = new G4Para(name + "_rightjawtip_solid",
                                       tipThickness * 0.5 - lengthSafety,
                                       yHalfHeight - lengthSafety,
                                       rightHalfLength - lengthSafety,
                                       0,
                                       jawTiltRight,
                                       0);
        }
      else
        {
          rightJawSolid = new G4Box(name + "_rightjaw_solid",
                                    (rightJawWidth - tipThickness) * 0.5 - lengthSafety,
                                    yHalfHeight - lengthSafety,
                                    chordLength * 0.5 - lengthSafety);
                                    
          rightJawTipSolid = new G4Box(name + "_rightjawtip_solid",
                                       tipThickness * 0.5 - lengthSafety,
                                       yHalfHeight - lengthSafety,
                                       chordLength * 0.5 - lengthSafety);
        }

      RegisterSolid(rightJawSolid);
      RegisterSolid(rightJawTipSolid);
      
      G4LogicalVolume* rightJawLV = new G4LogicalVolume(rightJawSolid,
                                                        collimatorMaterial,
                                                        name + "_rightjaw_lv");
      rightJawLV->SetVisAttributes(collimatorVisAttr);

      // user limits - provided by BDSAcceleratorComponent
      rightJawLV->SetUserLimits(collUserLimits);

      // register with base class (BDSGeometryComponent)
      RegisterLogicalVolume(rightJawLV);
      // register it in a set of collimator logical volumes
      BDSAcceleratorModel::Instance()->VolumeSet("collimators")->insert(rightJawLV);
      if (sensitiveOuter)
        {RegisterSensitiveVolume(rightJawLV, BDSSDType::collimatorcomplete);}
      
      G4LogicalVolume* rightJawTipLV = new G4LogicalVolume(rightJawTipSolid,
                                                           collimatorTipMaterial,
                                                           name + "_rightjawtip_lv");
      rightJawTipLV->SetVisAttributes(tipVisAttr);
      rightJawTipLV->SetUserLimits(collUserLimits);
      RegisterLogicalVolume(rightJawTipLV);
      BDSAcceleratorModel::Instance()->VolumeSet("collimators")->insert(rightJawTipLV);
      if (sensitiveOuter)
        {RegisterSensitiveVolume(rightJawTipLV, BDSSDType::collimatorcomplete);}
      
      // place the tip
      G4PVPlacement* rightJawTipPV = new G4PVPlacement(nullptr,     // rotation
                                                       rightJawPos + G4ThreeVector((rightJawWidth - tipThickness) * 0.5, 0, 0), // position
                                                       rightJawTipLV, // its logical volume
                                                       name + "_rightjawtip_pv", // its name
                                                       containerLogicalVolume, // its mother volume
                                                       false,           // no boolean operation
                                                       0,               // copy number
                                                       checkOverlaps);
      RegisterPhysicalVolume(rightJawTipPV);
      
      // place the jaw
      G4PVPlacement* rightJawPV = new G4PVPlacement(nullptr,          // rotation
                                                    rightJawPos - G4ThreeVector(tipThickness * 0.5, 0, 0),  // position
                                                    rightJawLV,     // logical volume
                                                    name + "_rightjaw_pv",  // name
                                                    containerLogicalVolume,  // its mother volume
                                                    false,          // no boolean operation
                                                    1,              // copy number
                                                    checkOverlaps);
      RegisterPhysicalVolume(rightJawPV);
    }
  // if no aperture but the code has got to this stage, build the collimator as a simple box.
  if (!buildAperture)
    {
      collimatorSolid = new G4Box(name + "_solid",
                                  horizontalWidth * 0.5 - lengthSafety,
                                  yHalfHeight - lengthSafety,
                                  chordLength * 0.5 - lengthSafety);
      RegisterSolid(collimatorSolid);
      
      G4LogicalVolume* collimatorLV = new G4LogicalVolume(collimatorSolid,
                                                          collimatorMaterial,
                                                          name + "_lv");
      collimatorLV->SetVisAttributes(collimatorVisAttr);

      // user limits - provided by BDSAcceleratorComponent - don't use collUserLimits
      collimatorLV->SetUserLimits(userLimits);

      // register with base class (BDSGeometryComponent)
      RegisterLogicalVolume(collimatorLV);
      if (sensitiveOuter)
        {RegisterSensitiveVolume(collimatorLV, BDSSDType::collimatorcomplete);}
      
      // place the collimator
      G4PVPlacement* collimatorPV = new G4PVPlacement(nullptr,
                                                      (G4ThreeVector) 0,
                                                      collimatorLV,
                                                      name + "_pv",
                                                      containerLogicalVolume,
                                                      false,
                                                      0,
                                                      checkOverlaps);
      RegisterPhysicalVolume(collimatorPV);
    }
  
  // build and place the vacuum volume only if the aperture is finite.
  if (buildAperture)
    {
      if (jawTiltLeft != 0 || jawTiltRight != 0)
        {
          /// If the jaw is not built, do not take it's tilt into account for the vacuum box
          G4double tiltLeft = buildLeftJaw ? jawTiltLeft : 0.;
          G4double tiltRight = buildRightJaw ? jawTiltRight : 0.;
          
          /// The vacuum volume should extend from edge to edge, but the tilted jaws themselves don't
          /// Compute an effective length to correctly obtain the vacuum size at the edges
          G4double halfLengthLeftEff = (chordLength  * 0.5) / std::cos(tiltLeft);
          G4double halfLengthRightEff = (chordLength  * 0.5) / std::cos(tiltRight);

          /// Rotate about y (from the z to the x axis) at x = 0 and translate
          /// The right jaw is at a negative half-gap
          G4double xGapLeftUpstream = -halfLengthLeftEff * std::sin(tiltLeft) + leftJawHalfGap;
          G4double xGapLeftDownstream = halfLengthLeftEff * std::sin(tiltLeft) + leftJawHalfGap;
          G4double xGapRightUpstream = -halfLengthRightEff * std::sin(tiltRight) - rightJawHalfGap;
          G4double xGapRightDownstream = halfLengthRightEff * std::sin(tiltRight) - rightJawHalfGap;

          std::vector<G4TwoVector> vertices {G4TwoVector(xGapRightUpstream + lengthSafety, -(yHalfHeight - lengthSafety)),
                                             G4TwoVector(xGapRightUpstream + lengthSafety, (yHalfHeight - lengthSafety)),
                                             G4TwoVector(xGapLeftUpstream - lengthSafety, (yHalfHeight - lengthSafety)),
                                             G4TwoVector(xGapLeftUpstream - lengthSafety, -(yHalfHeight - lengthSafety)),
                                             G4TwoVector(xGapRightDownstream + lengthSafety, -(yHalfHeight - lengthSafety)),
                                             G4TwoVector(xGapRightDownstream + lengthSafety, (yHalfHeight - lengthSafety)),
                                             G4TwoVector(xGapLeftDownstream - lengthSafety, (yHalfHeight - lengthSafety)),
                                             G4TwoVector(xGapLeftDownstream - lengthSafety, -(yHalfHeight - lengthSafety))};

          vacuumSolid = new G4GenericTrap(name + "_vacuum_solid",
                                          chordLength * 0.5 - lengthSafety,
                                          vertices);
          // The for tilted jaws, the vacuum trapezoid is constructed from absolute coordinates
          // need to rese the vacuum offset, which is intended for a box
          vacuumOffset = G4ThreeVector(0, 0, 0);
        }
      else
        {
          vacuumSolid = new G4Box(name + "_vacuum_solid",              // name
                                  vacuumWidth - lengthSafety,          // x half width
                                  yHalfHeight - lengthSafety,          // y half width
                                  chordLength * 0.5);                  // z half length
        }
      
      RegisterSolid(vacuumSolid);
      
      G4LogicalVolume* vacuumLV = new G4LogicalVolume(vacuumSolid,          // solid
                                                      vacuumMaterial,       // material
                                                      name + "_vacuum_lv"); // name
      
      vacuumLV->SetVisAttributes(containerVisAttr);
      vacuumLV->SetUserLimits(userLimits);
      SetAcceleratorVacuumLogicalVolume(vacuumLV);
      RegisterLogicalVolume(vacuumLV);
      if (sensitiveVacuum)
        {RegisterSensitiveVolume(vacuumLV, BDSSDType::energydepvacuum);}
      
      G4PVPlacement* vacPV = new G4PVPlacement(nullptr,                     // rotation
                                               vacuumOffset,                // position
                                               vacuumLV,                    // its logical volume
                                               name + "_vacuum_pv",         // its name 
                                               containerLogicalVolume,      // its mother volume
                                               false,                       // no boolean operation
                                               0,                           // copy number
                                               checkOverlaps);
      RegisterPhysicalVolume(vacPV);
    }
}