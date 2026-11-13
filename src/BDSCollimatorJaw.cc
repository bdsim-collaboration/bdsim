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
#include "BDSCollimatorJaw.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSMaterials.hh"
#include "BDSSDType.hh"
#include "BDSUtilities.hh"

#include "G4Box.hh"
#include "G4Para.hh"
#include "G4GenericTrap.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"

#include <cmath>
#include <vector>
#include <set>

BDSCollimatorJaw::BDSCollimatorJaw(const G4String&    nameIn,
                                   G4double    lengthIn,
                                   G4double    horizontalWidthIn,
                                   G4double    xHalfGapIn,
                                   G4double    yHalfHeightIn,
                                   G4double    xSizeLeftIn,
                                   G4double    xSizeRightIn,
                                   G4double    leftJawTiltIn,
                                   G4double    rightJawTiltIn,
                                   G4bool      buildLeftJawIn,
                                   G4bool      buildRightJawIn,
                                   G4Material* collimatorMaterialIn,
                                   G4Material* vacuumMaterialIn,
                                   G4Colour*   colourIn,
                                   const G4String& objectType):
BDSCollimator(nameIn, lengthIn, horizontalWidthIn, objectType, collimatorMaterialIn, vacuumMaterialIn,
              xHalfGapIn, yHalfHeightIn, xHalfGapIn, yHalfHeightIn, colourIn),
  xSizeLeft(xSizeLeftIn),
  xSizeRight(xSizeRightIn),
  xHalfGap(xHalfGapIn),
  jawTiltLeft(leftJawTiltIn),
  jawTiltRight(rightJawTiltIn),
  yHalfHeight(yHalfHeightIn),
  buildLeftJaw(buildLeftJawIn),
  buildRightJaw(buildRightJawIn),
  buildAperture(true),
  leftJawHalfGap(0),
  rightJawHalfGap(0),
  leftJawWidth(0),
  rightJawWidth(0),
  vacuumWidth(0),
  collimatorLV(nullptr)
{
  if (!BDS::IsFinite(xHalfGap) && !BDS::IsFinite(xSizeLeft) && !BDS::IsFinite(xSizeRight))
    {buildAperture = false;}

  if (!colour)
    {colour = BDSColours::Instance()->GetColour("collimator");}

  if (std::abs(xSizeLeft) > 0.5*horizontalWidth)
    {
      G4cerr << __METHOD_NAME__ << "jcol \"" << name
             << "\" left jaw offset is greater the element half width, jaw "
             << "will not be constructed" << G4endl;
      buildLeftJaw = false;
    }
  if (std::abs(xSizeRight) > 0.5*horizontalWidth)
    {
      G4cerr << __METHOD_NAME__ << "jcol \"" << name
             << "\" right jaw offset is greater the element half width, jaw "
             << "will not be constructed" << G4endl;
      buildRightJaw = false;
    }

  // set half height to half horizontal width if zero - finite height required.
  if (!BDS::IsFinite(yHalfHeight))
    {yHalfHeight = 0.5*horizontalWidth;}

  Calculations();
}

BDSCollimatorJaw::~BDSCollimatorJaw()
{;}

void BDSCollimatorJaw::Calculations()
{
  // set each jaws half gap default to aperture half size
  leftJawHalfGap = xHalfGap;
  rightJawHalfGap = xHalfGap;

  // update jaw half gap with offsets
  // if one jaw is not constructed, set the opening to xSize/2 for the aperture vacuum volume creation
  if (BDS::IsFinite(xSizeLeft))
    {leftJawHalfGap = buildLeftJaw ? xSizeLeft : 0.5 * horizontalWidth;}
  if (BDS::IsFinite(xSizeRight))
    {rightJawHalfGap = buildRightJaw ? xSizeRight : 0.5 * horizontalWidth;}

  // jaws have to fit inside containerLogicalVolume so calculate full jaw widths given offsets
  leftJawWidth = 0.5 * horizontalWidth - leftJawHalfGap;
  rightJawWidth = 0.5 * horizontalWidth - rightJawHalfGap;
  vacuumWidth = 0.5 * (leftJawHalfGap + rightJawHalfGap);

  // centre of jaw and vacuum volumes for placements
  G4double leftJawCentre = 0.5*leftJawWidth + leftJawHalfGap;
  G4double rightJawCentre = 0.5*rightJawWidth + rightJawHalfGap;
  G4double vacuumCentre = 0.5*(leftJawHalfGap - rightJawHalfGap);

  leftJawPos = G4ThreeVector(leftJawCentre, 0, 0);
  rightJawPos = G4ThreeVector(-rightJawCentre, 0, 0);
  vacuumOffset = G4ThreeVector(vacuumCentre, 0, 0);
}

void BDSCollimatorJaw::CheckParameters()
{
  // BDSCollimator::CheckParameters() <- we replace this and don't call it - 'tapered' is never set
  G4double totalGap = leftJawHalfGap + rightJawHalfGap;
  if (totalGap < 1e-3 && buildAperture) // 1um minimum, could also be negative
    {throw BDSException(__METHOD_NAME__, "gap too small (<1um) for \"" + name + "\"");}

  if (horizontalWidth - 2*lengthSafetyLarge < totalGap)
    {throw BDSException(__METHOD_NAME__, "horizontalWidth too small for the total gap width in \"" + name + "\"");}

  if (BDS::IsFinite(yHalfHeight) && (yHalfHeight < 1e-3)) // 1um minimum
    {throw BDSException(__METHOD_NAME__, "insufficient ysize for \"" + name + "\"");}

  if (!buildLeftJaw && !buildRightJaw)
    {throw BDSException(__METHOD_NAME__, "no jaws being built for \"" + name + "\"");}

  // the remaining checks only apply to the jaw and vacuum geometry
  if (!buildAperture)
    {return;}

  // jaw solids have a half width of jawWidth/2 - lengthSafety - for jcoltip this is the bulk
  // jaw width after the space for the tip has been removed in the derived class
  if (buildLeftJaw && (leftJawWidth * 0.5 - lengthSafety < 1e-3)) // 1um minimum, could also be negative
    {throw BDSException(__METHOD_NAME__, "left jaw too thin given horizontalWidth and aperture for \"" + name + "\"");}
  if (buildRightJaw && (rightJawWidth * 0.5 - lengthSafety < 1e-3)) // 1um minimum, could also be negative
    {throw BDSException(__METHOD_NAME__, "right jaw too thin given horizontalWidth and aperture for \"" + name + "\"");}

  if (std::abs(jawTiltLeft) > 0.5*CLHEP::halfpi)
    {throw BDSException(__METHOD_NAME__, "|jawTiltLeft| is over pi/4 radians for \"" + name + "\"");}
  if (std::abs(jawTiltRight) > 0.5*CLHEP::halfpi)
    {throw BDSException(__METHOD_NAME__, "|jawTiltRight| is over pi/4 radians for \"" + name + "\"");}

  // shift of each jaw face at the ends of the element due to its tilt - tilt is ignored for a
  // jaw that isn't built - uses the half gaps from Calculations(), which is called in the constructor
  G4double tiltShiftLeft  = buildLeftJaw  ? std::tan(jawTiltLeft)  * chordLength * 0.5 : 0;
  G4double tiltShiftRight = buildRightJaw ? std::tan(jawTiltRight) * chordLength * 0.5 : 0;

  G4double gapIn = totalGap - tiltShiftLeft + tiltShiftRight;
  G4double gapOut = totalGap + tiltShiftLeft - tiltShiftRight;
  if (gapIn <= 0 || gapOut <= 0)
    {throw BDSException(__METHOD_NAME__, "the tilts plus centre gap will cause the jaws to collide in \"" + name + "\"");}

  // vacuum full width at each end of the element - see vacuum construction in Build()
  if (std::min(gapIn, gapOut) * 0.5 - lengthSafety < 1e-3) // 1um minimum
    {throw BDSException(__METHOD_NAME__, "insufficient aperture between jaws in \"" + name + "\"");}
}

void BDSCollimatorJaw::BuildContainerLogicalVolume()
{
  G4double horizontalHalfWidth = horizontalWidth * 0.5;
  if (jawTiltLeft != 0 || jawTiltRight != 0)
    {
      // The box must encompass everything, so pick the largest absolute angle
      G4double maxTilt = std::max(std::abs(jawTiltLeft), std::abs(jawTiltRight));
      horizontalHalfWidth = horizontalWidth * 0.5 + chordLength * 0.5 * std::sin(maxTilt);
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

void BDSCollimatorJaw::Build()
{
  CheckParameters();
  BDSAcceleratorComponent::Build(); // calls BuildContainer and sets limits and vis for container

  G4VisAttributes* collimatorVisAttr = new G4VisAttributes(*colour);
  RegisterVisAttributes(collimatorVisAttr);

  // get appropriate user limits for jaw material
  G4UserLimits* collUserLimits = CollimatorUserLimits();

  // build jaws as appropriate
  if (buildLeftJaw && buildAperture)
    {
      G4VSolid* leftJawSolid = nullptr;
      if (jawTiltLeft != 0)
        {
          // Adjust the length of the parallelepiped to match the inside edges in Z
          // Due to the straight parallelepiped edges, it will never match the volume an angled box,
          // so it is chosen to underestimate the volume, but preserve the jaw x-y cutting plane.
          G4double leftHalfLength = chordLength * 0.5 * std::cos(jawTiltLeft);
          
          leftJawSolid = new G4Para(name + "_leftjaw_solid",
                                    leftJawWidth * 0.5 - lengthSafety,
                                    yHalfHeight - lengthSafety,
                                    leftHalfLength - lengthSafety,
                                    0, jawTiltLeft, 0);
        }
      else
        {
          leftJawSolid = new G4Box(name + "_leftjaw_solid",
                                   leftJawWidth * 0.5 - lengthSafety,
                                   yHalfHeight - lengthSafety,
                                   chordLength * 0.5 - lengthSafety);
        }
      
      RegisterSolid(leftJawSolid);
      
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
      
      // place the jaw
      G4PVPlacement* leftJawPV = new G4PVPlacement(nullptr,              // rotation
                                                   leftJawPos,              // position
                                                   leftJawLV,               // its logical volume
                                                   name + "_leftjaw_pv",    // its name
                                                   containerLogicalVolume,  // its mother volume
                                                   false,                            // no boolean operation
                                                   0,                            // copy number
                                                   checkOverlaps);
      RegisterPhysicalVolume(leftJawPV);
    }
  if (buildRightJaw && buildAperture)
    {
      G4VSolid* rightJawSolid = nullptr;
      
      if (jawTiltRight != 0)
        {
          // Adjust the length of the parallelepiped to match the inside edges in Z
          // Due to the straight parallelepiped edges, it will never match the volume an angled box,
          // so it is chosen to underestimate the volume, but preserve the jaw x-y cutting plane.
          G4double rightHalfLength = chordLength * 0.5 * std::cos(jawTiltRight);

          rightJawSolid = new G4Para(name + "_rightjaw_solid",
                                     rightJawWidth * 0.5 - lengthSafety,
                                     yHalfHeight - lengthSafety,
                                     rightHalfLength - lengthSafety,
                                     0, jawTiltRight, 0);
        }
      else
        {
          rightJawSolid = new G4Box(name + "_rightjaw_solid",
                                    rightJawWidth * 0.5 - lengthSafety,
                                    yHalfHeight - lengthSafety,
                                    chordLength * 0.5 - lengthSafety);
        }

      RegisterSolid(rightJawSolid);
      
      G4LogicalVolume* rightJawLV = new G4LogicalVolume(rightJawSolid,      // solid
                                                        collimatorMaterial,     // material
                                                        name + "_rightjaw_lv"); // name
      rightJawLV->SetVisAttributes(collimatorVisAttr);
      rightJawLV->SetUserLimits(collUserLimits);
      RegisterLogicalVolume(rightJawLV);
      BDSAcceleratorModel::Instance()->VolumeSet("collimators")->insert(rightJawLV);
      if (sensitiveOuter)
        {RegisterSensitiveVolume(rightJawLV, BDSSDType::collimatorcomplete);}
      
      // place the jaw
      G4PVPlacement* rightJawPV = new G4PVPlacement(nullptr,             // rotation
                                                    rightJawPos,             // position
                                                    rightJawLV,              // its logical volume
                                                    name + "_rightjaw_pv",   // its name
                                                    containerLogicalVolume,  // its mother volume
                                                    false,                           // no boolean operation
                                                    0,                           // copy number
                                                    checkOverlaps);
      RegisterPhysicalVolume(rightJawPV);
    }
  // if no aperture but the code has got to this stage, build the collimator as a simple box.
  if (!buildAperture)
    {
      collimatorSolid = new G4Box(name + "_block_solid",
                                  horizontalWidth * 0.5 - lengthSafety,
                                  yHalfHeight - lengthSafety,
                                  chordLength * 0.5 - lengthSafety);
      RegisterSolid(collimatorSolid);
      
      collimatorLV = new G4LogicalVolume(collimatorSolid, collimatorMaterial, name + "_block_lv");
      collimatorLV->SetVisAttributes(collimatorVisAttr);
      collimatorLV->SetUserLimits(collUserLimits);
      RegisterLogicalVolume(collimatorLV);
      BDSAcceleratorModel::Instance()->VolumeSet("collimators")->insert(collimatorLV);
      if (sensitiveOuter)
        {RegisterSensitiveVolume(collimatorLV, BDSSDType::collimatorcomplete);}
      
      // place the jaw
      G4PVPlacement* collimatorPV = new G4PVPlacement(nullptr,                 // rotation
                                                      (G4ThreeVector) 0,       // position
                                                      collimatorLV,            // its logical volume
                                                      name + "_pv",                        // its name
                                                      containerLogicalVolume,  // its mother volume
                                                      false,                       // no boolean operation
                                                      0,                               // copy number
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

          G4double tiltShiftLeftDownstream  = std::tan(tiltLeft)  * chordLength * 0.5;
          G4double tiltShiftRightDownstream = std::tan(tiltRight) * chordLength * 0.5;

          G4double xGapLeftUpstream = leftJawHalfGap - tiltShiftLeftDownstream;
          G4double xGapLeftDownstream = leftJawHalfGap + tiltShiftLeftDownstream;
          G4double xGapRightUpstream = -rightJawHalfGap - tiltShiftRightDownstream;
          G4double xGapRightDownstream = -rightJawHalfGap + tiltShiftRightDownstream;

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
          // For tilted jaws, the vacuum trapezoid is constructed from absolute coordinates
          // so need to zero the vacuum offset, which is intended for a box.
          vacuumOffset = G4ThreeVector(0, 0, 0);
        }
      else
        {
          vacuumSolid = new G4Box(name + "_vacuum_solid",               // name
                                  vacuumWidth - lengthSafety,           // x half width
                                  yHalfHeight - lengthSafety,           // y half width
                                  chordLength * 0.5);                   // z half length
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
      
      G4PVPlacement* vacPV = new G4PVPlacement(nullptr,                 // rotation
                                               vacuumOffset,            // position
                                               vacuumLV,                // its logical volume
                                               name + "_vacuum_pv",     // its name
                                               containerLogicalVolume,  // its mother  volume
                                               false,                   // no boolean operation
                                               0,                       // copy number
                                               checkOverlaps);
      RegisterPhysicalVolume(vacPV);
    }
}