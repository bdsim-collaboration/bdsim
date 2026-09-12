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
#include "BDSAcceleratorModel.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSCollimatorCrystal.hh"
#include "BDSCrystalFactory.hh"
#include "BDSCrystalInfo.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGeometryComponent.hh"
#include "BDSSDType.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include <cmath>

BDSCollimatorCrystal::BDSCollimatorCrystal(const G4String&  nameIn, 
					   G4double         lengthIn,
					   BDSBeamPipeInfo* beamPipeInfoIn,
					   BDSCrystalInfo*  crystalInfoLeftIn,
					   BDSCrystalInfo*  crystalInfoRightIn,
					   G4double         halfGapLeftIn,
					   G4double         halfGapRightIn,
					   G4double         angleYAxisLeftIn,
					   G4double         angleYAxisRightIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "crystalcol", beamPipeInfoIn),
  crystalInfoLeft(crystalInfoLeftIn),
  crystalInfoRight(crystalInfoRightIn),
  halfGapLeft(halfGapLeftIn),
  halfGapRight(halfGapRightIn),
  angleYAxisLeft(angleYAxisLeftIn),
  angleYAxisRight(angleYAxisRightIn),
  crystalLeft(nullptr),
  crystalRight(nullptr)
{
#ifdef USE_SIXTRACKLINK
  G4cout << "Left " << crystalInfoLeftIn << G4endl;
  G4cout << "Right " << crystalInfoRightIn << G4endl;
#endif
  if (crystalInfoLeft)
    {crystalInfoLeft->bendingAngleYAxis *= -1.0;}
#ifdef USE_SIXTRACKLINK
  G4cout << "halfGapLeftIn " << halfGapLeftIn << G4endl;
  G4cout << "halfGapRightIn " << halfGapRightIn << G4endl;
#endif
}

BDSCollimatorCrystal::~BDSCollimatorCrystal()
{
  delete crystalInfoLeft;
  delete crystalInfoRight;
}

void BDSCollimatorCrystal::Build()
{
  // we don't need to set user limits because that is done in the beam pipe factory
  BDSBeamPipeFactory* factory = BDSBeamPipeFactory::Instance();
  BDSBeamPipe* pipe = factory->CreateBeamPipe(name,
					      chordLength,
					      beamPipeInfo);

  RegisterDaughter(pipe);
  
  // make the beam pipe container, this object's container
  containerLogicalVolume = pipe->GetContainerLogicalVolume();
  containerSolid         = pipe->GetContainerSolid();

  // register vacuum volume (for biasing)
  SetAcceleratorVacuumLogicalVolume(pipe->GetVacuumLogicalVolume());

  // update extents
  InheritExtents(pipe);

  // update faces
  SetInputFaceNormal(pipe->InputFaceNormal());
  SetOutputFaceNormal(pipe->OutputFaceNormal());

  BDSCrystalFactory* fac = new BDSCrystalFactory();
  if (crystalInfoLeft)
    {
      crystalLeft = fac->CreateCrystal(name + "_left", crystalInfoLeft);
      RegisterDaughter(crystalLeft);
      RegisterCrystalLVs(crystalLeft);
    }
  if (crystalInfoRight)
    {
      crystalRight = fac->CreateCrystal(name + "_right", crystalInfoRight);
      RegisterDaughter(crystalRight);
      RegisterCrystalLVs(crystalRight);
    }
  delete fac;

  if (crystalLeft)
    {
      G4ThreeVector objectOffset     = crystalLeft->GetPlacementOffset();
      G4double dx                    = TransverseOffsetToEdge(crystalLeft, angleYAxisLeft, true);
      G4ThreeVector colOffsetL       = G4ThreeVector(halfGapLeft+dx,0,0);
      G4ThreeVector placementOffsetL = objectOffset + colOffsetL; // 'L' in p offset to avoid class with BDSGeometry Component member
      G4RotationMatrix* placementRot = crystalLeft->GetPlacementRotation();
      if (BDS::IsFinite(angleYAxisLeft))
	{
	  if (!placementRot)
	    {
	      placementRot = new G4RotationMatrix();
	      RegisterRotationMatrix(placementRot);
	    }
	  G4ThreeVector localUnitY = G4ThreeVector(0,1,0).transform(*placementRot);
	  placementRot->rotate(-angleYAxisLeft, localUnitY); // rotate about local unitY
	  // note minus sign to rotate *away* from centre
	}

      // check if it'll fit..
      // use the collOffsetL as the specific solid might require a large offset (e.g. cylinder or torus)
      BDSExtent extShifted = (crystalLeft->GetExtent()).Translate(colOffsetL);
      BDSExtent thisExtent = GetExtent(); // actually outer extent of beam pipe
      G4bool safe = thisExtent.Encompasses(extShifted);
      // second stricter check - TODO - use aperture check in future
      BDSExtent innerRadius = BDSExtent(beamPipeInfo->IndicativeRadiusInner(),
                                        beamPipeInfo->IndicativeRadiusInner(),
                                        0.5*chordLength);
      G4bool safe2 = innerRadius.Encompasses(extShifted);
      if (!safe || !safe2)
	      {BDS::Warning(__METHOD_NAME__, "Left crystal potential overlap in component \"" + name +"\"");}
      LongitudinalOverlap(crystalLeft->GetExtent(), angleYAxisLeft, "Left");

#ifdef USE_SIXTRACKLINK
      G4cout << "left crystal placement offset   " << placementOffsetL << G4endl;
      if (placementRot)
      {
        G4cout << "left crystal placement rotation " << *placementRot << G4endl;
      }
#endif
      
      G4LogicalVolume* vac = *(GetAcceleratorVacuumLogicalVolumes().begin()); // take the first one
      auto cL = new G4PVPlacement(placementRot,
				  placementOffsetL,
				  crystalLeft->GetContainerLogicalVolume(),
				  name + "_crystal_left_pv",
				  vac,
				  false,
				  0,
				  true); // always check
      RegisterPhysicalVolume(cL);
#ifdef USE_SIXTRACKLINK
      G4cout << "Placement of left crystal " << placementOffsetL << G4endl;
#endif
    }
  if (crystalRight)
    {
      G4ThreeVector objectOffset     = crystalRight->GetPlacementOffset();
      G4double dx                    = TransverseOffsetToEdge(crystalRight, angleYAxisRight, false);
      G4ThreeVector colOffsetR       = G4ThreeVector(-(halfGapRight+dx),0,0); // -ve as r.h. coord system
      G4ThreeVector placementOffsetL = objectOffset + colOffsetR;
      G4RotationMatrix* placementRot = crystalRight->GetPlacementRotation();
      if (BDS::IsFinite(angleYAxisRight))
	{
	  if (!placementRot)
	    {
	      placementRot = new G4RotationMatrix();
	      RegisterRotationMatrix(placementRot);
	    }
	  G4ThreeVector localUnitY = G4ThreeVector(0,1,0).transform(*placementRot);
	  placementRot->rotate(angleYAxisRight, localUnitY); // rotate about local unitY
	}
      
      // check if it'll fit..
      BDSExtent extShifted = (crystalRight->GetExtent()).Translate(colOffsetR);
      BDSExtent thisExtent = GetExtent();
      G4bool safe = thisExtent.Encompasses(extShifted);
      // second stricter check - TODO - use aperture check in future
      BDSExtent innerRadius = BDSExtent(beamPipeInfo->IndicativeRadiusInner(),
                                        beamPipeInfo->IndicativeRadiusInner(),
                                        0.5*chordLength);
      G4bool safe2 = innerRadius.Encompasses(extShifted);
      if (!safe || !safe2)
        {BDS::Warning(__METHOD_NAME__, "Right crystal potential overlap in component \"" + name +"\"");}
      LongitudinalOverlap(crystalRight->GetExtent(), angleYAxisRight, "Right");

#ifdef USE_SIXTRACKLINK
    G4cout << "right crystal placement offset   " << placementOffsetL << G4endl;
    if (placementRot)
    {
      G4cout << "right crystal placement rotation " << *placementRot << G4endl;
    }
#endif

      G4LogicalVolume* vac = *(GetAcceleratorVacuumLogicalVolumes().begin()); // take the first one
      auto cR = new G4PVPlacement(placementRot,
				  placementOffsetL,
				  crystalRight->GetContainerLogicalVolume(),
				  name + "_crystal_right_pv",
				  vac,
				  false,
				  0,
				  true); // always check
      RegisterPhysicalVolume(cR);
    }
  if (!crystalLeft && !crystalRight)
    {throw BDSException(__METHOD_NAME__, "Error in crystal construction in component \"" + name + "\"");}
}

G4String BDSCollimatorCrystal::Material() const
{
  auto bpMat = GetBeamPipeInfo()->beamPipeMaterial;
  if (bpMat)
    {return bpMat->GetName();}
  else
    {return BDSAcceleratorComponent::Material();} // none
}

void BDSCollimatorCrystal::LongitudinalOverlap(const BDSExtent& extCrystal,
					       G4double         crystalAngle,
					       const G4String&  side) const
{
  G4double zExt = extCrystal.MaximumZ();
  G4double dz   = zExt*std::tan(crystalAngle);

  G4bool overlap = 2*zExt + 2*std::abs(dz) > (chordLength - 2*lengthSafety);

  if (overlap)
    {
      G4cout << "Crystal of length " << 2*zExt/CLHEP::mm << " mm is at angle "
	     << crystalAngle / CLHEP::mrad << " mrad and container is "
	     << chordLength/CLHEP::m << " m long." << G4endl;
      throw BDSException(__METHOD_NAME__, side + " crystal won't fit in collimator due to rotation.");
    }
}
 
void BDSCollimatorCrystal::RegisterCrystalLVs(const BDSCrystal* crystal) const
{
  auto crystals    = BDSAcceleratorModel::Instance()->VolumeSet("crystals");
  auto collimators = BDSAcceleratorModel::Instance()->VolumeSet("collimators");
  for (auto lv : crystal->GetAllLogicalVolumes())
    {
      crystals->insert(lv);
      collimators->insert(lv);
    }
}

G4double BDSCollimatorCrystal::TransverseOffsetToEdge(const BDSCrystal* crystal,
						      G4double          placementAngle,
						      G4bool            left) const
{
  const BDSCrystalInfo* recipe = crystal->recipe;
  G4double result = 0;
  G4double factor = left ? 1.0 : -1.0;
  switch (recipe->shape.underlying())
    {
    case BDSCrystalType::box:
      {
	result =  0.5*recipe->lengthZ * std::sin(placementAngle);
	result += 0.5*recipe->lengthX * std::cos(placementAngle);
	break;
      }
    case BDSCrystalType::cylinder:
    case BDSCrystalType::torus:
      {
	G4double halfAngle = 0.5 * recipe->bendingAngleYAxis;
	if (!BDS::IsFinite(halfAngle)) // like a box
	  {
	    result =  0.5*recipe->lengthZ * std::sin(placementAngle);
	    result += 0.5*recipe->lengthX * std::cos(placementAngle);
	  }
	else
	  {
	    // use 2-vectors to do all the maths for us
	    // 2d vector from centre of curvature to middle of crystal
	    G4TwoVector a(recipe->BendingRadiusHorizontal(), 0);
	    G4TwoVector b(a); // copy it
	    b.rotate(-halfAngle); // rotate to the front face centre
	    // difference between these two is the vector from centre of crystal frame (which is the
	    // middle of the crystal half way along z) to the front centre face
	    G4TwoVector dxy = b - a;
	    // construct a perpendicular vector for the width along the front face
	    // 'factor' is because for the left crystal we want the right inside front face and the left inside
	    // front face for the right crystal
	    G4TwoVector frontFaceX = dxy.orthogonal() * factor;
	    // 1/2 the width x this unit vector gives a vector from the centre of the front face
	    // to the inside front edge
	    G4TwoVector frontCentreToEdge = frontFaceX.unit() * 0.5*recipe->lengthX;
	    // add this to the one from the middle of the crystal
	    G4TwoVector resultV = dxy + frontCentreToEdge;
	    // update by placement angle
	    resultV.rotate(-factor*placementAngle);
	    // result is horizontal (x) component of this - the sign should be right throughout
	    result = -factor * resultV.x();
	  }
	break;
      }
    default:
      {break;}
    }
  
  return result;
}
