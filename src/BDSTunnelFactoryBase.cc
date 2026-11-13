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
#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSSDType.hh"
#include "BDSTunnelInfo.hh"
#include "BDSTunnelFactoryBase.hh"
#include "BDSTunnelSection.hh"
#include "BDSUtilities.hh"            // for calculateorientation

#include "globals.hh"                 // geant4 globals / types
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include "G4UserLimits.hh"

#include <set>

BDSTunnelFactoryBase::BDSTunnelFactoryBase():
  tunnelComponent(nullptr),
  tunnelSection(nullptr),
  containerSolid(nullptr),
  tunnelSolid(nullptr),
  soilSolid(nullptr),
  floorSolid(nullptr),
  intersectionSolid(nullptr),
  containerLV(nullptr),
  tunnelLV(nullptr),
  soilLV(nullptr),
  floorLV(nullptr),
  floorDisplacement(G4ThreeVector(0,0,0)),
  cumulativeAngle(0)
{
  defaultModel  = BDSGlobalConstants::Instance()->TunnelInfo();
}

BDSTunnelSection* BDSTunnelFactoryBase::CreateTunnelSectionAngledIn(G4String      name,
								    G4double      length,
								    G4double      angleIn,
								    G4double      tunnelThickness,
								    G4double      tunnelSoilThickness,
								    G4Material*   tunnelMaterial,
								    G4Material*   tunnelSoilMaterial,
								    G4bool        tunnelFloor,
								    G4double      tunnelFloorOffset,
								    G4double      tunnel1,
								    G4double      tunnel2,
								    G4bool        visible)
{
  return CreateTunnelSectionAngledInOut(name, length, angleIn, 0, tunnelThickness,
					tunnelSoilThickness, tunnelMaterial, tunnelSoilMaterial,
					tunnelFloor, tunnelFloorOffset, tunnel1, tunnel2, visible);
}

BDSTunnelSection* BDSTunnelFactoryBase::CreateTunnelSectionAngledOut(G4String      name,
								     G4double      length,
								     G4double      angleOut,
								     G4double      tunnelThickness,
								     G4double      tunnelSoilThickness,
								     G4Material*   tunnelMaterial,
								     G4Material*   tunnelSoilMaterial,
								     G4bool        tunnelFloor,
								     G4double      tunnelFloorOffset,
								     G4double      tunnel1,
								     G4double      tunnel2,
								     G4bool        visible)
{
  return CreateTunnelSectionAngledInOut(name, length, 0, angleOut, tunnelThickness,
					tunnelSoilThickness, tunnelMaterial, tunnelSoilMaterial,
					tunnelFloor, tunnelFloorOffset, tunnel1, tunnel2, visible);
}

BDSTunnelSection* BDSTunnelFactoryBase::CreateTunnelSectionAngledInOut(G4String      name,
								       G4double      length,
								       G4double      angleIn,
								       G4double      angleOut,
								       G4double      tunnelThickness,
								       G4double      tunnelSoilThickness,
								       G4Material*   tunnelMaterial,
								       G4Material*   tunnelSoilMaterial,
								       G4bool        tunnelFloor,
								       G4double      tunnelFloorOffset,
								       G4double      tunnel1,
								       G4double      tunnel2,
								       G4bool        visible)
{
  auto faces = BDS::CalculateFaces(angleIn,angleOut);
  return CreateTunnelSectionAngled(name, length, faces.first, faces.second, tunnelThickness,
				   tunnelSoilThickness, tunnelMaterial, tunnelSoilMaterial,
				   tunnelFloor, tunnelFloorOffset, tunnel1, tunnel2, visible);
}

void BDSTunnelFactoryBase::CommontTestInputParameters(G4double&    length,
						      G4double&    tunnelThickness,
						      G4double&    tunnelSoilThickness,
						      G4Material*& tunnelMaterial,
						      G4Material*& tunnelSoilMaterial)
{ 
  if (length < 4*lengthSafety)
    {throw BDSException(__METHOD_NAME__, "tunnel section too short - length < 4*length safety");}
  if (tunnelThickness < 2*lengthSafety)
    {throw BDSException(__METHOD_NAME__, "tunnel section thickness too thin - thickness < 2*length safety");}
  if (tunnelSoilThickness < 2*lengthSafety)
    {throw BDSException(__METHOD_NAME__, "tunnel section soil thickness too thin - thickness < 2*length safety");}
  if (!tunnelMaterial)
    {tunnelMaterial = defaultModel->material;}

  if (!tunnelSoilMaterial)
    {tunnelSoilMaterial = defaultModel->soilMaterial;}
}

void BDSTunnelFactoryBase::CommonConstruction(G4String    name,
					      G4Material* tunnelMaterial,
					      G4Material* tunnelSoilMaterial,
					      G4double    length,
					      G4double    containerXRadius,
					      G4double    containerYRadius,
					      G4bool      visible)

{
  BuildLogicalVolumes(name, tunnelMaterial, tunnelSoilMaterial);
  SetVisAttributes(visible);
  SetUserLimits();
  PlaceComponents(name);
  PrepareGeometryComponent(containerXRadius, containerYRadius, 0.5*length);
  SetSensitiveVolumes();
  PrepareTunnelSection(name, length);
}

void BDSTunnelFactoryBase::BuildLogicalVolumes(G4String   name,
					       G4Material* tunnelMaterial,
					       G4Material* tunnelSoilMaterial)
{  
  // build logical volumes
  G4Material* worldMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->WorldMaterial());
  containerLV = new G4LogicalVolume(containerSolid,
				    worldMaterial,
				    name + "_container_lv");
  
  tunnelLV    = new G4LogicalVolume(tunnelSolid,
				    tunnelMaterial,
				    name + "_tunnel_lv");

  if (soilSolid)
    {
      soilLV      = new G4LogicalVolume(soilSolid,
					tunnelSoilMaterial,
					name + "_soil_lv");
    }
  // remember the floor is optional
  if (floorSolid)
    {
      floorLV     = new G4LogicalVolume(floorSolid,
					tunnelMaterial,
					name + "_floor_lv");
    }
}

void BDSTunnelFactoryBase::SetVisAttributes(G4bool visible)
{ 
  // note these could actually be owned by the factory or kept in one place as they're
  // always the same - this would however mean that the objects would have a dependency
  // on the factory which shouldn't happen - so waste a little memory just now on visattrs
  
  // tunnel & floor - a nice grey
  G4VisAttributes* tunnelVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("tunnel"));
  if (visible)
    {tunnelVisAttr->SetVisibility(true);}
  tunnelVisAttr->SetForceLineSegmentsPerCircle(BDSGlobalConstants::Instance()->NSegmentsPerCircle()*2);
  tunnelLV->SetVisAttributes(tunnelVisAttr);
  allVisAttributes.insert(tunnelVisAttr);
  if (floorLV)
    {
      G4VisAttributes* floorVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("tunnelfloor"));
      if (visible)
	{floorVisAttr->SetVisibility(true);}
      floorVisAttr->SetForceLineSegmentsPerCircle(BDSGlobalConstants::Instance()->NSegmentsPerCircle()*2);
      floorLV->SetVisAttributes(floorVisAttr);
      allVisAttributes.insert(floorVisAttr);
    }
  // soil - brown
  if (soilLV)
    {
      G4VisAttributes* soilVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("soil"));
      if (visible)
	{soilVisAttr->SetVisibility(true);}
      soilVisAttr->SetForceLineSegmentsPerCircle(BDSGlobalConstants::Instance()->NSegmentsPerCircle());
      soilLV->SetVisAttributes(soilVisAttr);
      allVisAttributes.insert(soilVisAttr);
    }
  // container & read out
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->ContainerVisAttr());
}

void BDSTunnelFactoryBase::PrepareGeometryComponent(G4double containerXRadius,
						    G4double containerYRadius,
						    G4double containerZRadius)
{
  // prepare final object and register logical volumes
  tunnelComponent = new BDSGeometryComponent(containerSolid, containerLV);
  tunnelComponent->RegisterLogicalVolume(tunnelLV);
  tunnelComponent->RegisterLogicalVolume(soilLV);
  if (floorLV)
    {tunnelComponent->RegisterLogicalVolume(floorLV);}

  // register objects
  tunnelComponent->RegisterSolid(allSolids);
  tunnelComponent->RegisterVisAttributes(allVisAttributes);

  // record extents
  // use the read out geometry for the limits as it's the maximum of x and y
  // in radius. The read out world is a duplicate of the real mass world so
  // we fiddle the extents here as the extents of the read out don't exist and
  // this is relatively safe as nothing will be placed against the outside edge
  // of the soil.
  tunnelComponent->SetExtent(BDSExtent(containerXRadius, containerYRadius, containerZRadius));
}

void BDSTunnelFactoryBase::PrepareTunnelSection(G4String name,
						G4double chordLength)
{
  tunnelSection = new BDSTunnelSection(name,
				       chordLength,
				       cumulativeAngle,
				       tunnelComponent,
				       intersectionSolid);

  // nominally, we'd leave this to something outside the factory, but seeing as this is
  // the only place where these are constructed, it's easier to do here.
  BDSAcceleratorComponentRegistry::Instance()->RegisterTunnelComponent(tunnelSection);
}

void BDSTunnelFactoryBase::SetSensitiveVolumes()
{
  // Register components as sensitive with BDSGeometryComponent instance.
  // As they use a (tunnel specific) read out geometry, the require attaching
  // only once the beamline has been constructed - done in BDSDetectorConstruction.
  // World must be already constructed, which requires beam line to be already constructed.
  tunnelComponent->RegisterSensitiveVolume(tunnelLV, BDSSDType::energydeptunnel);
  if (soilLV)
    {tunnelComponent->RegisterSensitiveVolume(soilLV, BDSSDType::energydeptunnel);}
  if (floorLV)
    {tunnelComponent->RegisterSensitiveVolume(floorLV, BDSSDType::energydeptunnel);}
}

void BDSTunnelFactoryBase::SetUserLimits()
{
  auto tunnelUserLimits = BDSGlobalConstants::Instance()->DefaultUserLimitsTunnel();
  //attach cuts to volumes
  tunnelLV->SetUserLimits(tunnelUserLimits);
  if (soilLV)
    {soilLV->SetUserLimits(tunnelUserLimits);}
  if (floorLV)
    {floorLV->SetUserLimits(tunnelUserLimits);}
  containerLV->SetUserLimits(tunnelUserLimits);
}

void BDSTunnelFactoryBase::PlaceComponents(G4String name)
{
  // PLACEMENT
  // place the components inside the container
  // note we don't need the pointer for anything - it's registered upon construction with g4
  new G4PVPlacement(nullptr,                  // no rotation
		    G4ThreeVector(),          // position
		    tunnelLV,                 // lv to be placed
		    name + "_tunnel_pv",      // name
		    containerLV,              // mother lv to be placed in
		    false,                    // no boolean operation
		    0,                        // copy number
		    checkOverlaps);           // whether to check overlaps
  if (soilLV)
    {
      new G4PVPlacement(nullptr,              // no rotation
			G4ThreeVector(),      // position
			soilLV,               // lv to be placed
			name + "_soil_pv",    // name
			containerLV,          // mother lv to be placed in
			false,                // no boolean operation
			0,                    // copy number
		        checkOverlaps);       // whether to check overlaps
    }
  if (floorLV)
    {
      new G4PVPlacement(nullptr,              // no rotation
			floorDisplacement,    // position
			floorLV,              // lv to be placed
			name + "_floor_pv",   // name
			containerLV,          // mother lv to be placed in
			false,                // no boolean operation
			0,                    // copy number
		        checkOverlaps);       // whether to check overlaps
    }
}

void BDSTunnelFactoryBase::CleanUp()
{
  FactoryBaseCleanUp();
  tunnelSection     = nullptr;
  containerSolid    = nullptr;
  tunnelSolid       = nullptr;
  soilSolid         = nullptr;
  floorSolid        = nullptr;
  intersectionSolid = nullptr;
  containerLV       = nullptr;
  tunnelLV          = nullptr;
  soilLV            = nullptr;
  floorLV           = nullptr;
  floorDisplacement = G4ThreeVector(0,0,0);
  cumulativeAngle   = 0;
}
