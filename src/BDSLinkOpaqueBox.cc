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

namespace
{
  G4bool AngledGeometryFace(const G4ThreeVector& normal,
                            const G4ThreeVector& reference)
  {
    return BDS::IsFinite(normal.cross(reference).mag2());
  }
}

BDSLinkOpaqueBox::BDSLinkOpaqueBox(BDSAcceleratorComponent* acceleratorComponentIn,
				   BDSTiltOffset* tiltOffsetIn,
				   G4double outputSamplerRadiusIn,
                                   BDSAcceleratorComponent* inputGuardIn,
                                   BDSAcceleratorComponent* outputGuardIn,
                                   G4double inputGuardLengthIn,
                                   G4double outputGuardLengthIn):
  BDSGeometryComponent(nullptr, nullptr),
  component(acceleratorComponentIn),
  componentBeamline(nullptr),
  outputSamplerRadius(outputSamplerRadiusIn),
  inputClearance(0),
  outputClearance(0),
  interfacePadding(0),
  tilt(tiltOffsetIn->GetTilt()),
  offsetX(tiltOffsetIn->GetXOffset()),
  offsetY(tiltOffsetIn->GetYOffset()),
  nominalStartIndex(0),
  nominalEndIndex(0),
  sampler(nullptr)
{
  const G4double chordLength = component->GetChordLength();
  const G4bool guardsBuilt = inputGuardIn || outputGuardIn;

  // Ask the normal beam-line builder for every placement and reference frame.
  // AddComponent transparently expands a BDSLine, which keeps RF bodies and
  // their entrance / exit fringes in one delegated link element.
  componentBeamline = new BDSBeamline();
  // A delegated component retains the normal BDSBeamline navigation gap at
  // both external boundaries.  These gaps are compensated in the state
  // exchanged with the calling tracking interface.
  interfacePadding = BDSBeamline::PaddingLength();
  if (inputGuardIn)
    {componentBeamline->AddComponent(inputGuardIn);}
  nominalStartIndex = (G4int)componentBeamline->size();
  componentBeamline->AddComponent(
    component,
    new BDSTiltOffset(offsetX, offsetY, tilt));
  nominalEndIndex = (G4int)componentBeamline->size() - 1;
  if (outputGuardIn)
    {componentBeamline->AddComponent(outputGuardIn);}
  const BDSBeamlineElement* first = componentBeamline->front();
  const BDSBeamlineElement* last  = componentBeamline->back();
  const BDSBeamlineElement* nominalFirst = componentBeamline->at(nominalStartIndex);
  const BDSBeamlineElement* nominalLast  = componentBeamline->at(nominalEndIndex);
  const G4ThreeVector desiredInput(
    0, 0, -0.5*(chordLength + inputGuardLengthIn + outputGuardLengthIn));
  nativeToOpaque = G4Transform3D(
    G4RotationMatrix(), desiredInput - first->GetReferencePositionStart());
  auto frame = [this](G4RotationMatrix* rotation,
                      const G4ThreeVector& position)
  {
    return nativeToOpaque * G4Transform3D(*rotation, position);
  };
  const G4Transform3D inputFrame = frame(
    nominalFirst->GetReferenceRotationStart(),
    nominalFirst->GetReferencePositionStart());
  transformToOutput = frame(
    nominalLast->GetReferenceRotationEnd(),
    nominalLast->GetReferencePositionEnd());
  const G4Transform3D trackingOutputFrame = frame(
    last->GetReferenceRotationEnd(), last->GetReferencePositionEnd());
  offsetToStart = inputFrame.getTranslation();
  const G4double gap                = 10 * CLHEP::cm;
  const G4double opaqueBoxThickness = 10 * CLHEP::mm;
  G4String name = component->GetName();

  G4double mx = 0;
  G4double my = 0;
  G4double mz = 0;
  G4Transform3D opaqueToOutput = transformToOutput.inverse();
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
  // Physical guards let Geant4 navigate across protruding angled faces.  All
  // guard and padding clearance is removed from the exchanged state.
  // Retain the previous virtual-clearance path for components that do not
  // require face-matched guards.
  if (guardsBuilt)
    {
      // Use the frames produced by the ordinary BDSBeamline builder.  Their
      // separation includes both the guard body and BDSIM's standard
      // inter-component navigation padding, exactly as in a normal beamline.
      // The interface compensates both the guard body and normal padding
      // without changing the delegated component geometry.
      const G4Transform3D trackingOutputInNominal =
        transformToOutput.inverse() * trackingOutputFrame;
      outputClearance = std::max(
        0.0, trackingOutputInNominal.getTranslation().z());
    }
  else
    {
      G4bool protrudingOutputFace = false;
      for (const auto& native : *componentBeamline)
        {
          const auto* child = native->GetAcceleratorComponent();
          // A finite parallel-world sampler centred exactly on the nominal
          // exit starts recording on its upstream face. Keep it beyond every
          // field, including straight RF cavities and maps.
          protrudingOutputFace |= AngledGeometryFace(
            child->GeometryOutputFaceNormal(), G4ThreeVector(0, 0, 1)) ||
            child->HasAField();
        }
      outputClearance = protrudingOutputFace ?
        std::max(0.0, maximumOutputZ) + 1*CLHEP::cm : 0.0;
      // Even a straight or field-free delegated component has the same
      // external navigation gaps as that component in a normal BDSBeamline.
      outputClearance = std::max(outputClearance, interfacePadding);
    }
  // Match ordinary BDSIM generation at the nominal reference entrance.  The
  // guard may extend upstream of this plane for an angled face, but starting
  // at the guard entrance would track an additional, x-dependent wedge of
  // field that an ordinary standalone element never sees.  Backstep only by
  // BDSIM's standard safety distance to avoid creation exactly on a boundary.
  const G4double injectionSafety = BDSGlobalConstants::Instance()->LengthSafety();
  inputClearance = injectionSafety;
  const G4ThreeVector outputPosition = transformToOutput.getTranslation();
  mx = std::max(mx, std::abs(outputPosition.x()) + outputSamplerRadius);
  my = std::max(my, std::abs(outputPosition.y()) + outputSamplerRadius);
  // The sampler radius is purely transverse.  Using it in z needlessly moves
  // the delegated element far from the world origin (by metres for the usual
  // 5 m samplerDiameter), which magnifies otherwise avoidable floating-point
  // differences from an ordinary standalone beam line.
  mz = std::max(mz,
                std::abs(outputPosition.z()) + 0.5*BDSSamplerCustom::ChordLength());
  transformToStart = inputFrame * G4Transform3D(
    G4RotationMatrix(), G4ThreeVector(0, 0, -injectionSafety));
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
					       BDSMaterials::Instance()->GetMaterial(
						 BDSGlobalConstants::Instance()->WorldMaterial()),
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

std::pair<G4double, G4double> BDSLinkOpaqueBox::FaceClearances(
  BDSAcceleratorComponent* componentIn,
  const BDSTiltOffset* tiltOffsetIn)
{
  BDSBeamline probe;
  probe.AddComponent(
    componentIn,
    new BDSTiltOffset(tiltOffsetIn->GetXOffset(),
                      tiltOffsetIn->GetYOffset(),
                      tiltOffsetIn->GetTilt()));
  const auto* first = probe.front();
  const auto* last  = probe.back();
  G4bool angledInput = false;
  G4bool angledOutput = false;
  for (const auto& native : probe)
    {
      const auto* child = native->GetAcceleratorComponent();
      angledInput |= AngledGeometryFace(
        child->GeometryInputFaceNormal(), G4ThreeVector(0, 0, -1));
      angledOutput |= AngledGeometryFace(
        child->GeometryOutputFaceNormal(), G4ThreeVector(0, 0, 1));
    }
  if (!angledInput && !angledOutput)
    {return {0, 0};}

  const G4Transform3D inputToGlobal(
    *first->GetReferenceRotationStart(), first->GetReferencePositionStart());
  const G4Transform3D outputToGlobal(
    *last->GetReferenceRotationEnd(), last->GetReferencePositionEnd());
  const G4Transform3D globalToInput = inputToGlobal.inverse();
  const G4Transform3D globalToOutput = outputToGlobal.inverse();
  G4double minimumInputZ = std::numeric_limits<G4double>::max();
  G4double maximumOutputZ = std::numeric_limits<G4double>::lowest();
  G4double inputFaceRadius = 0;
  G4double outputFaceRadius = 0;
  for (const auto& native : probe)
    {
      const G4Transform3D localToGlobal(
        *native->GetRotationMiddle(), native->GetPositionMiddle());
      for (const auto& corner : native->GetExtent().AllBoundaryPoints())
        {
          const auto global = localToGlobal *
            (HepGeom::Point3D<G4double>)corner;
          const auto inInput = globalToInput * global;
          const auto inOutput = globalToOutput * global;
          minimumInputZ = std::min(minimumInputZ, inInput.z());
          maximumOutputZ = std::max(maximumOutputZ, inOutput.z());
        }
    }
  auto transverseRadiusInFrame = [](const BDSBeamlineElement* native,
                                    const G4Transform3D&      globalToFrame)
  {
    const G4Transform3D localToGlobal(
      *native->GetRotationMiddle(), native->GetPositionMiddle());
    G4double result = 0;
    for (const auto& corner : native->GetExtent().AllBoundaryPoints())
      {
        const auto inFrame = globalToFrame * localToGlobal *
          (HepGeom::Point3D<G4double>)corner;
        result = std::max(result, std::hypot(inFrame.x(), inFrame.y()));
      }
    return result;
  };
  inputFaceRadius = transverseRadiusInFrame(first, globalToInput);
  outputFaceRadius = transverseRadiusInFrame(last, globalToOutput);

  auto faceExcursion = [](const BDSBeamlineElement* native,
                          const G4Transform3D&      globalToFrame,
                          const G4ThreeVector&      localNormal,
                          G4double                  transverseRadius)
  {
    const G4ThreeVector normal = globalToFrame.getRotation() *
      (*native->GetRotationMiddle() * localNormal);
    if (std::abs(normal.z()) <= std::numeric_limits<G4double>::epsilon())
      {return std::numeric_limits<G4double>::infinity();}
    return transverseRadius * normal.perp() / std::abs(normal.z());
  };
  const G4double margin = 1*CLHEP::cm;
  const G4double input = angledInput ?
    std::max(std::max(0.0, -minimumInputZ),
             faceExcursion(first,
                           globalToInput,
                           first->GetAcceleratorComponent()->GeometryInputFaceNormal(),
                           inputFaceRadius)) + margin : 0.0;
  const G4double output = angledOutput ?
    std::max(std::max(0.0, maximumOutputZ),
             faceExcursion(last,
                           globalToOutput,
                           last->GetAcceleratorComponent()->GeometryOutputFaceNormal(),
                           outputFaceRadius)) + margin : 0.0;
  return {input, output};
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

void BDSLinkOpaqueBox::AppendFieldReferenceElements(
  BDSBeamline* target,
  const G4Transform3D& opaqueToGlobal,
  G4double& referenceS,
  G4int& referenceIndex) const
{
  for (G4int i = nominalStartIndex; i <= nominalEndIndex; ++i)
    {
      const BDSBeamlineElement* native = componentBeamline->at(i);
      auto frame = [this, &opaqueToGlobal](G4RotationMatrix* rotation,
                                           const G4ThreeVector& position)
      {
        return opaqueToGlobal * nativeToOpaque *
          G4Transform3D(*rotation, position);
      };
      const G4Transform3D placementStart =
        frame(native->GetRotationStart(), native->GetPositionStart());
      const G4Transform3D placementMiddle =
        frame(native->GetRotationMiddle(), native->GetPositionMiddle());
      const G4Transform3D placementEnd =
        frame(native->GetRotationEnd(), native->GetPositionEnd());
      const G4Transform3D referenceStart =
        frame(native->GetReferenceRotationStart(), native->GetReferencePositionStart());
      const G4Transform3D referenceMiddle =
        frame(native->GetReferenceRotationMiddle(), native->GetReferencePositionMiddle());
      const G4Transform3D referenceEnd =
        frame(native->GetReferenceRotationEnd(), native->GetReferencePositionEnd());
      const G4double arcLength = native->GetArcLength();
      const BDSTiltOffset* nativeTilt = native->GetTiltOffset();
      BDSTiltOffset* tiltCopy = nativeTilt ?
        new BDSTiltOffset(nativeTilt->GetXOffset(),
                          nativeTilt->GetYOffset(),
                          nativeTilt->GetTilt()) : nullptr;
      target->AddBeamlineElement(new BDSBeamlineElement(
        native->GetAcceleratorComponent(),
        placementStart.getTranslation(),
        placementMiddle.getTranslation(),
        placementEnd.getTranslation(),
        new G4RotationMatrix(placementStart.getRotation()),
        new G4RotationMatrix(placementMiddle.getRotation()),
        new G4RotationMatrix(placementEnd.getRotation()),
        referenceStart.getTranslation(),
        referenceMiddle.getTranslation(),
        referenceEnd.getTranslation(),
        new G4RotationMatrix(referenceStart.getRotation()),
        new G4RotationMatrix(referenceMiddle.getRotation()),
        new G4RotationMatrix(referenceEnd.getRotation()),
        referenceS,
        referenceS + 0.5*arcLength,
        referenceS + arcLength,
        0, 0, 0,
        tiltCopy,
        nullptr,
        referenceIndex));
      referenceS += arcLength;
      referenceIndex++;
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
