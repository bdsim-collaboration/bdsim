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
#include "BDSBeamPipeInfo.hh"
#include "BDSTipCollimator.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSMaterials.hh"
#include "BDSSDType.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

#include <map>

BDSTipCollimator::BDSTipCollimator(const G4String& nameIn,
                                   G4double        lengthIn,
                                   G4double        horizontalWidthIn,
                                   const G4String& typeIn,
                                   G4Material* collimatorMaterialIn,
                                   G4Material* collimatorTipMaterialIn,
                                   G4Material* vacuumMaterialIn,
                                   G4double    xApertureIn,
                                   G4double    yApertureIn,
                                   G4double    xApertureOutIn,
                                   G4double    yApertureOutIn,
                                   G4Colour*   colourIn,
                                   G4Colour*   tipColourIn,
                                   G4double    tipThicknessIn,
                                   G4bool      circularOuterIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, typeIn),
  collimatorSolid(nullptr),
  innerSolid(nullptr),
  vacuumSolid(nullptr),
  horizontalWidth(horizontalWidthIn),
  collimatorMaterial(collimatorMaterialIn),
  collimatorTipMaterial(collimatorTipMaterialIn),
  vacuumMaterial(vacuumMaterialIn),
  xAperture(xApertureIn),
  yAperture(yApertureIn),
  xApertureOut(xApertureOutIn),
  yApertureOut(yApertureOutIn),
  tapered(false),
  colour(colourIn),
  tipColour(tipColourIn),
  tipThickness(tipThicknessIn),
  minKineticEnergy(0),
  circularOuter(circularOuterIn)
{;}

BDSTipCollimator::~BDSTipCollimator()
{;}

void BDSTipCollimator::CheckParameters()
{
  if ((xAperture > 0.5 * horizontalWidth) || (yAperture > 0.5 * horizontalWidth))
    {
      G4cerr << __METHOD_NAME__ << "half aperture bigger than width or height!" << G4endl;
      G4cerr << "Full horizontal width is " << horizontalWidth << " mm for component named: \""
             << name << "\"" << G4endl;
      G4cerr << "x (half) aperture " << xAperture << " mm, y (half) aperture " << yAperture << " mm" << G4endl;
      throw BDSException(__METHOD_NAME__, "Error in collimator");
    }

  if ((xApertureOut > 0.5 * horizontalWidth) || (yApertureOut > 0.5 * horizontalWidth))
    {
      G4cerr << __METHOD_NAME__ << "half aperture exit bigger than width or height!" << G4endl;
      G4cerr << "Full horizontal width is " << horizontalWidth << " mm for component named: \""
             << name << "\"" << G4endl;
      G4cerr << "x (half) aperture " << xApertureOut << " mm, y (half) aperture " << yApertureOut << " mm" << G4endl;
      throw BDSException(__METHOD_NAME__, "Error in collimator");
    }

  if (BDS::IsFinite(xApertureOut) && (xAperture <= 0))
    {BDS::Warning(__METHOD_NAME__, "element: \"" + name + "\": no entrance aperture set for collimator - exit aperture parameters will be ignored");}

  tapered = (BDS::IsFinite(xApertureOut) && BDS::IsFinite(yApertureOut));
  if (!tapered)
    {// copy for consistency in output and aperture prediction
      xApertureOut = xAperture;
      yApertureOut = yAperture;
    }

  if (!colour)
    {colour = BDSColours::Instance()->GetColour("collimator");}
  if (!tipColour)
    {tipColour = BDSColours::Instance()->GetColour("collimatorTip");}
}

G4String BDSTipCollimator::Material() const
{
  if (collimatorMaterial)
    {return collimatorMaterial->GetName();}
  else
    {return "none";}
}

void BDSTipCollimator::BuildContainerLogicalVolume()
{
  if (circularOuter)
    {
      containerSolid = new G4Tubs(name + "_solid",
				  0,
				  0.5*horizontalWidth,
				  0.5*chordLength,
				  0,
				  CLHEP::twopi);
    }
  else
    {
      containerSolid = new G4Box(name + "_container_solid",
				 horizontalWidth*0.5,
				 horizontalWidth*0.5,
				 chordLength*0.5);
    }
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
  BDSExtent ext(horizontalWidth * 0.5, horizontalWidth * 0.5,
                chordLength * 0.5);
  SetExtent(ext);
}

void BDSTipCollimator::Build()
{
  CheckParameters();
  BDSAcceleratorComponent::Build(); // calls BuildContainer and sets limits and vis for container

  // Swap variables around if exit size is larger than entrance size
  // Rotation for tapered collimator (needed for tapered elliptical collimator)
  G4bool isOutLarger = ((xApertureOut > xAperture) && (yApertureOut > yAperture));
  G4RotationMatrix* colRotate;
  if (tapered && isOutLarger)
    {
      std::swap(xAperture,xApertureOut);
      std::swap(yAperture,yApertureOut);
      colRotate = new G4RotationMatrix;
      colRotate->rotateX(CLHEP::pi);
      RegisterRotationMatrix(colRotate);
    }
  else
    {colRotate = nullptr;}

  G4VSolid* outerSolid;
  if (circularOuter)
    {
      outerSolid = new G4Tubs(name + "_outer_solid",
			      0,
			      horizontalWidth * 0.5 - lengthSafety,
			      chordLength * 0.5 - lengthSafety,
			      0,
			      CLHEP::twopi);
    }
  else
    {
      outerSolid = new G4Box(name + "_outer_solid",
                             horizontalWidth * 0.5 - lengthSafety,
                             horizontalWidth * 0.5 - lengthSafety,
                             chordLength * 0.5 - lengthSafety);
    }
  RegisterSolid(outerSolid);
  
  G4bool buildVacuumAndAperture = (BDS::IsFinite(xAperture) && BDS::IsFinite(yAperture));

  // only do subtraction if aperture actually set
  if (buildVacuumAndAperture)
    {
      BuildInnerCollimator();

      collimatorSolid = new G4SubtractionSolid(name + "_collimator_solid", // name
                                               outerSolid,                 // solid 1
                                               innerSolid);                // minus solid 2
      RegisterSolid(collimatorSolid);
    }
  else
    {collimatorSolid = outerSolid;}
  
  G4LogicalVolume* collimatorLV = new G4LogicalVolume(collimatorSolid,          // solid
                                                      collimatorMaterial,       // material
                                                      name + "_collimator_lv"); // name

  // register it in a set of collimator logical volumes
  BDSAcceleratorModel::Instance()->VolumeSet("collimators")->insert(collimatorLV);
  
  G4VisAttributes* collimatorVisAttr = new G4VisAttributes(*colour);
  collimatorLV->SetVisAttributes(collimatorVisAttr);
  RegisterVisAttributes(collimatorVisAttr);

  collimatorLV->SetUserLimits(CollimatorUserLimits());

  // register with base class (BDSGeometryComponent)
  RegisterLogicalVolume(collimatorLV);
  if (sensitiveOuter)
    {RegisterSensitiveVolume(collimatorLV, BDSSDType::collimatorcomplete);}

  G4PVPlacement* collPV = new G4PVPlacement(colRotate,               // rotation
                                            G4ThreeVector(),         // position
                                            collimatorLV,            // its logical volume
                                            name + "_collimator_pv", // its name
                                            containerLogicalVolume,  // its mother  volume
                                            false,		     // no boolean operation
                                            0,		             // copy number
                                            checkOverlaps);

  RegisterPhysicalVolume(collPV);

  if (buildVacuumAndAperture)
    {
      G4LogicalVolume* vacuumLV = new G4LogicalVolume(vacuumSolid,          // solid
                                                      vacuumMaterial,       // material
                                                      name + "_vacuum_lv"); // name

      vacuumLV->SetVisAttributes(containerVisAttr);
      // user limits - provided by BDSAcceleratorComponent
      vacuumLV->SetUserLimits(userLimits);
      SetAcceleratorVacuumLogicalVolume(vacuumLV);
      RegisterLogicalVolume(vacuumLV);
      if (sensitiveVacuum)
	{RegisterSensitiveVolume(vacuumLV, BDSSDType::energydepvacuum);}

      G4PVPlacement* vacPV = new G4PVPlacement(colRotate,               // rotation
                                               (G4ThreeVector) 0,       // position
                                               vacuumLV,                // its logical volume
                                               name + "_vacuum_pv",     // its name
                                               containerLogicalVolume,  // its mother  volume
                                               false,                   // no boolean operation
                                               0,                       // copy number
                                               checkOverlaps);

      RegisterPhysicalVolume(vacPV);
    }
}

G4UserLimits* BDSTipCollimator::CollimatorUserLimits()
{
  if (BDS::IsFinite(minKineticEnergy))
    {
      // copy default ones with correct length and global time etc provided
      // by BDSAcceleratorComponent
      G4UserLimits* collUserLimits = new G4UserLimits(*userLimits);
      collUserLimits->SetUserMinEkine(minKineticEnergy);
      RegisterUserLimits(collUserLimits);
      return collUserLimits;
    }
  else // user limits - provided by BDSAcceleratorComponent
    {return userLimits;}
}