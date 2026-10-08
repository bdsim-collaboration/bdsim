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
#include "BDSAcceleratorModel.hh"
#include "BDSCollimatorJawTip.hh"
#include "BDSColourFromMaterial.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSCollimatorJaw.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSSDType.hh"
#include "BDSUtilities.hh"

#include "G4Box.hh"
#include "G4Para.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4VisAttributes.hh"

#include <cmath>
#include <set>

BDSCollimatorJawTip::BDSCollimatorJawTip(const G4String&    nameIn,
                                         G4double    lengthIn,
                                         G4double    horizontalWidthIn,
                                         G4double    xHalfGapIn,
                                         G4double    yHalfHeightIn,
                                         G4double    xSizeLeftIn,
                                         G4double    xSizeRightIn,
                                         G4double    leftJawTiltIn,
                                         G4double    rightJawTiltIn,
                                         G4double    tipThicknessIn,
                                         G4double    tipTaperAngleIn,
                                         G4bool      buildLeftJawIn,
                                         G4bool      buildRightJawIn,
                                         G4Material* collimatorMaterialIn,
                                         G4Material* collimatorTipMaterialIn,
                                         G4Material* vacuumMaterialIn,
                                         G4Colour*   colourIn,
                                         G4Colour*   tipColourIn):
BDSCollimatorJaw(nameIn, lengthIn, horizontalWidthIn, xHalfGapIn, yHalfHeightIn, xSizeLeftIn, xSizeRightIn,
                 leftJawTiltIn, rightJawTiltIn, buildLeftJawIn, buildRightJawIn, collimatorMaterialIn,
                 vacuumMaterialIn, colourIn, "jcoltip"),
  tipColour(tipColourIn),
  tipThickness(tipThicknessIn),
  collimatorTipMaterial(collimatorTipMaterialIn)
{
  tipTaperAngle = tipTaperAngleIn;
  if (!tipColour)
    {
      G4Colour* defaultTipColour = BDSColours::Instance()->GetColour("collimatorTip");
      tipColour = BDSColourFromMaterial::Instance()->GetColourWithDefault(collimatorTipMaterialIn, defaultTipColour);
    }
  UpdateCalculations();
}

BDSCollimatorJawTip::~BDSCollimatorJawTip()
{;}

void BDSCollimatorJawTip::UpdateCalculations()
{
  BDSCollimatorJaw::Calculations();

  // shrink the bulk jaws from the aperture side to make space for the tips, so the
  // centre of each jaw moves outwards by half the space taken
  G4double tipSpace = tipThickness + 2*lengthSafety;
  leftJawWidth -= tipSpace;
  leftJawPos += G4ThreeVector(0.5*tipSpace, 0, 0);
  rightJawWidth -= tipSpace;
  rightJawPos -= G4ThreeVector(0.5*tipSpace, 0, 0);

  leftJawTipPos = G4ThreeVector(leftJawHalfGap + 0.5*tipThickness, 0, 0);
  rightJawTipPos = G4ThreeVector(-(rightJawHalfGap + 0.5*tipThickness), 0, 0);
}

void BDSCollimatorJawTip::CheckParametersForTips()
{
  // the remaining checks only apply to the jaw and vacuum geometry
  if (!buildAperture)
    {
      if (tipThickness > (0.5*horizontalWidth-4*lengthSafety))
        {throw BDSException(__METHOD_NAME__, "2x tipThickness is greater than the horizontal width for zero opening jcoltip \"" + name + "\"");}
      return;
    }

  // tip solids have a half width of tipThickness/2 - lengthSafety
  if (tipThickness * 0.5 - lengthSafety < 1e-3) // 1um minimum, could also be negative
    {throw BDSException(__METHOD_NAME__, "insufficient tipThickness for jcoltip \"" + name + "\"");}

  // bulk jaw solids have a half width of (jawWidth - tipThickness)/2 - lengthSafety
  if (buildLeftJaw && ((0.5 * horizontalWidth - leftJawHalfGap - tipThickness) * 0.5 - lengthSafety < 1e-3))
    {throw BDSException(__METHOD_NAME__, "left jaw of jcoltip \"" + name + "\" too thin given horizontalWidth, aperture and tipThickness");}
  if (buildRightJaw && ((0.5 * horizontalWidth - rightJawHalfGap - tipThickness) * 0.5 - lengthSafety < 1e-3))
    {throw BDSException(__METHOD_NAME__, "right jaw of jcoltip \"" + name + "\" too thin given horizontalWidth, aperture and tipThickness");}
}

void BDSCollimatorJawTip::Build()
{
  CheckParametersForTips();
  BDSCollimatorJaw::Build();
  BuildTips();
}

void BDSCollimatorJawTip::BuildTips()
{
  G4VisAttributes* tipVisAttr = new G4VisAttributes(*tipColour);
  RegisterVisAttributes(tipVisAttr);

  G4UserLimits* tipCollUserLimits = CollimatorUserLimits();

  // If not aperture, then the base class just fills the container with one material. Now,
  // place a tip material block inside that block.
  if (!buildAperture && tipThickness > 1e-3)
    {
      G4VSolid* tipSolid = new G4Box(name + "_tip_solid",
                                     tipThickness,
                                     yHalfHeight - 2*lengthSafety,
                                     chordLength * 0.5 - 2*lengthSafety);
      // the containerLV is of length chordLength * 0.5 - lengthSafety
      RegisterSolid(tipSolid);

      G4LogicalVolume* collimatorTipLV = new G4LogicalVolume(tipSolid, collimatorTipMaterial, name + "_tip_lv");
      collimatorTipLV->SetVisAttributes(tipVisAttr);
      collimatorTipLV->SetUserLimits(tipCollUserLimits);

      // register with base class (BDSGeometryComponent)
      RegisterLogicalVolume(collimatorTipLV);
      BDSAcceleratorModel::Instance()->VolumeSet("collimators")->insert(collimatorTipLV);
      if (sensitiveOuter)
        {RegisterSensitiveVolume(collimatorTipLV, BDSSDType::collimatorcomplete);}

      // place the jaw
      G4PVPlacement* collimatorTipPV = new G4PVPlacement(nullptr,                 // rotation
                                                         (G4ThreeVector) 0,       // position
                                                         collimatorTipLV,         // its logical volume
                                                         name + "_tip_pv",            // its name
                                                         collimatorLV,            // its mother volume
                                                         false,                   // no boolean operation
                                                         0,                       // copy number
                                                         checkOverlaps);
      RegisterPhysicalVolume(collimatorTipPV);
      return;
    }

  G4VSolid* leftJawTipSolid = nullptr;
  if (buildLeftJaw && buildAperture)
    {
      G4ThreeVector leftJawTipPlacementPos = leftJawTipPos;
      G4RotationMatrix* leftJawTipRot = nullptr;
      if (jawTiltLeft != 0)
        {
          G4double leftHalfLength = chordLength * 0.5 * std::cos(jawTiltLeft);
          leftJawTipSolid = new G4Para(name + "_leftjawtip_solid",
                                       tipThickness * 0.5 - lengthSafety,
                                       yHalfHeight - lengthSafety,
                                       leftHalfLength - lengthSafety,
                                       0, jawTiltLeft, 0);
        }
      else if (BDS::IsFinite(tipTaperAngle))
        {
          // Same taper law, and the same fullDepth (jaw outer edge), as the
          // bulk jaw built in BDSCollimatorJaw::Build() - the two solids meet
          // exactly at depth = tipThickness with no gap or overlap.
          G4double leftFullDepth = 0.5 * horizontalWidth - leftJawHalfGap;
          TaperedBox tb = BuildTaperedJawBox(name + "_leftjawtip_solid", leftJawHalfGap, +1,
                                             0, tipThickness, leftFullDepth,
                                             yHalfHeight, chordLength);
          leftJawTipSolid = tb.solid;
          leftJawTipPlacementPos = tb.position;
          leftJawTipRot = tb.rotation;
        }
      else
        {
          leftJawTipSolid = new G4Box(name + "_leftjawtip_solid",
                                     tipThickness * 0.5 - lengthSafety,
                                     yHalfHeight - lengthSafety,
                                      chordLength * 0.5 - lengthSafety);
        }
      RegisterSolid(leftJawTipSolid);

      G4LogicalVolume* leftJawTipLV = new G4LogicalVolume(leftJawTipSolid, collimatorTipMaterial, name + "_leftjawtip_lv");
      leftJawTipLV->SetVisAttributes(tipVisAttr);
      leftJawTipLV->SetUserLimits(tipCollUserLimits);
      RegisterLogicalVolume(leftJawTipLV);
      BDSAcceleratorModel::Instance()->VolumeSet("collimators")->insert(leftJawTipLV);
      if (sensitiveOuter)
        {RegisterSensitiveVolume(leftJawTipLV, BDSSDType::collimatorcomplete);}

      // place the tip
      G4PVPlacement* leftJawTipPV = new G4PVPlacement(leftJawTipRot,
                                                      leftJawTipPlacementPos,
                                                      leftJawTipLV,
                                                      name + "_leftjawtip_pv",
                                                      containerLogicalVolume,
                                                      false,               // no boolean operation
                                                      0,                   // copy number
                                                      checkOverlaps);
      RegisterPhysicalVolume(leftJawTipPV);
    }

  G4VSolid* rightJawTipSolid = nullptr;
  if (buildRightJaw && buildAperture)
    {
      G4ThreeVector rightJawTipPlacementPos = rightJawTipPos;
      G4RotationMatrix* rightJawTipRot = nullptr;
      if (jawTiltRight != 0)
        {
          G4double rightHalfLength = chordLength * 0.5 * std::cos(jawTiltRight);
          rightJawTipSolid = new G4Para(name + "_rightjawtip_solid",
                                        tipThickness * 0.5 - lengthSafety,
                                        yHalfHeight - lengthSafety,
                                        rightHalfLength - lengthSafety,
                                        0, jawTiltRight, 0);
        }
      else if (BDS::IsFinite(tipTaperAngle))
        {
          G4double rightFullDepth = 0.5 * horizontalWidth - rightJawHalfGap;
          TaperedBox tb = BuildTaperedJawBox(name + "_rightjawtip_solid", rightJawHalfGap, -1,
                                             0, tipThickness, rightFullDepth,
                                             yHalfHeight, chordLength);
          rightJawTipSolid = tb.solid;
          rightJawTipPlacementPos = tb.position;
          rightJawTipRot = tb.rotation;
        }
      else
        {
          rightJawTipSolid = new G4Box(name + "_rightjawtip_solid",
                                       tipThickness * 0.5 - lengthSafety,
                                       yHalfHeight - lengthSafety,
                                       chordLength * 0.5 - lengthSafety);
        }
      RegisterSolid(rightJawTipSolid);

      G4LogicalVolume* rightJawTipLV = new G4LogicalVolume(rightJawTipSolid, collimatorTipMaterial, name + "_rightjawtip_lv");
      rightJawTipLV->SetVisAttributes(tipVisAttr);
      rightJawTipLV->SetUserLimits(tipCollUserLimits);
      RegisterLogicalVolume(rightJawTipLV);
      BDSAcceleratorModel::Instance()->VolumeSet("collimators")->insert(rightJawTipLV);
      if (sensitiveOuter)
        {RegisterSensitiveVolume(rightJawTipLV, BDSSDType::collimatorcomplete);}

      // place the tip
      G4PVPlacement* rightJawTipPV = new G4PVPlacement(rightJawTipRot,
                                                       rightJawTipPlacementPos,
                                                       rightJawTipLV,
                                                       name + "_rightjawtip_pv",
                                                       containerLogicalVolume,
                                                       false,           // no boolean operation
                                                       0,               // copy number
                                                       checkOverlaps);
      RegisterPhysicalVolume(rightJawTipPV);
    }
}