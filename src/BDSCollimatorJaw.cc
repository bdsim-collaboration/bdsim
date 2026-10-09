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
#include "G4RotationMatrix.hh"
#include "G4Trd.hh"
#include "G4VisAttributes.hh"

#include <algorithm>
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
  tipTaperAngle(0),
  taperFlatLength(0),
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

G4double BDSCollimatorJaw::TaperedLength(G4double flatLength,
                                         G4double horizontalWidth,
                                         G4double xHalfGap,
                                         G4double xSizeLeft,
                                         G4double xSizeRight,
                                         G4bool   buildLeftJaw,
                                         G4bool   buildRightJaw,
                                         G4double taperAngle)
{
  if (!(taperAngle > 0 && taperAngle < CLHEP::halfpi))
    {return flatLength;}
  // same jaw half gaps as in the constructor and Calculations()
  G4double maxDepth = 0;
  if (buildLeftJaw && std::abs(xSizeLeft) <= 0.5*horizontalWidth)
    {
      G4double gap = BDS::IsFinite(xSizeLeft) ? xSizeLeft : xHalfGap;
      maxDepth = std::max(maxDepth, 0.5*horizontalWidth - gap);
    }
  if (buildRightJaw && std::abs(xSizeRight) <= 0.5*horizontalWidth)
    {
      G4double gap = BDS::IsFinite(xSizeRight) ? xSizeRight : xHalfGap;
      maxDepth = std::max(maxDepth, 0.5*horizontalWidth - gap);
    }
  return flatLength + 2*maxDepth / std::tan(taperAngle);
}

BDSCollimatorJaw::TaperedBox
BDSCollimatorJaw::BuildTaperedJawBox(const G4String& solidName,
                                    G4double xHalfGapThisJaw,
                                    G4int    sign,
                                    G4double depthInner,
                                    G4double depthOuter,
                                    G4double halfHeight,
                                    G4double fullChordLength) const
{
  G4double xInner = sign * (xHalfGapThisJaw + depthInner);
  G4double xOuter = sign * (xHalfGapThisJaw + depthOuter);
  G4double xCentre = 0.5 * (xInner + xOuter);

  if (!BDS::IsFinite(tipTaperAngle))
    {
      // exactly the original, untapered construction
      G4VSolid* box = new G4Box(solidName,
                                0.5 * (depthOuter - depthInner) - lengthSafety,
                                halfHeight - lengthSafety,
                                fullChordLength * 0.5 - lengthSafety);
      return TaperedBox{box, G4ThreeVector(xCentre, 0, 0), nullptr};
    }

  // Half chord length (BDSIM z, the beam direction) as a function of depth
  // into the jaw: half the flat length at the jaw edge, growing linearly with
  // depth. The component is made long enough for the deepest jaw (see
  // TaperedLength()), so this never exceeds 0.5 * fullChordLength.
  auto halfLengthAtDepth = [&](G4double depth)
    {return 0.5 * taperFlatLength + depth / std::tan(tipTaperAngle);};

  G4double dx1 = halfLengthAtDepth(depthInner) - lengthSafety;
  G4double dx2 = halfLengthAtDepth(depthOuter) - lengthSafety;
  if (dx1 < 1e-3 || dx2 < 1e-3) // 1um minimum, could also be negative
    {
      throw BDSException(__METHOD_NAME__, "tapered jaw length too small for \"" + name + "\"");
    }

  G4VSolid* trd = new G4Trd(solidName, dx1, dx2,
                            halfHeight - lengthSafety, halfHeight - lengthSafety,
                            0.5 * std::abs(depthOuter - depthInner));

  // Swap the solid's own (fixed-length) z axis with the depth (x) axis: a
  // +90 degree rotation about y sends the solid's local +z to the mother's
  // -x, and -90 degrees sends it to the mother's +x - verified numerically
  // against G4Navigator, not just derived on paper, since getting the sign
  // wrong here would silently mirror the wedge. Depth increases along +x for
  // the left jaw (sign=+1) and along -x for the right jaw (sign=-1).
  G4RotationMatrix* rot = new G4RotationMatrix();
  rot->rotateY((sign > 0 ? -90.0 : 90.0) * CLHEP::deg);

  return TaperedBox{trd, G4ThreeVector(xCentre, 0, 0), rot};
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

  if (BDS::IsFinite(tipTaperAngle))
    {
      if (tipTaperAngle <= 0 || tipTaperAngle >= CLHEP::halfpi)
        {throw BDSException(__METHOD_NAME__, "tipTaperAngle must be in (0, pi/2) radians for \"" + name + "\"");}
      if ((buildLeftJaw && jawTiltLeft != 0) || (buildRightJaw && jawTiltRight != 0))
        {throw BDSException(__METHOD_NAME__, "tipTaperAngle cannot be combined with a jaw tilt for \"" + name + "\"");}
    }

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
      G4ThreeVector leftJawPlacementPos = leftJawPos;
      G4RotationMatrix* leftJawRot = nullptr;
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
      else if (BDS::IsFinite(tipTaperAngle))
        {
          G4double leftFullDepth = 0.5 * horizontalWidth - leftJawHalfGap;
          TaperedBox tb = BuildTaperedJawBox(name + "_leftjaw_solid", leftJawHalfGap, +1,
                                             leftFullDepth - leftJawWidth, leftFullDepth,
                                             yHalfHeight, chordLength);
          leftJawSolid = tb.solid;
          leftJawPlacementPos = tb.position;
          leftJawRot = tb.rotation;
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
      G4PVPlacement* leftJawPV = new G4PVPlacement(leftJawRot,              // rotation
                                                   leftJawPlacementPos,     // position
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
      G4ThreeVector rightJawPlacementPos = rightJawPos;
      G4RotationMatrix* rightJawRot = nullptr;

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
      else if (BDS::IsFinite(tipTaperAngle))
        {
          G4double rightFullDepth = 0.5 * horizontalWidth - rightJawHalfGap;
          TaperedBox tb = BuildTaperedJawBox(name + "_rightjaw_solid", rightJawHalfGap, -1,
                                             rightFullDepth - rightJawWidth, rightFullDepth,
                                             yHalfHeight, chordLength);
          rightJawSolid = tb.solid;
          rightJawPlacementPos = tb.position;
          rightJawRot = tb.rotation;
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
      G4PVPlacement* rightJawPV = new G4PVPlacement(rightJawRot,             // rotation
                                                    rightJawPlacementPos,    // position
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