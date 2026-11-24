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
#include "BDSBeamPipeFactoryCircularVacuum.hh"
#include "BDSBeamPipe.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 globals / types
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VSolid.hh"

#include <cmath>
#include <set>
#include <utility>

class G4Material;

BDSBeamPipeFactoryCircularVacuum::BDSBeamPipeFactoryCircularVacuum()
{;}

BDSBeamPipe* BDSBeamPipeFactoryCircularVacuum::CreateBeamPipe(const G4String& nameIn,
							      G4double    lengthIn,
							      G4double    aper1In,
							      G4double    /*aper2In*/,
							      G4double    /*aper3In*/,
							      G4double    /*aper4In*/,
							      G4Material* vacuumMaterialIn,
							      G4double    /*beamPipeThicknessIn*/,
							      G4Material* /*beamPipeMaterialIn*/,
							      const G4String& /*pointsFileIn*/,
							      const G4String& /*pointsUnitIn*/)
{
  // clean up after last usage
  CleanUp();
  
  containerSolid = new G4Tubs(nameIn + "_container_solid",  // name
			      0,                            // inner radius
			      aper1In,                      // outer radius
			      lengthIn*0.5,                 // half length
			      0,                            // rotation start angle
			      CLHEP::twopi);                // rotation finish angle

  G4double containerRadius = aper1In + lengthSafetyLarge;
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, lengthIn, containerRadius);
}

BDSBeamPipe* BDSBeamPipeFactoryCircularVacuum::CreateBeamPipe(const G4String&      nameIn,
							      G4double             lengthIn,
							      const G4ThreeVector& inputFaceNormalIn,
							      const G4ThreeVector& outputFaceNormalIn,
							      G4double      aper1In,
							      G4double      /*aper2In*/,
							      G4double      /*aper3In*/,
							      G4double      /*aper4In */,
							      G4Material*   vacuumMaterialIn,
							      G4double      /*beamPipeThicknessIn*/,
							      G4Material*   /*beamPipeMaterialIn*/,
							      const G4String& /*pointsFileIn*/,
							      const G4String& /*pointsUnitIn*/)
{
  // clean up after last usage
  CleanUp();
  inputFaceNormal  = inputFaceNormalIn;
  outputFaceNormal = outputFaceNormalIn;

  // check faces of angled container volume don't intersect - if it can be built, remaining angled volumes can be built
  CheckAngledVolumeCanBeBuilt(lengthIn, inputFaceNormal, outputFaceNormal, aper1In, nameIn);

  containerSolid = new G4CutTubs(nameIn + "_container_solid",  // name
				 0,                            // inner radius
				 aper1In,                      // outer radius
				 lengthIn*0.5,                 // half length
				 0,                            // rotation start angle
				 CLHEP::twopi,                 // rotation finish angle
				 inputFaceNormal,              // input face normal
				 outputFaceNormal);            // rotation finish angle

  G4double containerRadius = aper1In + lengthSafetyLarge;
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, lengthIn, containerRadius);
}

/// only the solids are unique, once we have those, the logical volumes and placement in the
/// container are the same.  group all this functionality together
BDSBeamPipe* BDSBeamPipeFactoryCircularVacuum::CommonFinalConstruction(const G4String& nameIn,
								       G4Material* vacuumMaterialIn,
								       G4double    lengthIn,
								       G4double    containerRadiusIn)
{
  // prepare a longer container subtraction solid
  // doesn't have to be angled as it's only used for transverse subtraction
  containerSubtractionSolid = new G4Tubs(nameIn + "_container_sub_solid",// name
					 0,                              // inner radius
					 containerRadiusIn,              // outer radius
					 lengthIn*4,                     // full length for unambiguous subtraction
					 0,                              // rotation start angle
					 CLHEP::twopi);                  // rotation finish angle

  // We don't use BDSBeamPipeFactoryBase::CommonConstruction as this factory doesn't
  // produce a beam pipe solid etc - only vacuum so it wouldn't work.
  
  containerLV = new G4LogicalVolume(containerSolid,
				    vacuumMaterialIn,
				    nameIn + "_container_lv");

  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->ContainerVisAttr());
  vacuumLV = containerLV; // copy pointer for referencing in BuildBeamPipeAndRegisterVolumes.

  // user limits
  auto defaultUL = BDSGlobalConstants::Instance()->DefaultUserLimits();
  //copy the default and update with the length of the object rather than the default 1m
  G4UserLimits* ul = BDS::CreateUserLimits(defaultUL, lengthIn);
  if (ul != defaultUL) // if it's not the default register it
    {allUserLimits.insert(ul);}
  containerLV->SetUserLimits(ul);
	vacuumLV->SetUserLimits(ul);

  // record extents
  BDSExtent ext = BDSExtent(containerRadiusIn, containerRadiusIn, lengthIn*0.5);
  
  return BDSBeamPipeFactoryBase::BuildBeamPipeAndRegisterVolumes(ext, containerRadiusIn);
}
