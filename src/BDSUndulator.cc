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
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSSDType.hh"
#include "BDSUndulator.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"

#include <cmath>

BDSUndulator::BDSUndulator(const G4String&  nameIn,
			   G4double         lengthIn,
			   G4double         periodIn,
			   G4double         undulatorMagnetHeightIn,
			   G4double         horizontalWidthIn,
			   G4double         undulatorGapIn,
			   BDSBeamPipeInfo* beamPipeInfoIn,
			   BDSFieldInfo*    vacuumFieldInfoIn,
			   BDSFieldInfo*    outerFieldInfoIn,
			   const G4String&  materialIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "undulator", beamPipeInfoIn),
  vacuumFieldInfo(vacuumFieldInfoIn),
  outerFieldInfo(outerFieldInfoIn),
  undulatorPeriod(periodIn),
  horizontalWidth(horizontalWidthIn),
  undulatorMagnetHeight(undulatorMagnetHeightIn),
  undulatorGap(undulatorGapIn),
  numMagnets(0)
{
  if (materialIn.empty())
    {
      BDS::Warning(__METHOD_NAME__, "element \"" + name + "\" no material set for undulator magnet - using iron");
      material = "iron";
    }
  else
    {material = materialIn;}

  if (vacuumFieldInfo)
    {vacuumFieldInfo->SetBeamPipeRadius(beamPipeInfoIn->IndicativeRadius());}
}

BDSUndulator::~BDSUndulator()
{;}

void BDSUndulator::BuildContainerLogicalVolume()
{
  // input Checks
  BDSExtent bp = beamPipeInfo->Extent();
  if (!BDS::IsFinite(undulatorPeriod))
    {throw BDSException(__METHOD_NAME__, "undulator period is 0, period must be finite.");}
  
  // check if the undulator period is an integer factor of the element length
  if (BDS::IsFinite(std::fmod(chordLength, undulatorPeriod)))
    {throw BDSException(__METHOD_NAME__, "undulator length \"arcLength\" does not divide into an integer number of\n undulator periods (length \"undulatorPeriod\".");}
  
  // can now cast num magnets to integer as above check should catch if it isn't an integer.
  numMagnets = (G4int)(2*chordLength/undulatorPeriod);

  G4double beampipeThickness = BDSGlobalConstants::Instance()->DefaultBeamPipeModel()->beamPipeThickness;
  if (!BDS::IsFinite(undulatorGap))
    {
      G4cout << __METHOD_NAME__ << "\"undulatorGap\" = 0 -> using 2x beam pipe height." << G4endl;
      undulatorGap = 2*(bp.DY() +  2*beampipeThickness);
    }
  if (undulatorGap < (bp.DY() + 2*beampipeThickness + lengthSafetyLarge))
    {throw BDSException(__METHOD_NAME__, "\"undulatorGap\" for element: \"" + name + "\" smaller than beam pipe aperture.");}
  
  if (undulatorGap >= horizontalWidth)
    {throw BDSException(__METHOD_NAME__, "\"undulatorGap\" for element: \"" + name + "\" larger than horizontalWidth.");}
  
  if (!BDS::IsFinite(undulatorMagnetHeight))
    {
      // update single magnet box height in case of undulator gap change.
      undulatorMagnetHeight = 0.5 * (horizontalWidth - undulatorGap);
    }
  if (undulatorMagnetHeight > 0.5*horizontalWidth)
    {throw BDSException(__METHOD_NAME__, "\"undulatorMagnetHeight\" larger than 0.5*horizontalWidth in component " + name);}
  else if ((2*undulatorMagnetHeight + undulatorGap) > horizontalWidth)
    {
      throw BDSException(__METHOD_NAME__, "Total undulator height (2*undulatorMagnetHeight + undulatorGap) is larger than horizontalWidth in component " + name);
    }

  G4double halfWidth  = 0.5 * (horizontalWidth + lengthSafetyLarge);
  containerSolid = new G4Box(name + "_container_solid",
                             halfWidth,
                             halfWidth,
                             chordLength*0.5);

  containerLogicalVolume = new G4LogicalVolume(containerSolid,
                                               emptyMaterial,
                                               name + "_container_lv");

  BDSExtent ext = BDSExtent(2 * halfWidth, 2 * halfWidth, chordLength);
  SetExtent(ext);
}

void BDSUndulator::Build()
{
  BDSAcceleratorComponent::Build();

  BDSBeamPipeFactory* factory = BDSBeamPipeFactory::Instance();
  BDSBeamPipe* pipe = factory->CreateBeamPipe(name, chordLength, beamPipeInfo);
  RegisterDaughter(pipe);

  G4double singleMagnetLength = (undulatorPeriod * 0.5) - lengthSafetyLarge;

  // magnet geometry
  G4Box* magnet = new G4Box(name + "_single_magnet_solid",
                            0.5*horizontalWidth - 2*lengthSafetyLarge,
                            0.5*undulatorMagnetHeight - 2*lengthSafetyLarge,
                            0.5*singleMagnetLength);
  RegisterSolid(magnet);

  G4Material* materialBox  = BDSMaterials::Instance()->GetMaterial(material);

  G4LogicalVolume* lowerBoxLV = new G4LogicalVolume(magnet,
						    materialBox,
						    name + "_lower_box_lv");
  
  G4LogicalVolume* upperBoxLV = new G4LogicalVolume(magnet,
						    materialBox,
						    name + "_upper_box_lv");
  RegisterLogicalVolume(lowerBoxLV);
  RegisterLogicalVolume(upperBoxLV);
  if (sensitiveOuter)
    {
      RegisterSensitiveVolume(lowerBoxLV, BDSSDType::energydep);
      RegisterSensitiveVolume(upperBoxLV, BDSSDType::energydep);
    }

  // colour
  G4VisAttributes* lowerBoxcolour = new G4VisAttributes(*BDSColours::Instance()->GetColour("quadrupole"));
  lowerBoxLV->SetVisAttributes(lowerBoxcolour);
  RegisterVisAttributes(lowerBoxcolour);

  G4VisAttributes* upperBoxcolour = new G4VisAttributes(*BDSColours::Instance()->GetColour("sectorbend"));
  upperBoxLV->SetVisAttributes(upperBoxcolour);
  RegisterVisAttributes(upperBoxcolour);

  G4double verticalOffset = 0.5 * (undulatorGap + undulatorMagnetHeight);
  // place upper and lower magnets in a loop
  for (G4int i = 1; i <= numMagnets; i++)
    {
      G4bool sign = BDS::IsFinite(std::fmod(i, 2));
      G4LogicalVolume* uVol = sign ? upperBoxLV : lowerBoxLV;
      G4LogicalVolume* lVol = !sign ? upperBoxLV : lowerBoxLV;

      G4ThreeVector upperBoxPos(0, verticalOffset,  (0.5*chordLength - undulatorPeriod/4.0) -  ((i-1) *undulatorPeriod/2.0));
      G4ThreeVector lowerBoxPos(0, -verticalOffset, (0.5*chordLength - undulatorPeriod/4.0) -  ((i-1) *undulatorPeriod/2.0));
      
      G4PVPlacement* upperBoxPV = new G4PVPlacement(nullptr,                  // rotation
                                                    upperBoxPos,              // position
                                                    uVol,                     // logical volume
                                                    name + "_upper_pos_" + std::to_string(i) +  "_pv", // name
                                                    containerLogicalVolume,   // mother volume
                                                    false,                    // no boolean operation
                                                    i,                        // copy number
                                                    checkOverlaps);
      
      G4PVPlacement* lowerBoxPV= new G4PVPlacement(nullptr,
                                                   lowerBoxPos,
                                                   lVol,
                                                   name + "_lower_pos_" + std::to_string(i) +  "_pv",
                                                   containerLogicalVolume,
                                                   false,
                                                   i,
                                                   checkOverlaps);
      RegisterPhysicalVolume(upperBoxPV);
      RegisterPhysicalVolume(lowerBoxPV);
    }

  // place beam pipe volume
  SetAcceleratorVacuumLogicalVolume(pipe->GetVacuumLogicalVolume());
  SetInputFaceNormal(pipe->InputFaceNormal());
  SetOutputFaceNormal(pipe->OutputFaceNormal());
  G4PVPlacement* bpPV = new G4PVPlacement(nullptr,
                                          G4ThreeVector(),
                                          pipe->GetContainerLogicalVolume(),
                                          name+"_beampipe_pv",
                                          containerLogicalVolume,
                                          false,
                                          0,
                                          checkOverlaps);
  RegisterPhysicalVolume(bpPV);

  BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumFieldInfo,
                                                            pipe->GetVacuumLogicalVolume(),
                                                            true);

  if (outerFieldInfo)
    {
      // Attach to the container but don't propagate to daughter volumes. This ensures
      // any gap between the beam pipe and the outer also has a field.
      BDSFieldBuilder::Instance()->RegisterFieldForConstruction(outerFieldInfo,
                                                                containerLogicalVolume,
                                                                false,
                                                                vacuumFieldInfo->MagnetStrength(),
                                                                "field");
    }
}
