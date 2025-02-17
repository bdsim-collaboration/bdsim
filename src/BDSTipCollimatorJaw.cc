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
#include "BDSTipCollimatorJaw.hh"
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

BDSTipCollimatorJaw::BDSTipCollimatorJaw(const G4String&    nameIn,
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
BDSTipCollimator(nameIn, lengthIn, horizontalWidthIn, "tipjcol", collimatorMaterialIn, collimatorTipMaterialIn, vacuumMaterialIn,
              xHalfGapIn, yHalfHeightIn, xHalfGapIn, yHalfHeightIn, colourIn, tipColourIn, tipThicknessIn),
  jawSolid(nullptr),
  xSizeLeft(xSizeLeftIn),
  xSizeRight(xSizeRightIn),
  xHalfGap(xHalfGapIn),
  jawTiltLeft(leftJawTiltIn),
  jawTiltRight(rightJawTiltIn),
  jawHalfWidth(0),
  yHalfHeight(yHalfHeightIn),
  buildLeftJaw(buildLeftJawIn),
  buildRightJaw(buildRightJawIn),
  buildAperture(true)
{
  jawHalfWidth = 0.5 * (0.5*horizontalWidth - lengthSafetyLarge - xHalfGap);
}

BDSTipCollimatorJaw::~BDSTipCollimatorJaw()
{;}

void BDSTipCollimatorJaw::CheckParameters()
{
  if (!colour)
    {colour = BDSColours::Instance()->GetColour("collimator");}
  if (!tipColour)
    {tipColour = BDSColours::Instance()->GetColour("collimatorTip");}
  
  if (jawHalfWidth < 1e-3) // 1um minimum, could also be negative
    {throw BDSException(__METHOD_NAME__, "horizontalWidth insufficient given xsize of tipjcol \"" + name + "\"");}

  if (!BDS::IsFinite(yHalfHeight))
    {yHalfHeight = 0.5*horizontalWidth;}

  if (BDS::IsFinite(yHalfHeight) && (yHalfHeight < 1e-3)) // 1um minimum
    {throw BDSException(__METHOD_NAME__, "insufficient ysize for tipjcol \"" + name + "\"");}

  if ((yHalfHeight < 0) || ((yHalfHeight > 0) && (yHalfHeight < 1e-3))) // 1um minimum and not negative
    {throw BDSException(__METHOD_NAME__, "insufficient ysize for tipjcol \"" + name + "\"");}

  if (xSizeLeft < 0)
    {throw BDSException(__METHOD_NAME__, "left tipjcol jaw cannot have negative half aperture size: \"" + name + "\"");}
  if (xSizeRight < 0)
    {throw BDSException(__METHOD_NAME__, "left tipjcol jaw cannot have negative half aperture size: \"" + name + "\"");}

  if (std::abs(xSizeLeft) > 0.5*horizontalWidth)
    {
      G4cerr << __METHOD_NAME__ << "tipjcol \"" << name
             << "\" left jaw offset is greater the element half width, jaw "
             << "will not be constructed" << G4endl;
      buildLeftJaw = false;
    }
  if (std::abs(xSizeRight) > 0.5*horizontalWidth)
    {
      G4cerr << __METHOD_NAME__ << "tipjcol \"" << name
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

void BDSTipCollimatorJaw::BuildContainerLogicalVolume()
{
  G4double horizontalHalfWidth = horizontalWidth * 0.5;
  if (jawTiltLeft != 0 || jawTiltRight != 0)
    {
      horizontalHalfWidth = horizontalWidth * 0.5 + chordLength * 0.5 * std::sin(std::max(std::abs(jawTiltLeft), std::abs(jawTiltRight)));
    }
  
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

void BDSTipCollimatorJaw::Build()
{
  CheckParameters();
  BDSAcceleratorComponent::Build();

  G4double leftJawHalfGap = xHalfGap;
  G4double rightJawHalfGap = xHalfGap;

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

  G4double leftJawWidth = 0.5 * horizontalWidth - leftJawHalfGap;
  G4double rightJawWidth = 0.5 * horizontalWidth - rightJawHalfGap;
  G4double vacuumWidth = 0.5 * (leftJawHalfGap + rightJawHalfGap);

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

  G4UserLimits* collUserLimits = CollimatorUserLimits();

  if (buildLeftJaw && buildAperture)
    {
      G4VSolid* leftJawSolid = nullptr;
      G4VSolid* leftJawTipSolid = nullptr;
      
      if (jawTiltLeft != 0)
        {
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
      
      G4LogicalVolume* leftJawLV = new G4LogicalVolume(leftJawSolid,
                                                       collimatorMaterial,
                                                       name + "_leftjaw_lv");
      leftJawLV->SetVisAttributes(collimatorVisAttr);
      leftJawLV->SetUserLimits(collUserLimits);
      RegisterLogicalVolume(leftJawLV);
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
      
      G4PVPlacement* leftJawTipPV = new G4PVPlacement(nullptr,
                                                      leftJawPos - G4ThreeVector((leftJawWidth - tipThickness) * 0.5, 0, 0),
                                                      leftJawTipLV,
                                                      name + "_leftjawtip_pv",
                                                      containerLogicalVolume,
                                                      false,
                                                      0,
                                                      checkOverlaps);
      RegisterPhysicalVolume(leftJawTipPV);
      
      G4PVPlacement* leftJawPV = new G4PVPlacement(nullptr,
                                                   leftJawPos + G4ThreeVector(tipThickness * 0.5, 0, 0),
                                                   leftJawLV,
                                                   name + "_leftjaw_pv",
                                                   containerLogicalVolume,
                                                   false,
                                                   1,
                                                   checkOverlaps);
      RegisterPhysicalVolume(leftJawPV);
    }
  if (buildRightJaw && buildAperture)
    {
      G4VSolid* rightJawSolid = nullptr;
      G4VSolid* rightJawTipSolid = nullptr;
      
      if (jawTiltRight != 0)
        {
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
      rightJawLV->SetUserLimits(collUserLimits);
      RegisterLogicalVolume(rightJawLV);
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
      
      G4PVPlacement* rightJawTipPV = new G4PVPlacement(nullptr,
                                                       rightJawPos + G4ThreeVector((rightJawWidth - tipThickness) * 0.5, 0, 0),
                                                       rightJawTipLV,
                                                       name + "_rightjawtip_pv",
                                                       containerLogicalVolume,
                                                       false,
                                                       0,
                                                       checkOverlaps);
      RegisterPhysicalVolume(rightJawTipPV);
      
      G4PVPlacement* rightJawPV = new G4PVPlacement(nullptr,
                                                    rightJawPos - G4ThreeVector(tipThickness * 0.5, 0, 0),
                                                    rightJawLV,
                                                    name + "_rightjaw_pv",
                                                    containerLogicalVolume,
                                                    false,
                                                    1,
                                                    checkOverlaps);
      RegisterPhysicalVolume(rightJawPV);
    }
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
      collimatorLV->SetUserLimits(userLimits);
      RegisterLogicalVolume(collimatorLV);
      if (sensitiveOuter)
        {RegisterSensitiveVolume(collimatorLV, BDSSDType::collimatorcomplete);}
      
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
  
  if (buildAperture)
    {
      if (jawTiltLeft != 0 || jawTiltRight != 0)
        {
          G4double tiltLeft = buildLeftJaw ? jawTiltLeft : 0.;
          G4double tiltRight = buildRightJaw ? jawTiltRight : 0.;
          
          G4double halfLengthLeftEff = (chordLength  * 0.5) / std::cos(tiltLeft);
          G4double halfLengthRightEff = (chordLength  * 0.5) / std::cos(tiltRight);

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
          vacuumOffset = G4ThreeVector(0, 0, 0);
        }
      else
        {
          vacuumSolid = new G4Box(name + "_vacuum_solid",
                                  vacuumWidth - lengthSafety,
                                  yHalfHeight - lengthSafety,
                                  chordLength * 0.5);
        }
      
      RegisterSolid(vacuumSolid);
      
      G4LogicalVolume* vacuumLV = new G4LogicalVolume(vacuumSolid,
                                                      vacuumMaterial,
                                                      name + "_vacuum_lv");
      
      vacuumLV->SetVisAttributes(containerVisAttr);
      vacuumLV->SetUserLimits(userLimits);
      SetAcceleratorVacuumLogicalVolume(vacuumLV);
      RegisterLogicalVolume(vacuumLV);
      if (sensitiveVacuum)
        {RegisterSensitiveVolume(vacuumLV, BDSSDType::energydepvacuum);}
      
      G4PVPlacement* vacPV = new G4PVPlacement(nullptr,
                                               vacuumOffset,
                                               vacuumLV,
                                               name + "_vacuum_pv",
                                               containerLogicalVolume,
                                               false,
                                               0,
                                               checkOverlaps);
      RegisterPhysicalVolume(vacPV);
    }
}