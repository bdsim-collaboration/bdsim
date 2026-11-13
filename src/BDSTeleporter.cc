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
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSamplerPlane.hh"
#include "BDSTeleporter.hh"
#include "BDSUtilities.hh"

#include "globals.hh" //G4 global constants & types
#include "G4Box.hh" 
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4UserLimits.hh"

#include <cmath>
#include <string>


BDSTeleporter::BDSTeleporter(const G4double length,
			     const G4double horizontalWidthIn,
			     BDSFieldInfo*  vacuumFieldInfoIn):
  BDSAcceleratorComponent("teleporter", length, 0, "teleporter"),
  horizontalWidth(horizontalWidthIn),
  vacuumFieldInfo(vacuumFieldInfoIn)
{;}

void BDSTeleporter::Build()
{
  // We don't use BDSAcceleratorComponent::Build() so we can control the user limits
  // to limit the step length in the volume.
  BuildContainerLogicalVolume();
  // set user limits for container & visual attributes
  if(containerLogicalVolume)
    {
      // user limits
      auto defaultUL = BDSGlobalConstants::Instance()->DefaultUserLimits();
      //copy the default and update with the length of the object rather than the default 1m
      G4UserLimits* ul = new G4UserLimits(*defaultUL);
      ul->SetMaxAllowedStep(1.1*chordLength);
      RegisterUserLimits(ul);

      containerLogicalVolume->SetUserLimits(ul);
      containerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->ContainerVisAttr());
    }
  BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumFieldInfo,
							    containerLogicalVolume,
							    true);
}

void BDSTeleporter::BuildContainerLogicalVolume()
{
  containerSolid = new G4Box(name+"_container_solid",
			     horizontalWidth * 0.5,
			     horizontalWidth * 0.5,
			     chordLength*0.5);
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");

  // register extents with BDSGeometryComponent base class
  SetExtent(BDSExtent(0.5*horizontalWidth, 0.5*horizontalWidth, chordLength*0.5));
}

G4Transform3D BDS::CalculateTeleporterDelta(const BDSBeamline* beamline,
					    G4double& teleporterLength)
{
  if (beamline->empty())
    {// can't do anything for an empty beam line
      teleporterLength = 0;
      return G4Transform3D();
    }
  
  // get position of last item in beamline
  // and then calculate necessary offset teleporter should apply
  // remember beam line could have finite offset and rotation to start with
  auto firstElement = beamline->front();
  auto lastElement  = beamline->back();
  G4ThreeVector lastItemPosition  = lastElement->GetReferencePositionEnd();
  G4ThreeVector firstItemPosition = firstElement->GetReferencePositionStart();
  G4ThreeVector positionDelta     = firstItemPosition - lastItemPosition;
  
  // we must subtract off the required padding length from the teleporter
  // delta 3-vector (note, along the axis of the beam line at the end)
  const G4double sL = BDSSamplerPlane::ChordLength();
  const G4double pL = beamline->PaddingLength();

  // unit z direction at end of current beam line
  G4ThreeVector lastItemUnitDir = G4ThreeVector(0,0,1).transform(*(lastElement->GetReferenceRotationEnd()));

  // project the length of the position delta onto the beam line direction.
  G4double rawLength = positionDelta.dot(lastItemUnitDir);

  // minimum space for the circular mechanics are:
  // 1x terminator with sampler chord length
  // 1x teleporter with (minimum) 1x sampler chord length
  // 3x padding length
  G4double minimumRequiredSpace = 2*sL + 3*pL;

  if (rawLength > 1*CLHEP::m)
    {
      std::string msg = "\nError - the calculated teleporter delta is above 1m!\nThe teleporter was only intended for small shifts";
      throw BDSException(__METHOD_NAME__, msg);
    }
  else if (rawLength < minimumRequiredSpace)
    {// should protect against -ve length teleporter
      std::string msg = "\nInsufficient space between the first and last elements\n";
      msg += "in the beam line to fit the terminator and teleporter - these will not be built.\n";
      msg += "Minimum space for circular mechanics is ";
      msg += std::to_string( minimumRequiredSpace/CLHEP::um) + " um";
      throw BDSException(__METHOD_NAME__, msg);
    }

  // update input reference variable (ie 2nd output variable)
  teleporterLength = rawLength - (sL + 3*pL);
  positionDelta -= lastItemUnitDir * (sL + 3*pL); // subtraction only in local z
  
  auto firstItemRotation = firstElement->GetReferenceRotationStart();
  auto lastItemRotation  = lastElement->GetReferenceRotationEnd();
  auto rotation = (*firstItemRotation) * (*lastItemRotation);

  G4Transform3D result = G4Transform3D(rotation, positionDelta);
  
  G4cout << "Calculating Teleporter delta" << G4endl;
  G4cout << "Last item end position:             " << lastItemPosition  << " mm" << G4endl;
  G4cout << "First item start position:          " << firstItemPosition << " mm" << G4endl;
  G4cout << "Teleporter delta (pos):             " << positionDelta     << " mm" << G4endl;
  G4cout << "Rotation: " << rotation << G4endl;
  return result;
}

BDSTeleporter::~BDSTeleporter()
{
  delete vacuumFieldInfo;
}
