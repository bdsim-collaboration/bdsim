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
#include "BDSDebug.hh"
#include "BDSDegrader.hh"
#include "BDSException.hh"
#include "BDSSDType.hh"

#include "G4Box.hh"
#include "G4ExtrudedSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4TwoVector.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

#include "globals.hh" // geant4 globals / types
#include <string>
#include <vector>

#include "CLHEP/Units/SystemOfUnits.h"

BDSDegrader::BDSDegrader(const G4String&   nameIn,
			 G4double   lengthIn,
			 G4double   horizontalWidthIn,
			 G4int      numberWedgesIn,
			 G4double   wedgeLengthIn,
			 G4double   degraderHeightIn,
			 G4double   degraderOffsetIn,
			 G4double   baseWidthIn,
			 G4Material* degraderMaterialIn,
			 G4Material* vacuumMaterialIn,
			 G4Colour*   colourIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "degrader"),
  horizontalWidth(horizontalWidthIn),
  numberWedges(numberWedgesIn),
  wedgeLength(wedgeLengthIn),
  degraderHeight(degraderHeightIn),
  degraderOffset(degraderOffsetIn),
  baseWidth(baseWidthIn),
  degraderMaterial(degraderMaterialIn),
  vacuumMaterial(vacuumMaterialIn),
  colour(colourIn)
{;}

BDSDegrader::~BDSDegrader()
{;}

void BDSDegrader::BuildContainerLogicalVolume()
{
  // Input Checks
  if (horizontalWidth <= 0)
    {throw BDSException(__METHOD_NAME__,"option \"horizontalWidth\" is not defined or must be greater than 0 for element \"" + name + "\"");}

  if (numberWedges < 1)
    {throw BDSException(__METHOD_NAME__, "option \"numberWedges\" is not defined or must be greater than 0 for element \"" + name + "\"");}
  
  if (wedgeLength <= 0)
    {throw BDSException(__METHOD_NAME__, "option \"wedgeLength\" is not defined or must be greater than 0 for element \"" + name + "\"");}
  
  if (degraderHeight <= 0)
    {throw BDSException(__METHOD_NAME__, "option \"degraderHeight\" is not defined or must be greater than 0 for element \"" + name + "\"");}
  
  if (degraderHeight > (0.5*horizontalWidth))
    {throw BDSException(__METHOD_NAME__, "option \"degraderHeight\" must be less than 0.5 times \"horizontalWidth\" for element \"" + name + "\"");}

  if ((degraderOffset + baseWidth) > 0.5*horizontalWidth)
    {throw BDSException(__METHOD_NAME__, "option \"degraderOffset\" must be less than (0.5 times \"horizontalWidth\", minus aper1) for element \"" + name + "\"");}

  // adjust wedge offset to account for added base width
  degraderOffset += baseWidth;

  // ensure the container is wide enough
  G4double minimumWidth = wedgeLength;
  G4double containerWidth = degraderOffset;
  if (degraderOffset < minimumWidth)
    {containerWidth = minimumWidth;}

  containerSolid = new G4Box(name + "_container_solid",
			     containerWidth + lengthSafety,
			     degraderHeight*0.5,
			     chordLength*0.5);

  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       vacuumMaterial,
					       name + "_container_lv");
}

void BDSDegrader::Build()
{
  BDSAcceleratorComponent::Build();

  // width of the wedge at its base
  G4double wedgeWidth = chordLength/numberWedges - lengthSafety;

  // additional width to make the additional base section triangular
  // reduce by 10nm to remove annoying angstrom sized overlaps with fewer lengthSafety's
  G4double addedBasewidth = baseWidth* wedgeWidth/(2.0*wedgeLength) - 10*lengthSafety;

  std::vector<G4TwoVector> fullWedgeVertices; // vertex co-ordinates
  std::vector<G4TwoVector> halfWedgeVertices; // vertex co-ordinates

  // vertices of full wedge plus base
  fullWedgeVertices.push_back(G4TwoVector(0.5*wedgeWidth + addedBasewidth, 0));
  fullWedgeVertices.push_back(G4TwoVector(0, wedgeLength+baseWidth));
  fullWedgeVertices.push_back(G4TwoVector(-0.5*wedgeWidth - addedBasewidth, 0));

  // vertices of half wedge plus base
  halfWedgeVertices.push_back(G4TwoVector(0, 0));
  halfWedgeVertices.push_back(G4TwoVector(0.5*wedgeWidth + addedBasewidth, 0));
  halfWedgeVertices.push_back(G4TwoVector(0, wedgeLength+baseWidth));

  // full wedge Solid and logical Volume
  G4ExtrudedSolid* fullWedge = new G4ExtrudedSolid(name + "_fullwedge_solid",
                                                   fullWedgeVertices,
	                                           degraderHeight*0.5 - lengthSafety,
	                                           G4TwoVector(),1, G4TwoVector(), 1);
  RegisterSolid(fullWedge);
  G4LogicalVolume* fullWedgeLV = new G4LogicalVolume(fullWedge,                // solid
	                                             degraderMaterial,         // material
	                                             name + "_fullwedge_lv");  // name
  RegisterLogicalVolume(fullWedgeLV);

  // half wedge Solid and logical Volume
  G4ExtrudedSolid* halfWedge = new G4ExtrudedSolid(name + "_halfwedge_solid",
                                                   halfWedgeVertices,
                                                   degraderHeight*0.5 - lengthSafety,
	                                           G4TwoVector(),1, G4TwoVector(), 1);
  RegisterSolid(halfWedge);
  G4LogicalVolume* halfWedgeLV = new G4LogicalVolume(halfWedge,                // solid
                                                     degraderMaterial,         // material
                                                     name + "_halfwedge_lv");  // name
  RegisterLogicalVolume(halfWedgeLV);

  // Wedge color
  G4VisAttributes* degraderVisAttr = new G4VisAttributes(colour);
  fullWedgeLV->SetVisAttributes(degraderVisAttr);
  halfWedgeLV->SetVisAttributes(degraderVisAttr);
  RegisterVisAttributes(degraderVisAttr);

  // Rotation  of wedges. Left taken to be +VE x direction, right is -VE x direction.
  G4RotationMatrix* rightRot = new G4RotationMatrix();
  rightRot->rotateY(-CLHEP::halfpi);
  rightRot->rotateX(-CLHEP::halfpi);
  RegisterRotationMatrix(rightRot);

  G4RotationMatrix* leftRot = new G4RotationMatrix();
  leftRot->rotateY(CLHEP::halfpi);
  leftRot->rotateX(-CLHEP::halfpi);
  RegisterRotationMatrix(leftRot);

  // seperate rotation for first half wedge in even number of wedges.
  G4RotationMatrix* altRightRot = new G4RotationMatrix();
  altRightRot->rotateY(CLHEP::halfpi);
  altRightRot->rotateX(CLHEP::halfpi);
  RegisterRotationMatrix(altRightRot);

  // loop over number of wedges and place
  for (G4int i = 0; i < (numberWedges + 1); i++)
    {
      G4String wedgeName = name + "_" + std::to_string(i)+"_pv";
      if (IsEven(numberWedges))
	{
	  if (i == 0)
	    {PlaceWedge(true, lengthSafety, wedgeName, halfWedgeLV, altRightRot);}
	  else if (i==numberWedges)
	    {PlaceWedge(true, arcLength-lengthSafety, wedgeName, halfWedgeLV, rightRot);}
	  else if(IsEven(i))
	    {PlaceWedge(true, i*wedgeWidth, wedgeName, fullWedgeLV, rightRot);}
	  else if (IsOdd(i))
	    {PlaceWedge(false, i*wedgeWidth, wedgeName, fullWedgeLV, leftRot);}
	}
      else if (IsOdd(numberWedges))
        {
	  if (i == 0)
	    {PlaceWedge(false, lengthSafety, wedgeName, halfWedgeLV, leftRot);}
	  else if (i == numberWedges)
	    {PlaceWedge(true, arcLength-lengthSafety, wedgeName, halfWedgeLV, rightRot);}
	  else if (IsEven(i))
	    {PlaceWedge(false, i*wedgeWidth, wedgeName, fullWedgeLV, leftRot);}
	  else if (IsOdd(i))
	    {PlaceWedge(true, i*wedgeWidth, wedgeName, fullWedgeLV, rightRot);}
	}
    }

  if (sensitiveOuter)
    {
      RegisterSensitiveVolume(fullWedgeLV, BDSSDType::energydep);
      RegisterSensitiveVolume(halfWedgeLV, BDSSDType::energydep);
    }
}

void BDSDegrader::PlaceWedge(G4bool            placeRight,
                             G4double          zOffset,
                             const G4String&   placementName,
                             G4LogicalVolume*  lv,
                             G4RotationMatrix* rotation)
{
  G4double xOffset = -degraderOffset;  // default is right placement
  if (!placeRight)
    {xOffset = degraderOffset;}

  G4VPhysicalVolume* wedgePV = new G4PVPlacement(rotation,
                                                 G4ThreeVector(xOffset, 0, -0.5*arcLength + zOffset),
	                                         lv,
	                                         placementName,
	                                         containerLogicalVolume,
	                                         false,
	                                         0,
	                                         checkOverlaps);
  RegisterPhysicalVolume(wedgePV);
}
