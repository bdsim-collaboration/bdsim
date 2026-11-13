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
#include "BDSCavity.hh"
#include "BDSCavityFactoryBase.hh"
#include "BDSCavityInfo.hh"
#include "BDSColours.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSSDType.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <map>
#include <set>

class G4UserLimits;

BDSCavityFactoryBase::BDSCavityFactoryBase()
{
  emptyMaterial      = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->EmptyMaterial());
  sensitiveBeamPipe  = BDSGlobalConstants::Instance()->SensitiveBeamPipe();
  sensitiveVacuum    = BDSGlobalConstants::Instance()->StoreELossVacuum();

  CleanUpBase(); // initialise variables
}

BDSCavity* BDSCavityFactoryBase::CreateCavity(const G4String&      name,
					      G4double             totalChordLength,
					      const BDSCavityInfo* info,
					      G4Material*          vacuumMaterial)
{
  CleanUp();
  G4double containerRadius = CreateSolids(name, totalChordLength, info);
  CreateLogicalVolumes(name, info, vacuumMaterial);
  SetUserLimits(totalChordLength);
  SetVisAttributes();
  PlaceComponents(name);

  BDSExtent ext = BDSExtent(containerRadius, containerRadius, totalChordLength*0.5);
  BDSCavity* result = BuildCavityAndRegisterObjects(ext);

  return result;
}

void BDSCavityFactoryBase::CleanUp()
{
  CleanUpBase();
}

void BDSCavityFactoryBase::CleanUpBase()
{
  FactoryBaseCleanUp();

  vacuumSolid    = nullptr;
  cavitySolid    = nullptr;
  containerSolid = nullptr;
  vacuumLV       = nullptr;
  cavityLV       = nullptr;
  containerLV    = nullptr;
}

void BDSCavityFactoryBase::CreateLogicalVolumes(const G4String&      name,
						const BDSCavityInfo* info,
						G4Material*          vacuumMaterial)
{
  cavityLV = new G4LogicalVolume(cavitySolid,          // solid
				 info->material,       // material
				 name + "_cavity_lv"); // name
  allLogicalVolumes.insert(cavityLV);
  sensitiveVolumes[cavityLV] = BDSSDType::energydep;
  
  vacuumLV = new G4LogicalVolume(vacuumSolid,           // solid
				 vacuumMaterial,        // material
				 name + "_vacuum_lv");  // name
  if (sensitiveVacuum)
    {sensitiveVolumes[vacuumLV] = BDSSDType::energydepvacuum;}
  allLogicalVolumes.insert(vacuumLV);

  containerLV = new G4LogicalVolume(containerSolid,
				    emptyMaterial,
				    name + "_container_lv");
}

void BDSCavityFactoryBase::SetUserLimits(G4double length)
{
  auto defaultUL = BDSGlobalConstants::Instance()->DefaultUserLimits();
  //copy the default and update with the length of the object rather than the default 1m
  G4UserLimits* ul = BDS::CreateUserLimits(defaultUL, length);

  if (ul != defaultUL) // if it's not the default register it
    {allUserLimits.insert(ul);}

  for (auto lv : allLogicalVolumes)
    {lv->SetUserLimits(ul);}
}

void BDSCavityFactoryBase::SetVisAttributes(const G4String& colourName)
{
  // visualisation attributes
  auto col = BDSColours::Instance()->GetColour(colourName);
  G4VisAttributes* cavityVis = new G4VisAttributes(*col);
  cavityVis->SetVisibility(true);
  cavityVis->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  cavityLV->SetVisAttributes(cavityVis);
  allVisAttributes.insert(cavityVis);
  
  // vacuum
  vacuumLV->SetVisAttributes(BDSGlobalConstants::Instance()->ContainerVisAttr());
  // container
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->ContainerVisAttr());
}

void BDSCavityFactoryBase::PlaceComponents(const G4String& name)
{
  G4PVPlacement* vacuumPV = new G4PVPlacement(nullptr,               // rotation
					      G4ThreeVector(),       // position
					      vacuumLV,              // logical Volume to be placed
					      name + "_vacuum_pv",   // placement name
					      containerLV,           // mother volume
					      false,                 // pMany unused
					      0,                     // copy number
					      checkOverlaps);        // check overlaps
  allPhysicalVolumes.insert(vacuumPV);
  
  G4PVPlacement* cavityPV = new G4PVPlacement(nullptr,               // rotation
					      G4ThreeVector(),       // position
					      cavityLV,              // logical Volume to be placed
					      name + "_cavity_pv",   // placement name
					      containerLV,           // mother volume
					      false,                 // pMany unused
					      0,                     // copy number
					      checkOverlaps);        // check overlaps
  allPhysicalVolumes.insert(cavityPV);
}

BDSCavity* BDSCavityFactoryBase::BuildCavityAndRegisterObjects(const BDSExtent& extent)
{
  BDSCavity* cavity = new BDSCavity(containerSolid, containerLV,
				    extent, vacuumLV);

  // register objects
  cavity->RegisterSolid(allSolids);
  cavity->RegisterLogicalVolume(allLogicalVolumes); //using geometry component base class method
  if (sensitiveBeamPipe)
    {cavity->RegisterSensitiveVolume(sensitiveVolumes);}
  cavity->RegisterPhysicalVolume(allPhysicalVolumes);
  cavity->RegisterRotationMatrix(allRotationMatrices);
  cavity->RegisterUserLimits(allUserLimits);
  cavity->RegisterVisAttributes(allVisAttributes);
  
  return cavity;
}
