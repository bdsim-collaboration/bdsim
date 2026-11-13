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
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterFactoryPolesSquare.hh"
#include "BDSMaterials.hh"
#include "BDSSDType.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

#include <map>
#include <set>
#include <vector>

BDSMagnetOuterFactoryPolesSquare::BDSMagnetOuterFactoryPolesSquare():
  BDSMagnetOuterFactoryPolesBase(/*poleStopFactor=*/1.5)
{
  CleanUp();
}

void BDSMagnetOuterFactoryPolesSquare::CleanUp()
{
  CleanUpPolesSquare();
  BDSMagnetOuterFactoryPolesBase::CleanUp();
}

void BDSMagnetOuterFactoryPolesSquare::CleanUpPolesSquare()
{
  poleRotations.clear();
  poleSolids.clear();
  poleLVs.clear();
  order = 0;
}

void BDSMagnetOuterFactoryPolesSquare::CreateYokeAndContainerSolid(const G4String& name,
								   G4double        length,
								   G4int         /*order*/,
								   G4double        magnetContainerLength,
								   G4double        magnetContainerRadiusIn)
{
  // square yoke - have to do subtraction between two solid boxes
  G4VSolid* yokeOuter = new G4Box(name + "_yoke_outer_solid", // name
				  yokeFinishRadius,           // x half width
				  yokeFinishRadius,           // y half width
				  length*0.5 - lengthSafety); // z half length

  G4VSolid* yokeInner = new G4Box(name + "_yoke_inner_solid", // name
				  yokeStartRadius,            // x half width
				  yokeStartRadius,            // y half width
				  length);                    // z half length
  // inner length is 2x as long for unambiguous subtraction
  allSolids.insert(yokeOuter);
  allSolids.insert(yokeInner);

  yokeSolid = new G4SubtractionSolid(name + "_yoke_solid",    // name
				     yokeOuter,
				     yokeInner);

  // note container must have hole in it for the beampipe to fit in!
  // poled geometry doesn't fit tightly to beampipe so can alays use a circular aperture

  G4VSolid* containerOuter = new G4Box(name + "_container_outer_solid", // name
				       yokeFinishRadius,                // x half width
				       yokeFinishRadius,                // y half width
				       length*0.5);                     // x half width

  G4VSolid* containerInner = new G4Tubs(name + "_container_inner_solid", // name
					0,                               // inner radius
					poleStartRadius,                 // finish radius
					length,                          // z half length
					0,                               // start angle
					CLHEP::twopi);                   // sweep angle
  // length of inner is long for unambiguous subtraction
  allSolids.insert(containerOuter);
  allSolids.insert(containerInner);

  // pole intersection solid
  G4double  croppingBoxRadius = yokeStartRadius - lengthSafety;
  poleIntersectionSolid = new G4Box(name + "_pole_intersection_solid", // name
				    croppingBoxRadius,                 // x half width
				    croppingBoxRadius,                 // y half width
				    length);                           // z length
  allSolids.insert(poleIntersectionSolid);
  // z length long for unambiguous intersection
  
  containerSolid = new G4SubtractionSolid(name + "_outer_container_solid", // name
					  containerOuter,
					  containerInner);

  magnetContainerSolid = new G4Box(name + "_container_solid", // name
				   magnetContainerRadiusIn,     // x half length
				   magnetContainerRadiusIn,     // y half length
				   magnetContainerLength*0.5);// z half length

  magContExtent = BDSExtent(magnetContainerRadiusIn, magnetContainerRadiusIn, magnetContainerLength*0.5);
}

void BDSMagnetOuterFactoryPolesSquare::IntersectPoleWithYoke(const G4String& name,
                                                             G4double      /*length*/,
                                                             G4int           orderIn)
{
  order = orderIn; // copy to member variable - this is the first function to be called with order
  
  G4int nPoles = 2*orderIn;
  segmentAngle = CLHEP::twopi/nPoles; // angle per pole
  // create different poles to fit inside square yoke
  for (G4int i = 0; i < nPoles; ++i)
    {
      G4RotationMatrix* iPoleRM = new G4RotationMatrix();
      G4double rotationAngle = (0.5-i)*segmentAngle + CLHEP::pi*0.5;
      iPoleRM->rotateZ(rotationAngle);
      allRotationMatrices.insert(iPoleRM);
      // crop the singlepolesolid with the cropping box so it'll fit inside the outer square yoke
      G4IntersectionSolid* aSolid = new G4IntersectionSolid(name + "_pole_solid", // name
							    poleSolid,            // solid 1 - the pole
							    poleIntersectionSolid,// solid 2 - the one to be shifted
							    iPoleRM,              // rotation matrix
							    G4ThreeVector());     // translation vector
      
      poleSolids.push_back(aSolid);
    }
}

void BDSMagnetOuterFactoryPolesSquare::CreateLogicalVolumes(const G4String& name,
							    G4Colour*       colour,
							    G4Material*     outerMaterial)
{
  G4VisAttributes* outerVisAttr = new G4VisAttributes(*colour);
  outerVisAttr->SetVisibility(true);
  outerVisAttr->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  allVisAttributes.insert(outerVisAttr);

  for (G4int n = 0; n < 2*order; ++n)
    {
      G4LogicalVolume* thisPole = new G4LogicalVolume(poleSolids[n],
						      outerMaterial,
						      name + "_pole_lv");
      thisPole->SetVisAttributes(outerVisAttr);
      poleLVs.push_back(thisPole);
      allLogicalVolumes.insert(thisPole);
    }

  // yoke
  yokeLV = new G4LogicalVolume(yokeSolid,
			       outerMaterial,
			       name + "_yoke_lv");
  yokeLV->SetVisAttributes(outerVisAttr);

  // container
  G4Material* worldMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->WorldMaterial());
  containerLV = new G4LogicalVolume(containerSolid,
				    worldMaterial,
				    name + "_container_lv");
  containerLV->SetVisAttributes(containerVisAttr);

  magnetContainerLV = new G4LogicalVolume(magnetContainerSolid,
					  worldMaterial,
					  name + "_container_lv");
  magnetContainerLV->SetVisAttributes(containerVisAttr);

  // user limits
  yokeLV->SetUserLimits(defaultUserLimits);
  containerLV->SetUserLimits(defaultUserLimits);
  magnetContainerLV->SetUserLimits(defaultUserLimits);
  for (auto& lv : poleLVs)
    {lv->SetUserLimits(defaultUserLimits);}
  for (auto& lv : allLogicalVolumes)
    {lv->SetUserLimits(defaultUserLimits);}

  // create logical volumes for the coils using base class method
  CreateLogicalVolumesCoil(name);
}

void BDSMagnetOuterFactoryPolesSquare::PlaceComponents(const G4String& name,
						       G4int    orderIn)
{
  // PLACEMENT
  // place the components inside the container
  // note we don't need the pointer for placements - it's registered upon construction with g4
  yokePV = new G4PVPlacement(nullptr,             // no rotation
			     G4ThreeVector(),     // position
			     yokeLV,              // lv to be placed
			     name + "_yoke_pv",   // name
			     containerLV,         // mother lv to be placed in
			     false,               // no boolean operation
			     0,                   // copy number
			     checkOverlaps);      // whether to check overlaps
  
  // place poles
  if (!buildPole)
    {return;}
  G4double nPoles = 2*orderIn;
  segmentAngle = CLHEP::twopi/nPoles; // angle per pole
  G4PVPlacement* aPlacement = nullptr;
  for (G4int n = 0; n < 2*orderIn; ++n)
    {
      G4RotationMatrix* rm = new G4RotationMatrix();
      G4double rotationAngle = (0.5-n)*segmentAngle + CLHEP::pi*0.5;
      rm->rotateZ(-rotationAngle);
      allRotationMatrices.insert(rm);
      G4String pvName = name + "_pole_" + std::to_string(n) + "_pv";
      // only need to test the end of one iterator as both should be the same length
      aPlacement = new G4PVPlacement(rm,                 // rotation
				     G4ThreeVector(),    // position
				     poleLVs[n],         // logical volume
				     pvName,             // name      
				     containerLV,        // mother lv to be placed in
				     false,              // no boolean operation
				     n,                  // copy number
				     checkOverlaps);     // check overlaps
      allPhysicalVolumes.insert(aPlacement);
    }
}

BDSMagnetOuter* BDSMagnetOuterFactoryPolesSquare::CommonConstructor(const G4String& name,
								    G4double        length,
								    BDSBeamPipe*    beamPipe,
								    G4int           orderIn,
								    G4double        magnetContainerRadiusIn,
								    const BDSMagnetOuterInfo* recipe)
{
  BDSMagnetOuter* outer = BDSMagnetOuterFactoryPolesBase::CommonConstructor(name, length, beamPipe,
									    orderIn, magnetContainerRadiusIn, recipe);

  std::set<G4LogicalVolume*> tempLVs(poleLVs.begin(), poleLVs.end());
  outer->RegisterLogicalVolume(tempLVs);
  if (sensitiveOuter)
    {outer->RegisterSensitiveVolume(tempLVs, BDSSDType::energydep);}
  
  return outer;
}
  
