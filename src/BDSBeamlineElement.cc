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
#include "BDSBeamlineElement.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSExtentGlobal.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSamplerType.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "G4AssemblyVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <algorithm>
#include <ostream>
#include <set>
#include <string>

BDSBeamlineElement::BDSBeamlineElement(BDSAcceleratorComponent* componentIn,
				       const G4ThreeVector&     positionStartIn,
				       const G4ThreeVector&     positionMiddleIn,
				       const G4ThreeVector&     positionEndIn,
				       G4RotationMatrix*        rotationStartIn,
				       G4RotationMatrix*        rotationMiddleIn,
				       G4RotationMatrix*        rotationEndIn,
				       const G4ThreeVector&     referencePositionStartIn,
				       const G4ThreeVector&     referencePositionMiddleIn,
				       const G4ThreeVector&     referencePositionEndIn,
				       G4RotationMatrix*        referenceRotationStartIn,
				       G4RotationMatrix*        referenceRotationMiddleIn,
				       G4RotationMatrix*        referenceRotationEndIn,
				       G4double                 sPositionStartIn,
				       G4double                 sPositionMiddleIn,
				       G4double                 sPositionEndIn,
               G4double                 synchronousTMiddleIn,
               G4double                 startMomentumIn,
               G4double                 startKineticEnergyIn,
				       BDSTiltOffset*           tiltOffsetIn,
				       BDSSamplerInfo*          samplerInfoIn,
				       G4int                    indexIn):
  component(componentIn),
  positionStart(positionStartIn), positionMiddle(positionMiddleIn), positionEnd(positionEndIn),
  rotationStart(rotationStartIn), rotationMiddle(rotationMiddleIn), rotationEnd(rotationEndIn),
  referencePositionStart(referencePositionStartIn),
  referencePositionMiddle(referencePositionMiddleIn),
  referencePositionEnd(referencePositionEndIn),
  referenceRotationStart(referenceRotationStartIn),
  referenceRotationMiddle(referenceRotationMiddleIn),
  referenceRotationEnd(referenceRotationEndIn),
  sPositionStart(sPositionStartIn), sPositionMiddle(sPositionMiddleIn), sPositionEnd(sPositionEndIn),
  synchronousTMiddle(synchronousTMiddleIn),
  startMomentum(startMomentumIn),
  startKineticEnergy(startKineticEnergyIn),
  tiltOffset(tiltOffsetIn),
  samplerInfo(samplerInfoIn),
  samplerPlacementTransform(nullptr),
  index(indexIn)
{
  componentIn->IncrementCopyNumber(); // increase copy number (starts at -1)
  copyNumber = componentIn->GetCopyNumber();
  /// placement name (starting at 0)
  placementName = componentIn->GetName() + "_" + std::to_string(copyNumber);
  
  // create the placement transform from supplied rotation matrices and vector
  placementTransform   = new G4Transform3D(*rotationMiddle, positionMiddle);
  placementTransformCL = new G4Transform3D(*referenceRotationMiddle, referencePositionMiddle);

  // calculate sampler central position slightly away from end position of element.
  auto samplerType = GetSamplerType();
  if (samplerType == BDSSamplerType::plane)
    {
      G4ThreeVector dZLocal = G4ThreeVector(0,0,1);
      // if we have a sampler, add on the thickness of the sampler to the padding
      dZLocal *= (BDSBeamline::PaddingLength() + BDSSamplerPlane::ChordLength())*0.5;
      dZLocal.transform(*referenceRotationEnd);
      G4ThreeVector samplerPosition = referencePositionEnd + dZLocal;
      samplerPlacementTransform = new G4Transform3D(*referenceRotationEnd, samplerPosition);
    }
  else if (samplerType == BDSSamplerType::cylinder)
    {samplerPlacementTransform = new G4Transform3D(*referenceRotationMiddle, referencePositionMiddle);}
}

BDSBeamlineElement::~BDSBeamlineElement()
{
  delete rotationStart;
  delete rotationMiddle;
  delete rotationEnd;
  delete referenceRotationStart;
  delete referenceRotationMiddle;
  delete referenceRotationEnd;
  delete placementTransform;
  delete placementTransformCL;
  delete samplerPlacementTransform;
  delete samplerInfo;
}

std::set<G4VPhysicalVolume*> BDSBeamlineElement::GetPVsFromAssembly(G4AssemblyVolume* av)
{
  // this is obtuse because of the lack of access in G4Assembly
  std::vector<G4VPhysicalVolume*> allPlacementsFromThisAssembly;
  auto it = av->GetVolumesIterator();
  for (G4int i = 0; i < (G4int)av->TotalImprintedVolumes(); it++, i++)
    {allPlacementsFromThisAssembly.push_back(*it);}
  std::set<G4VPhysicalVolume*> result(allPlacementsFromThisAssembly.begin(), allPlacementsFromThisAssembly.end());
  return result;
}

std::set<G4VPhysicalVolume*> BDSBeamlineElement::PlaceElement(const G4String&    pvName,
							      G4VPhysicalVolume* motherPV,
							      G4bool             useCLPlacementTransform,
							      G4int              pvCopyNumber,
							      G4bool             checkOverlaps) const
{
  G4Transform3D* pvTransform = GetPlacementTransform();
  if (useCLPlacementTransform)
    {pvTransform = GetPlacementTransformCL();}
  
  std::set<G4VPhysicalVolume*> result;
  if (component->ContainerIsAssembly())
    {
      G4AssemblyVolume* av = component->GetContainerAssemblyVolume();
      auto existingPVSet = GetPVsFromAssembly(av);
      av->MakeImprint(motherPV->GetLogicalVolume(),
		      *pvTransform,
		      pvCopyNumber,checkOverlaps);
      
      // need to get pvs before and afterwards and o the difference
      auto updatedPVSet = GetPVsFromAssembly(av);
      std::set_difference(updatedPVSet.begin(), updatedPVSet.end(),
			  existingPVSet.begin(), existingPVSet.end(),
			  std::inserter(result, result.begin()));
    }
  else
    {
      G4LogicalVolume* lv = component->GetContainerLogicalVolume();
      result.insert(new G4PVPlacement(*pvTransform,       // placement transform
				      pvName,             // placement name
				      lv,                 // volume to be placed
				      motherPV,           // volume to place it in
				      false,              // no boolean operation
				      pvCopyNumber,       // copy number
				      checkOverlaps));    // overlap checking
    }
  return result;
}

BDSExtentGlobal BDSBeamlineElement::GetExtentGlobal() const
{
  BDSExtent ext = component->GetExtent();
  BDSExtentGlobal extG = BDSExtentGlobal(ext, *GetPlacementTransform());
  return extG;
}

G4ThreeVector BDSBeamlineElement::InputFaceNormal() const
{
  G4ThreeVector inputFNLocal = component->InputFaceNormal();
  if (!tiltOffset) // no tilt so the same as local
    {return inputFNLocal;}
  else
    {
      G4ThreeVector inputFNGlobal = inputFNLocal.rotateZ(tiltOffset->GetTilt());
      return inputFNGlobal;
    }
}

G4ThreeVector BDSBeamlineElement::OutputFaceNormal() const
{
  G4ThreeVector outputFNLocal = component->OutputFaceNormal();
  if (!tiltOffset) // no tilt so the same as local
    {return outputFNLocal;}
  else
    {
      G4ThreeVector outputFNGlobal = outputFNLocal.rotateZ(-tiltOffset->GetTilt());
      return outputFNGlobal;
    }
}

void BDSBeamlineElement::UpdateSamplerPlacementTransform(const G4Transform3D& tranfsormIn)
{
  delete samplerPlacementTransform;
  samplerPlacementTransform = new G4Transform3D(tranfsormIn);
}

std::ostream& operator<< (std::ostream& out, BDSBeamlineElement const &e)
{
  out << "Beamline element: " << e.component->GetName() << G4endl;
  out << "Start, middle & end position: "
      << e.GetPositionStart()  << " " << e.GetPositionMiddle()  << " " << e.GetPositionEnd()  << G4endl
      << "Start, middle & end rotation: "
      << *(e.GetRotationStart())  << " " << *(e.GetRotationMiddle())  << " " << *(e.GetRotationEnd())  << G4endl
      << "Start, middle & end s position: "
      << e.GetSPositionStart() << " " << e.GetSPositionMiddle() << " " << e.GetSPositionEnd() << G4endl;

  return out;
}

G4bool BDSBeamlineElement::Overlaps(const BDSBeamlineElement* otherElement) const
{
  BDSExtentGlobal thisGlobal  = BDSExtentGlobal(component->GetExtent(),
						*(GetPlacementTransform()));
  BDSExtentGlobal otherGlobal = BDSExtentGlobal(otherElement->GetAcceleratorComponent()->GetExtent(),
						*(otherElement->GetPlacementTransform()));

  return thisGlobal.Overlaps(otherGlobal);
}


