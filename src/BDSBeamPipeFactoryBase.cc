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
#include "BDSBeamPipeFactoryBase.hh"
#include "BDSColours.hh"
#include "BDSColourFromMaterial.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSSDType.hh"
#include "BDSUtilities.hh"

#include "globals.hh"                 // geant4 globals / types
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

#include <limits>
#include <set>

BDSBeamPipeFactoryBase::BDSBeamPipeFactoryBase()
{
  BDSGlobalConstants* g = BDSGlobalConstants::Instance();
  sensitiveBeamPipe     = g->SensitiveBeamPipe();
  sensitiveVacuum       = g->StoreELossVacuum();
  storeApertureImpacts  = g->StoreApertureImpacts();
  CleanUpBase(); // non-virtual call in constructor
}

void BDSBeamPipeFactoryBase::CleanUp()
{
  CleanUpBase();
}

void BDSBeamPipeFactoryBase::CleanUpBase()
{
  FactoryBaseCleanUp();
  vacuumSolid               = nullptr;
  beamPipeSolid             = nullptr;
  containerSolid            = nullptr;
  containerSubtractionSolid = nullptr;
  vacuumLV                  = nullptr;
  beamPipeLV                = nullptr;
  containerLV               = nullptr;
  vacuumPV                  = nullptr;
  beamPipePV                = nullptr;

  inputFaceNormal  = G4ThreeVector(0,0,-1);
  outputFaceNormal = G4ThreeVector(0,0, 1);
}
  
void BDSBeamPipeFactoryBase::CommonConstruction(const G4String& nameIn,
                                                G4Material* vacuumMaterialIn,
                                                G4Material* beamPipeMaterialIn,
                                                G4double    length)
{
  allSolids.insert(vacuumSolid);
  allSolids.insert(beamPipeSolid);
  /// build logical volumes
  BuildLogicalVolumes(nameIn, vacuumMaterialIn, beamPipeMaterialIn);
  /// set visual attributes
  SetVisAttributes(beamPipeMaterialIn, vacuumMaterialIn);
  /// set user limits
  SetUserLimits(length);
  /// place volumes
  PlaceComponents(nameIn);
}

void BDSBeamPipeFactoryBase::BuildLogicalVolumes(const G4String& nameIn,
                                                 G4Material* vacuumMaterialIn,
                                                 G4Material* beamPipeMaterialIn)
{
  // build the logical volumes
  vacuumLV   = new G4LogicalVolume(vacuumSolid,
                                   vacuumMaterialIn,
                                   nameIn + "_vacuum_lv");
  
  beamPipeLV = new G4LogicalVolume(beamPipeSolid,
                                   beamPipeMaterialIn,
                                   nameIn + "_beampipe_lv");

  G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->EmptyMaterial());
  containerLV = new G4LogicalVolume(containerSolid,
                                    emptyMaterial,
                                    nameIn + "_container_lv");
  allLogicalVolumes.insert(vacuumLV);
  allLogicalVolumes.insert(beamPipeLV);
}

void BDSBeamPipeFactoryBase::SetVisAttributes(G4Material* beamPipeMaterialIn,
                                              G4Material* vacuumMaterialIn)
{
  auto cfm = BDSColourFromMaterial::Instance();
  G4Colour* c = cfm->GetColourWithDefault(beamPipeMaterialIn, BDSColours::Instance()->GetColour("beampipe"));
  G4VisAttributes* pipeVisAttr = new G4VisAttributes(*c);
  pipeVisAttr->SetVisibility(true);
  pipeVisAttr->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  allVisAttributes.insert(pipeVisAttr);
  beamPipeLV->SetVisAttributes(pipeVisAttr);

  if (vacuumMaterialIn->GetDensity() > (1e-3*CLHEP::gram/CLHEP::cm3))
    {
      G4Colour* cv = cfm->GetColour(vacuumMaterialIn);
      G4VisAttributes* vacVisAttr = new G4VisAttributes(*cv);
      vacVisAttr->SetVisibility(true);
      vacVisAttr->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
      allVisAttributes.insert(vacVisAttr);
      vacuumLV->SetVisAttributes(vacVisAttr);
    }
  else
    {vacuumLV->SetVisAttributes(BDSGlobalConstants::Instance()->ContainerVisAttr());}

  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->ContainerVisAttr());
}

void BDSBeamPipeFactoryBase::SetUserLimits(G4double length)
{
  auto defaultUL = BDSGlobalConstants::Instance()->DefaultUserLimits();
  //copy the default and update with the length of the object rather than the default 1m
  G4UserLimits* ul = BDS::CreateUserLimits(defaultUL, length);
  if (ul != defaultUL) // if it's not the default register it
    {allUserLimits.insert(ul);}
  vacuumLV->SetUserLimits(ul);
  containerLV->SetUserLimits(ul);
  
  G4UserLimits* beamPipeUL = ul;
  if (BDSGlobalConstants::Instance()->BeamPipeIsInfiniteAbsorber())
    {// new beam pipe user limits, copy from updated default user limits above
      beamPipeUL = new G4UserLimits(*ul);
      beamPipeUL->SetUserMinEkine(std::numeric_limits<double>::max());
      allUserLimits.insert(beamPipeUL);
    }
  beamPipeLV->SetUserLimits(beamPipeUL);
}

void BDSBeamPipeFactoryBase::PlaceComponents(const G4String& nameIn)
{
  // PLACEMENT
  // place the components inside the container
  // note we don't need the pointer for anything - it's registered upon construction with g4  
  vacuumPV = new G4PVPlacement(nullptr,                  // no rotation
                               G4ThreeVector(),          // position
                               vacuumLV,                 // lv to be placed
                               nameIn + "_vacuum_pv",    // name
                               containerLV,              // mother lv to be placed in
                               false,                    // no boolean operation
                               0,                        // copy number
                               checkOverlaps);           // whether to check overlaps
  
  beamPipePV = new G4PVPlacement(nullptr,                      // no rotation
                                 G4ThreeVector(),              // position
                                 beamPipeLV,                   // lv to be placed
                                 nameIn + "_beampipe_pipe_pv", // name
                                 containerLV,                  // mother lv to be placed in
                                 false,                        // no boolean operation
                                 0,                            // copy number
                                 checkOverlaps);               // whether to check overlaps
  allPhysicalVolumes.insert(vacuumPV);
  allPhysicalVolumes.insert(beamPipePV);
}

BDSBeamPipe* BDSBeamPipeFactoryBase::BuildBeamPipeAndRegisterVolumes(BDSExtent extent,
                                                                     G4double  containerRadius,
                                                                     G4bool    containerIsCircular)
{  
  // build the BDSBeamPipe instance and return it
  BDSBeamPipe* aPipe = new BDSBeamPipe(containerSolid,containerLV,extent,
                                       containerSubtractionSolid,
                                       vacuumLV,containerIsCircular,containerRadius,
                                       inputFaceNormal, outputFaceNormal);

  // register objects
  aPipe->RegisterSolid(allSolids);
  aPipe->RegisterLogicalVolume(allLogicalVolumes); //using geometry component base class method
  aPipe->RegisterPhysicalVolume(allPhysicalVolumes);
  if (sensitiveVacuum)
    {aPipe->RegisterSensitiveVolume(vacuumLV, BDSSDType::energydepvacuum);}
  if (beamPipeLV)
    {
      if (sensitiveBeamPipe && storeApertureImpacts)// in the case of the circular vacuum, there isn't a beampipeLV
        {aPipe->RegisterSensitiveVolume(beamPipeLV, BDSSDType::aperturecomplete);}
      else if (storeApertureImpacts)
        {aPipe->RegisterSensitiveVolume(beamPipeLV, BDSSDType::apertureimpacts);}
      else
        {aPipe->RegisterSensitiveVolume(beamPipeLV, BDSSDType::energydep);}
    }
  aPipe->RegisterUserLimits(allUserLimits);
  aPipe->RegisterVisAttributes(allVisAttributes);
  
  return aPipe;
}

void BDSBeamPipeFactoryBase::CheckAngledVolumeCanBeBuilt(G4double length,
                                                         const G4ThreeVector &inputface,
                                                         const G4ThreeVector &outputface,
                                                         G4double beampipeRadius,
                                                         const G4String& name)
{
  G4bool intersects = BDS::WillIntersect(inputface.angle(), outputface.angle(), beampipeRadius*2, length);

  if (intersects)
    {throw BDSException(__METHOD_NAME__, "a volume in the \"" + name + "\" beam pipe geometry cannot be constructed as it's angled faces intersect within it's extent");}
}
