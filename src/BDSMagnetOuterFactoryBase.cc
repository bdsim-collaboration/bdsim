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
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSExtent.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnetOuterFactoryBase.hh"
#include "BDSMaterials.hh"

#include "globals.hh" // geant4 types / globals

#include "G4Colour.hh"
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"

#include <set>

BDSMagnetOuterFactoryBase::BDSMagnetOuterFactoryBase()
{
  sensitiveOuter = BDSGlobalConstants::Instance()->SensitiveOuter();

  // initialise variables and pointers that'll be used by the factory
  CleanUpBase();
}

void BDSMagnetOuterFactoryBase::CleanUp()
{
  CleanUpBase();
}

void BDSMagnetOuterFactoryBase::CleanUpBase()
{
  FactoryBaseCleanUp();
  poleSolid            = nullptr;
  yokeSolid            = nullptr;
  containerSolid       = nullptr;
  magnetContainerSolid = nullptr;
  poleLV               = nullptr;
  yokeLV               = nullptr;
  containerLV          = nullptr;
  magnetContainerLV    = nullptr;
  yokePV               = nullptr;
  outerVisAttributes   = nullptr;

  magnetContainer = nullptr;

  inputFaceNormal  = G4ThreeVector(0,0,-1);
  outputFaceNormal = G4ThreeVector(0,0, 1);
}

void BDSMagnetOuterFactoryBase::CreateLogicalVolumes(const G4String& name,
						     G4Colour*       colour,
						     G4Material*     outerMaterial)
{
  if (poleSolid)
    {
      poleLV = new G4LogicalVolume(poleSolid,
				   outerMaterial,
				   name + "_pole_lv");
    }
  
  yokeLV = new G4LogicalVolume(yokeSolid,
			       outerMaterial,
			       name + "_yoke_lv");

  // the container is filled with the world material as it may not tightly fit the yoke - e.g. square container
  // for a C-shaped yoke -> there's a gap that should be filled with (e.g.) air
  G4Material* worldMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->WorldMaterial());
  containerLV = new G4LogicalVolume(containerSolid,
				    worldMaterial,
				    name + "_outer_container_lv");

  magnetContainerLV = new G4LogicalVolume(magnetContainerSolid,
					  worldMaterial,
					  name + "_container_lv");
  
  // VISUAL ATTRIBUTES
  G4VisAttributes* outerVisAttr;
  if (colour)
    {outerVisAttr = new G4VisAttributes(*colour);}
  else
    {outerVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("default"));}
  outerVisAttr->SetVisibility(true);
  outerVisAttr->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  allVisAttributes.insert(outerVisAttr);
  if (poleLV)
    {poleLV->SetVisAttributes(outerVisAttr);}
  yokeLV->SetVisAttributes(outerVisAttr);
  // container
  containerLV->SetVisAttributes(containerVisAttr);
  magnetContainerLV->SetVisAttributes(containerVisAttr);
}

void BDSMagnetOuterFactoryBase::SetUserLimits()
{
  if (poleLV)
    {poleLV->SetUserLimits(defaultUserLimits);}
  yokeLV->SetUserLimits(defaultUserLimits);
  containerLV->SetUserLimits(defaultUserLimits);
  magnetContainerLV->SetUserLimits(defaultUserLimits);

  for (auto& lv : allLogicalVolumes)
    {lv->SetUserLimits(defaultUserLimits);}
}

void BDSMagnetOuterFactoryBase::BuildMagnetContainerSolidAngled(const G4String& name,
								G4double        magnetContainerLength,
								G4double        magnetContainerRadius,
								G4bool          flatFaces)
{
  magnetContainerRadius += lengthSafetyLarge; // extra margin
  magnetContainerSolid = BDS::AppropriateTubs(name + "_container_solid",   // name
				       0,                           // inner radius
				       magnetContainerRadius,       // outer radius
				       magnetContainerLength * 0.5, // z half length
				       0,                           // starting angle
				       CLHEP::twopi,                // sweep angle
				       inputFaceNormal,             // input face normal vector
				       outputFaceNormal,            // output face normal vector
				       flatFaces);

  magContExtent = BDSExtent(magnetContainerRadius, magnetContainerRadius, magnetContainerLength*0.5);
}

void BDSMagnetOuterFactoryBase::BuildMagnetContainerSolidStraight(const G4String& name,
								  G4double        magnetContainerLength,
								  G4double        magnetContainerRadius)
{
  magnetContainerRadius += lengthSafetyLarge; // extra margin
  magnetContainerSolid = new G4Tubs(name + "_container_solid",   // name
				    0,                           // inner radius
				    magnetContainerRadius,       // outer radius
				    magnetContainerLength * 0.5, // z half length
				    0,                           // starting angle
				    CLHEP::twopi);               // sweep angle

  magContExtent = BDSExtent(magnetContainerRadius, magnetContainerRadius, magnetContainerLength*0.5);
}
  
void BDSMagnetOuterFactoryBase::CreateMagnetContainerComponent()
{
  magnetContainer = new BDSGeometryComponent(magnetContainerSolid,
					     magnetContainerLV,
					     magContExtent);
}


void BDSMagnetOuterFactoryBase::SetFaceNormals(BDSMagnetOuter* outer)
{
  outer->SetInputFaceNormal(inputFaceNormal);
  outer->SetOutputFaceNormal(outputFaceNormal);
}
  
