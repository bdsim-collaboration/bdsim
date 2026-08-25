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
#include "BDSApertureInfo.hh"
#include "BDSBeamlineElement.hh"
#include "BDSApertureType.hh"
#include "BDSBeamline.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSLinkOpaqueBox.hh"
#include "BDSMagnet.hh"
#include "BDSMaterials.hh"
#include "BDSParallelWorldSampler.hh"
#include "BDSParser.hh"
#include "BDSSamplerCustom.hh"
#include "BDSSamplerPlacementRecord.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSSDManager.hh"
#include "BDSSDSamplerLink.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "G4Box.hh"
#include "G4AssemblyVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4TwoVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Point3D.h"

#include <algorithm>
#include <cmath>
#include <limits>

BDSLinkOpaqueBox::BDSLinkOpaqueBox(BDSAcceleratorComponent* acceleratorComponentIn,
				   BDSTiltOffset* tiltOffsetIn,
				   G4double outputSamplerRadiusIn):
  BDSGeometryComponent(nullptr, nullptr),
  component(acceleratorComponentIn),
  componentBeamline(nullptr),
  outputSamplerRadius(outputSamplerRadiusIn),
  inputClearance(0),
  outputClearance(0),
  tilt(tiltOffsetIn->GetTilt()),
  offsetX(tiltOffsetIn->GetXOffset()),
  offsetY(tiltOffsetIn->GetYOffset()),
  sampler(nullptr)
{
  const G4double chordLength = component->GetChordLength();

  // Ask the normal beam-line builder for every placement and reference frame.
  // AddComponent transparently expands a BDSLine, which keeps RF bodies and
  // their entrance / exit fringes in one delegated link element.
  componentBeamline = new BDSBeamline();
  componentBeamline->AddComponent(
    component,
    new BDSTiltOffset(offsetX, offsetY, tilt));
  const BDSBeamlineElement* first = componentBeamline->front();
  const BDSBeamlineElement* last  = componentBeamline->back();
  const G4ThreeVector desiredInput(0, 0, -0.5*chordLength);
  nativeToOpaque = G4Transform3D(
    G4RotationMatrix(), desiredInput - first->GetReferencePositionStart());
  auto frame = [this](G4RotationMatrix* rotation,
                      const G4ThreeVector& position)
  {
    return nativeToOpaque * G4Transform3D(*rotation, position);
  };
  const G4Transform3D inputFrame = frame(
    first->GetReferenceRotationStart(), first->GetReferencePositionStart());
  transformToOutput = frame(
    last->GetReferenceRotationEnd(), last->GetReferencePositionEnd());
  offsetToStart = inputFrame.getTranslation();
  const G4double gap                = 10 * CLHEP::cm;
  const G4double opaqueBoxThickness = 10 * CLHEP::mm;
  G4String name = component->GetName();

  G4double mx = 0;
  G4double my = 0;
  G4double mz = 0;
  G4Transform3D opaqueToOutput = transformToOutput.inverse();
  G4double minimumZ = std::numeric_limits<G4double>::max();
  G4double maximumOutputZ = std::numeric_limits<G4double>::lowest();
  for (const auto& native : *componentBeamline)
    {
      const G4Transform3D componentTransform = nativeToOpaque *
        G4Transform3D(*native->GetRotationMiddle(), native->GetPositionMiddle());
      for (const auto& corner : native->GetExtent().AllBoundaryPoints())
        {
          const auto transformed = componentTransform *
            (HepGeom::Point3D<G4double>)corner;
          const auto inOutput = opaqueToOutput * transformed;
          mx = std::max(mx, std::abs(transformed.x()));
          my = std::max(my, std::abs(transformed.y()));
          mz = std::max(mz, std::abs(transformed.z()));
          minimumZ = std::min(minimumZ, transformed.z());
          maximumOutputZ = std::max(maximumOutputZ, inOutput.z());
        }
    }
  outputSamplerRadius = std::max(outputSamplerRadius, std::hypot(mx, my));
  // Reuse BDSIM's existing samplerDiameter option as an explicit link-sampler
  // override. Its 5 m default is intentionally ignored unless the user set
  // it, so ordinary link applications retain a geometry-sized sampler.
  const auto& options = BDSParser::Instance()->GetOptions();
  if (options.HasBeenSet("samplerDiameter"))
    {
      outputSamplerRadius = std::max(
        outputSamplerRadius,
        0.5*BDSGlobalConstants::Instance()->SamplerDiameter());
    }
  // The output plane is deliberately downstream of every angled face. Give
  // it transverse headroom as well, otherwise wide-angle shower products
  // that have genuinely left the component can miss a sampler whose radius
  // is exactly equal to the geometry envelope.
  outputSamplerRadius += gap;
  // Externally the link state is defined on the nominal input plane.  In a
  // standalone line, however, a particle coming from the preceding drift can
  // enter a protruding angled face before reaching that plane. Reconstruct
  // this short upstream transport when BDSIM's component geometry says that
  // the input face is angled.  A rectangular bend is the one exception to
  // that generic test: e1=0 denotes its rectangular face, which is oblique to
  // the incoming reference trajectory even though AngledInputFace() reports
  // the stored pole-face rotation as zero.
  G4bool protrudingInputFace = false;
  G4bool protrudingOutputFace = false;
  for (const auto& native : *componentBeamline)
    {
      const auto* child = native->GetAcceleratorComponent();
      const G4bool nativeBend = dynamic_cast<const BDSMagnet*>(child) &&
        BDS::IsFinite(child->GetAngle());
      protrudingInputFace |= child->AngledInputFace() || nativeBend;
      // A finite parallel-world sampler centred exactly on the nominal exit
      // starts recording on its upstream face.  Keep it beyond every field,
      // including straight RF cavities and maps, so the complete element and
      // any exit fringe are tracked before the hit is projected back.
      protrudingOutputFace |= child->AngledOutputFace() || child->HasAField();
    }
  inputClearance = protrudingInputFace ?
    std::max(0.0, offsetToStart.z() - minimumZ) + 1*CLHEP::cm : 0.0;
  // Finish a curved field before returning the particle.  The link
  // sampler records in the nominal output frame and projects its hit back to
  // that plane, so this is invisible to the external tracking interface. The
  // clearance is derived from the complete component extent, including the
  // longitudinal support of a field map attached to generic geometry.
  outputClearance = protrudingOutputFace ?
    std::max(0.0, maximumOutputZ) + 1*CLHEP::cm : 0.0;
  const G4ThreeVector outputPosition = transformToOutput.getTranslation();
  mx = std::max(mx, std::abs(outputPosition.x()) + outputSamplerRadius);
  my = std::max(my, std::abs(outputPosition.y()) + outputSamplerRadius);
  mz = std::max(mz, std::abs(outputPosition.z()) + outputSamplerRadius);
  transformToStart = inputFrame * G4Transform3D(
    G4RotationMatrix(), G4ThreeVector(0, 0, -inputClearance));
  G4double mr = std::max({mx, my, outputSamplerRadius});
  G4Box* terminatorBoxOuter = new G4Box(name + "_terminator_box_outer_solid",
					mr + gap + opaqueBoxThickness,
					mr + gap + opaqueBoxThickness,
					mz + gap + opaqueBoxThickness);
  RegisterSolid(terminatorBoxOuter);
  G4Box* terminatorBoxInner = new G4Box(name + "_terminator_box_inner_solid",
					mr + gap,
					mr + gap,
					mz + gap);
  RegisterSolid(terminatorBoxInner);
  G4SubtractionSolid* opaqueBox = new G4SubtractionSolid(name + "_opaque_box_solid",
							 terminatorBoxOuter,
							 terminatorBoxInner);
  RegisterSolid(opaqueBox);
  G4LogicalVolume* opaqueBoxLV = new G4LogicalVolume(opaqueBox,
						     BDSMaterials::Instance()->GetMaterial("G4_Galactic"),
						     name + "_opaque_box_lv");
  RegisterLogicalVolume(opaqueBoxLV);

  G4UserLimits* termUL = new G4UserLimits();
  termUL->SetUserMinEkine(std::numeric_limits<double>::max());
  RegisterUserLimits(termUL);
  opaqueBoxLV->SetUserLimits(termUL);
  
  G4VisAttributes* obVis = new G4VisAttributes(*BDSColours::Instance()->GetColour("opaquebox"));
  obVis->SetVisibility(true);
  opaqueBoxLV->SetVisAttributes(obVis);
  RegisterVisAttributes(obVis);
  
  G4double ls = BDSGlobalConstants::Instance()->LengthSafetyLarge();
  G4double margin = gap + opaqueBoxThickness + ls;
  G4double xsize = mr + margin;
  G4double ysize = mr + margin;
  G4double zsize = mz + margin;
  containerSolid = new G4Box(name + "_opaque_box_vacuum_solid",
			     xsize,
			     ysize,
			     zsize);
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       BDSMaterials::Instance()->GetMaterial("G4_Galactic"),
					       name + "_container_lv");
  containerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->ContainerVisAttr());

  // auto boxPlacement = 
  new G4PVPlacement(nullptr,
		    G4ThreeVector(),
		    opaqueBoxLV,
		    name + "_opaque_box_pv",
		    containerLogicalVolume,
		    false,
		    1,
		    true);

  for (const auto& native : *componentBeamline)
    {
      BDSAcceleratorComponent* child = native->GetAcceleratorComponent();
      G4Transform3D placementTransform = nativeToOpaque *
        G4Transform3D(*native->GetRotationMiddle(), native->GetPositionMiddle());
      if (child->ContainerIsAssembly())
        {
          child->GetContainerAssemblyVolume()->MakeImprint(
            containerLogicalVolume, placementTransform,
            native->GetCopyNo(), true);
        }
      else
        {
          new G4PVPlacement(placementTransform,
                            child->GetContainerLogicalVolume(),
                            child->GetName() + "_link_pv",
                            containerLogicalVolume,
                            false,
                            native->GetCopyNo(),
                            true);
        }
    }
  
  outerExtent = BDSExtent(xsize, ysize, zsize);

}

BDSLinkOpaqueBox::~BDSLinkOpaqueBox()
{
  // Samplers are owned and deleted by BDSSamplerRegistry.
  delete componentBeamline;
}

void BDSLinkOpaqueBox::SetFieldLinkTransform(const G4Transform3D& opaqueToGlobal)
{
  // Native magnets already carry the element tilt and offset in their field
  // recipe. A generic ``element`` field follows the component placement.
  // Preserve that distinction while supplying the fixed world transform.
  for (const auto& native : *componentBeamline)
    {
      BDSAcceleratorComponent* child = native->GetAcceleratorComponent();
      const G4Transform3D placementToOpaque = nativeToOpaque *
        G4Transform3D(*native->GetRotationMiddle(), native->GetPositionMiddle());
      const G4Transform3D referenceToOpaque = nativeToOpaque *
        G4Transform3D(*native->GetReferenceRotationMiddle(),
                      native->GetReferencePositionMiddle());
      child->SetFieldLinkTransform(
        opaqueToGlobal * (dynamic_cast<BDSMagnet*>(child)
                            ? referenceToOpaque : placementToOpaque));
    }
}

G4int BDSLinkOpaqueBox::PlaceOutputSampler(BDSParallelWorldSampler* samplerWorld,
                                           const G4Transform3D& outputToGlobal)
{  
  G4String samplerName = component->GetName() + "_out";
  BDSApertureType apt = BDSApertureType::circular;
  BDSApertureInfo ap = BDSApertureInfo(apt, outputSamplerRadius, 0, 0, 0);
  sampler = new BDSSamplerCustom(samplerName, ap);
  sampler->GetContainerLogicalVolume()->SetSensitiveDetector(BDSSDManager::Instance()->SamplerLink());
  const G4Transform3D samplerToGlobal = outputToGlobal * G4Transform3D(
    G4RotationMatrix(), G4ThreeVector(0, 0, outputClearance));
  return samplerWorld->RegisterSamplerForConstruction(
    samplerName, sampler, samplerToGlobal);
}
