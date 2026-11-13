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
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSLWCalorimeter.hh"
#include "BDSSDManager.hh"
#include "BDSMaterials.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4UserLimits.hh"


BDSLWCalorimeter::BDSLWCalorimeter(G4String         name,
				   G4double         length,
				   BDSBeamPipeInfo* beamPipeInfo,
				   G4int            precisionRegion):
  BDSAcceleratorComponent(name, length, 0, "lwcalorimeter", precisionRegion, beamPipeInfo),
  lwCalLogicalVolume(nullptr),lwCal(nullptr),physiLWCal(nullptr),beampipe(nullptr)
{;}

void BDSLWCalorimeter::Build()
{
  BDSAcceleratorComponent::Build();
  BuildCal(chordLength);
  BuildBeampipe();
}

void BDSLWCalorimeter::BuildMarkerLogicalVolume()
{
  G4double SampTransSize;
  SampTransSize = BDSGlobalConstants::Instance()->SamplerDiameter() * 0.5 * CLHEP::mm;

  containerSolid = new G4Box(name + "_container_solid", // name
			     SampTransSize,
			     SampTransSize,
			     chordLength*0.5);
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
  
#ifndef NOUSERLIMITS
  G4UserLimits* outerUserLimits =new G4UserLimits();
  outerUserLimits->SetMaxAllowedStep(chordLength);
  outerUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->MaxTime());
  RegisterUserLimits(outerUserLimits);
  containerLogicalVolume->SetUserLimits(outerUserLimits);
#endif
}

void BDSLWCalorimeter::BuildCal(G4double aLength)
{
  // build the Calorimeter
  lwCal=new G4Box(name + "_lw_cal_solid",
                  BDSGlobalConstants::Instance()->LWCalWidth()/2,
                  BDSGlobalConstants::Instance()->LWCalWidth()/2,
		  aLength/2);
  RegisterSolid(lwCal);
  lwCalLogicalVolume=new G4LogicalVolume(lwCal,
					 BDSMaterials::Instance()->GetMaterial("LeadTungstate"),
					 name + "_lw_cal_lv");
  RegisterLogicalVolume(lwCalLogicalVolume);
  physiLWCal = new G4PVPlacement(nullptr,                       // rotation
				 G4ThreeVector(BDSGlobalConstants::Instance()->LWCalOffset(),0.,0.),
				 lwCalLogicalVolume,   // its logical volume
				 name +"_lw_cal_pv",	     // its name
				 containerLogicalVolume,  // its mother  volume
				 false,		     // no boolean operation
				 0,                       // copy number
				 checkOverlaps);
  RegisterPhysicalVolume(physiLWCal);
  
  lwCalLogicalVolume->SetSensitiveDetector(BDSSDManager::Instance()->GetLWCalorimeterSD());    
}

void BDSLWCalorimeter::BuildBeampipe()
{
  beampipe = BDSBeamPipeFactory::Instance()->CreateBeamPipe(name,
							    chordLength,
							    beamPipeInfo);

  G4PVPlacement* beampipePV = new G4PVPlacement(nullptr,                                 // rotation
						(G4ThreeVector)0,                  // position
						beampipe->GetContainerLogicalVolume(), // its logical volume
						name +"_beampipe_pv",              // its name
						containerLogicalVolume,            // its mother  volume
						false,		                   // no boolean operation
						0,
						checkOverlaps);                    // copy number

  RegisterPhysicalVolume(beampipePV);

  InheritExtents(beampipe);
}

std::vector<G4LogicalVolume*> BDSLWCalorimeter::GetAllSensitiveVolumes() const
{
  if (!beampipe)
    {return BDSGeometryComponent::GetAllSensitiveVolumes();}
  else
    {
      std::vector<G4LogicalVolume*> result;
      for (auto i : beampipe->GetAllSensitiveVolumes())
	{result.push_back(i);}
      for (auto i : BDSGeometryComponent::GetAllSensitiveVolumes())
	{result.push_back(i);}
      return result;
    }
}


BDSLWCalorimeter::~BDSLWCalorimeter()
{
  delete beampipe;
}
