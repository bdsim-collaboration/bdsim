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
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSSDType.hh"
#include "BDSShield.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"

class G4Colour;
class G4Material;

BDSShield::BDSShield(const G4String&  nameIn,
                     G4double         lengthIn,
                     G4double         horizontalWidthIn,
                     G4double         xSizeIn,
                     G4double         ySizeIn,
                     G4Material*      materialIn,
                     G4Colour*        colourIn,
                     BDSBeamPipeInfo* beamPipeInfoIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "shield", beamPipeInfoIn),
  horizontalWidth(horizontalWidthIn),
  xSize(xSizeIn),
  ySize(ySizeIn),
  material(materialIn),
  colour(colourIn)
{;}

BDSShield::~BDSShield()
{;}

void BDSShield::Build()
{
  BDSAcceleratorComponent::Build();
  BuildShield();
  BuildBeamPipe();
}

void BDSShield::BuildContainerLogicalVolume()
{
  containerSolid = new G4Box(name + "_container_solid",
                             horizontalWidth*0.5,
                             horizontalWidth*0.5,
                             chordLength*0.5);
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
                                               worldMaterial,
                                               name + "_container_lv");
}

void BDSShield::BuildShield()
{
  G4VSolid* outerSolid = new G4Box(name + "_outer_solid",
                                   horizontalWidth*0.5 - lengthSafetyLarge,
                                   horizontalWidth*0.5 - lengthSafetyLarge,
                                   chordLength*0.5 - lengthSafety);
  RegisterSolid(outerSolid);

  G4VSolid* shieldSolid;
  G4LogicalVolume* shieldLV;

  // only subtract inner solid if shield aperture is finite.
  if (BDS::IsFinite(xSize) && BDS::IsFinite(ySize))
    {
      G4VSolid* innerSolid = new G4Box(name + "_inner_solid",
                                       xSize,
                                       ySize,
                                       chordLength); // extra long for unambiguous subtraction
      RegisterSolid(innerSolid);
      
      shieldSolid = new G4SubtractionSolid(name + "shield_solid",
                                           outerSolid,   // this
                                           innerSolid);  // minus this
      RegisterSolid(shieldSolid);
      
      shieldLV = new G4LogicalVolume(shieldSolid,
                                     material,
                                     name+"_shield_lv");
    }
  else
    {
      shieldLV = new G4LogicalVolume(outerSolid,
                                     material,
                                     name+"_shield_lv");
    }
  
  RegisterLogicalVolume(shieldLV);
  if (sensitiveOuter)
    {RegisterSensitiveVolume(shieldLV, BDSSDType::energydep);}

  G4VisAttributes* shieldVisAttr = new G4VisAttributes(*colour);
  shieldVisAttr->SetVisibility(true);
  shieldLV->SetVisAttributes(shieldVisAttr);
  RegisterVisAttributes(shieldVisAttr);

  G4PVPlacement* shieldPV = new G4PVPlacement(nullptr,
                                              G4ThreeVector(),
                                              shieldLV,
                                              name + "_shield_pv",
                                              containerLogicalVolume,
                                              false,
                                              0,
                                              checkOverlaps);

  RegisterPhysicalVolume(shieldPV);
}

void BDSShield::BuildBeamPipe()
{
  // check we should build a beam pipe
  if (!beamPipeInfo)
    {return;}
  
  // check beam pipe fits
  if ((xSize < (beamPipeInfo->aper1 + beamPipeInfo->beamPipeThickness)) || (ySize < (beamPipeInfo->aper2 + beamPipeInfo->beamPipeThickness)))
    {
      G4cout << __METHOD_NAME__ << "Shield will not fit around beam pipe - not building beam pipe!" << G4endl << G4endl;
      return;
    }
  
  // construct and place beam pipe
  auto bp = BDSBeamPipeFactory::Instance()->CreateBeamPipe(name,
                                                           chordLength - lengthSafety,
                                                           beamPipeInfo);
  RegisterDaughter(bp);

  G4PVPlacement* bpPV = new G4PVPlacement(nullptr,
                                          G4ThreeVector(),
                                          bp->GetContainerLogicalVolume(),
                                          name+"_beampipe_pv",
                                          containerLogicalVolume,
                                          false,
                                          0,
                                          checkOverlaps);

  RegisterPhysicalVolume(bpPV);
}
