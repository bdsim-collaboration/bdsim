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
#include "BDSAcceleratorModel.hh"
#include "BDSApertureInfo.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSDetectorConstruction.hh"
#include "BDSExtent.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSOutput.hh"
#include "BDSParallelWorldSampler.hh"
#include "BDSParser.hh"
#include "BDSSampler.hh"
#include "BDSSamplerCustom.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSSamplerInfo.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSSamplerSphere.hh"
#include "BDSSamplerType.hh"
#include "BDSSDSampler.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "parser/samplerplacement.h"

#include "globals.hh" // geant4 types / globals
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VUserParallelWorld.hh"

#include <map>
#include <set>
#include <vector>


BDSParallelWorldSampler::BDSParallelWorldSampler(const G4String& name):
  G4VUserParallelWorld("SamplerWorld_" + name),
  suffix(name),
  samplerWorldVis(nullptr),
  generalPlane(nullptr),
  samplerWorldLV(nullptr)
{;}

BDSParallelWorldSampler::~BDSParallelWorldSampler()
{
  // we leak all the placements as g4 is incredibly slow to delete
  // them as it deregisters them - g4 will clean up
  //for (auto placement : placements)
  //  {delete placement;}
  delete samplerWorldVis;
  // samplers are deleted by the sampler registry
}

void BDSParallelWorldSampler::Construct()
{
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  G4VPhysicalVolume* samplerWorld = GetWorld();
  samplerWorldLV = samplerWorld->GetLogicalVolume();

  samplerWorldVis = new G4VisAttributes(*(globals->VisibleDebugVisAttr()));
  samplerWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  samplerWorldLV->SetVisAttributes(samplerWorldVis);
  
  const G4double samplerRadius = 0.5*globals->SamplerDiameter();
  const BDSBeamline* beamline  = BDSAcceleratorModel::Instance()->BeamlineMain();
  const G4bool checkOverlaps   = globals->CheckOverlaps();

  // Construct the one sampler typically used for a general sampler.
  generalPlane = new BDSSamplerPlane("Plane_sampler", samplerRadius);
  samplerInstances[-1] = generalPlane;
  // Construct any others we need.
  std::map<int, std::set<int>> filterIDToSet = BDSParser::Instance()->GetSamplerFilterIDToSet();
  for (const auto& kv : filterIDToSet)
    {
      BDSSamplerPlane* samp = new BDSSamplerPlane("Plane_sampler_w_filter_set_" + std::to_string(kv.first),
                                                  samplerRadius,
                                                  (G4int)kv.first);
      samplerInstances[(G4int)kv.first] = samp;
    }

  // For each element in the beamline (if a beamline has been defined) construct and place the
  // appropriate type of sampler if required. Info encoded in BDSBeamlineElement instance
  if (beamline != nullptr)
    {
      for (auto element : *beamline)
	{Place(element, samplerRadius);}
    }

  // Now user customised samplers
  std::vector<GMAD::SamplerPlacement> samplerPlacements = BDSParser::Instance()->GetSamplerPlacements();

  for (const auto& samplerPlacement : samplerPlacements)
    {
      G4String sn = samplerPlacement.name;
      G4cout << "User placed sampler: \"" << sn << "\"" << G4endl;
      
      if (BDSOutput::InvalidSamplerName(samplerPlacement.name))
	{
	  G4cerr << __METHOD_NAME__ << "invalid sampler name \"" << sn << "\"" << G4endl;
	  BDSOutput::PrintProtectedNames(G4cerr);
	  throw BDSException(__METHOD_NAME__, "");
	}
      
      // use main beamline - in future, multiple beam lines
      G4Transform3D transform = BDSDetectorConstruction::CreatePlacementTransform(samplerPlacement, beamline);
  
      BDSSamplerType st = BDS::DetermineSamplerType(samplerPlacement.samplerType);
      AdjustTransform(transform, st); // for 'forward' samplers we add an extra rotation
      G4double radius = 0;
      BDSSampler* sampler = BuildSampler(samplerPlacement, st, radius);
      G4int samplerID = BDSSamplerRegistry::Instance()->RegisterSampler(sn,
									sampler,
									transform,
									-1000,
									nullptr,
									st,
									radius);
      
      G4String uniqueName = BDSSamplerRegistry::Instance()->GetNameUnique(samplerID);
      if (uniqueName != sn)
	{BDS::Warning("Sampler placement with name \"" + sn + "\" will be named \"" + uniqueName + "\" in the output");}

      G4PVPlacement* pl = new G4PVPlacement(transform,
					    sampler->GetContainerLogicalVolume(),
					    sn + "_pv",
					    samplerWorldLV,
					    false,
					    samplerID,
					    checkOverlaps);
      placements.push_back(pl);
    } 

  // Link samplers are known while the mass world is being constructed, before
  // this parallel world's logical volume exists. Materialise the already
  // registered placements now, at their exact nominal reference planes.
  for (const auto& deferred : deferredPlacements)
    {
      G4PVPlacement* pl = new G4PVPlacement(deferred.transform,
                                            deferred.sampler->GetContainerLogicalVolume(),
                                            deferred.name + "_pv",
                                            samplerWorldLV,
                                            false,
                                            deferred.samplerID,
                                            checkOverlaps);
      placements.push_back(pl);
    }

}

G4int BDSParallelWorldSampler::RegisterSamplerForConstruction(
  const G4String&      name,
  BDSSampler*          sampler,
  const G4Transform3D& transform)
{
  G4int samplerID = BDSSamplerRegistry::Instance()->RegisterSampler(
    name, sampler, transform);
  if (samplerWorldLV)
    {
      // Link elements are normally added lazily after RunManager::Initialize,
      // so the parallel world already exists. Place their sampler now. During
      // initial mass-world construction the logical volume is not available
      // yet and the same placement is deferred until Construct().
      G4PVPlacement* placement = new G4PVPlacement(
        transform,
        sampler->GetContainerLogicalVolume(),
        name + "_pv",
        samplerWorldLV,
        false,
        samplerID,
        BDSGlobalConstants::Instance()->CheckOverlaps());
      placements.push_back(placement);
    }
  else
    {deferredPlacements.push_back({name, sampler, transform, samplerID});}
  return samplerID;
}

void BDSParallelWorldSampler::ErrorNonPositive(G4double value,
                                               const G4String& variableName,
                                               const G4String& objectName) const
{
  if (value <= 0)
  {throw BDSException("\"" + variableName + "\" is <= 0 and must be > 0 in samplerplacement \"" + objectName + "\"");}
}

BDSSampler* BDSParallelWorldSampler::BuildSampler(const GMAD::SamplerPlacement& samplerPlacement,
                                                  BDSSamplerType st,
                                                  G4double& radius) const
{
  BDSSampler* result = nullptr;
  G4String samplerName = G4String(samplerPlacement.name);
  
  switch (st.underlying())
    {
    case BDSSamplerType::plane:
      {
	BDSApertureInfo* shape;
	if (samplerPlacement.apertureModel.empty())
	  {
	    shape = new BDSApertureInfo(samplerPlacement.shape,
					samplerPlacement.aper1 * CLHEP::m,
					samplerPlacement.aper2 * CLHEP::m,
					samplerPlacement.aper3 * CLHEP::m,
					samplerPlacement.aper4 * CLHEP::m,
					samplerName);
	  }
	else
	  {shape = BDSAcceleratorModel::Instance()->Aperture(samplerPlacement.apertureModel);}
	
	result = new BDSSamplerCustom(samplerName, *shape, samplerPlacement.partIDSetID);
	break;
      }
    case BDSSamplerType::cylinder:
      {
        ErrorNonPositive(samplerPlacement.aper1, "aper1", samplerName);
        ErrorNonPositive(samplerPlacement.aper2, "aper2", samplerName);
        G4double startAnglePhi = samplerPlacement.startAnglePhi * CLHEP::rad;
        G4double sweepAnglePhi = samplerPlacement.sweepAnglePhi * CLHEP::rad;
        if (sweepAnglePhi <= 0) // default in parser is -1 to flag we should use 2pi now we have units
          {sweepAnglePhi = CLHEP::twopi;}
        else if (sweepAnglePhi > CLHEP::twopi + 1e-6)
	  {throw BDSException(__METHOD_NAME__, "\"sweepAnglePhi\" must be in range (0 to pi] in samplerplacement \"" + samplerName + "\"");}
   
	result = new BDSSamplerCylinder(samplerName,
					samplerPlacement.aper1 * CLHEP::m,
					2 * samplerPlacement.aper2 * CLHEP::m,
					startAnglePhi,
					sweepAnglePhi,
					samplerPlacement.partIDSetID);
        radius = samplerPlacement.aper1 * CLHEP::m;
	break;
      }
    case BDSSamplerType::cylinderforward:
      {
        ErrorNonPositive(samplerPlacement.aper1, "aper1", samplerName);
        ErrorNonPositive(samplerPlacement.aper2, "aper2", samplerName);
        if (BDS::IsFinite(samplerPlacement.startAnglePhi))
          {BDS::Warning("\"startAnglePhi\" in samplerplacement \""+samplerName+"\" != 0 -> this has no effect for a cylinderforward sampler");}
        
        G4double sweepAnglePhi = samplerPlacement.sweepAnglePhi * CLHEP::rad;
        if (sweepAnglePhi <= 0) // default in parser is -1 to flag we should use 2pi now we have units
          {sweepAnglePhi = CLHEP::twopi;}
        else if (sweepAnglePhi > CLHEP::twopi + 1e-6)
          {throw BDSException(__METHOD_NAME__, "\"sweepAnglePhi\" must be in range (0 to pi] in samplerplacement \"" + samplerName + "\"");}
        G4double startAnglePhi = -0.5*sweepAnglePhi;
        
        result = new BDSSamplerCylinder(samplerName,
                                        samplerPlacement.aper1 * CLHEP::m,
                                        2 * samplerPlacement.aper2 * CLHEP::m,
                                        startAnglePhi,
                                        sweepAnglePhi,
                                        samplerPlacement.partIDSetID);
        radius = samplerPlacement.aper1 * CLHEP::m;
        break;
      }
    case BDSSamplerType::sphere:
      {
        ErrorNonPositive(samplerPlacement.aper1, "aper1", samplerName);
        G4double startAnglePhi = samplerPlacement.startAnglePhi * CLHEP::rad;
        G4double sweepAnglePhi = samplerPlacement.sweepAnglePhi * CLHEP::rad;
        if (sweepAnglePhi <= 0) // default in parser is -1 to flag we should use 2pi now we have units
          {sweepAnglePhi = CLHEP::twopi;}
        else if (sweepAnglePhi > CLHEP::twopi + 1e-6)
          {throw BDSException(__METHOD_NAME__, "\"sweepAnglePhi\" must be in range (0 to 2 pi] in samplerplacement \"" + samplerName + "\"");}
        G4double startAngleTheta = samplerPlacement.startAngleTheta * CLHEP::rad;
	if (startAngleTheta < 0)
	  {throw BDSException(__METHOD_NAME__, "\"startAngleTheta\" must be in the range [0 to pi] in samplerplacement \"" + samplerName + "\"");}
        G4double sweepAngleTheta = samplerPlacement.sweepAngleTheta * CLHEP::rad;
        if (sweepAngleTheta <= 0) // default in parser is -1 to flag we should use 2pi now we have units
          {sweepAngleTheta = CLHEP::pi;}
        else if (sweepAngleTheta > CLHEP::pi + 1e-6)
          {throw BDSException(__METHOD_NAME__, "\"sweepAngleTheta\" must be in range (0 to pi] in samplerplacement \"" + samplerName + "\"");}
	else if (sweepAngleTheta - startAngleTheta > CLHEP::pi + 1e-6)
          {throw BDSException(__METHOD_NAME__, "\"startAngleTheta\" + \"sweepAngleTheta\" must be in range (0 to pi] in samplerplacement \"" + samplerName + "\"");}
        
        result = new BDSSamplerSphere(samplerName,
				      samplerPlacement.aper1 * CLHEP::m,
				      startAnglePhi,
				      sweepAnglePhi,
				      startAngleTheta,
				      sweepAngleTheta,
				      samplerPlacement.partIDSetID);
        radius = samplerPlacement.aper1 * CLHEP::m;
	break;
      }
    case BDSSamplerType::sphereforward:
      {
        ErrorNonPositive(samplerPlacement.aper1, "aper1", samplerName);
        if (BDS::IsFinite(samplerPlacement.startAnglePhi))
          {BDS::Warning("\"startAnglePhi\" in samplerplacement \""+samplerName+"\" != 0 -> this has no effect for a sphereforward sampler");}
        if (BDS::IsFinite(samplerPlacement.startAngleTheta))
          {BDS::Warning("\"startAngleTheta\" in samplerplacement \""+samplerName+"\" != 0 -> this has no effect for a sphereforward sampler");}
        
        G4double sweepAnglePhi = samplerPlacement.sweepAnglePhi * CLHEP::rad;
        if (sweepAnglePhi <= 0) // default in parser is -1 to flag we should use 2pi now we have units
          {sweepAnglePhi = CLHEP::twopi;}
        else if (sweepAnglePhi > CLHEP::twopi + 1e-6)
          {throw BDSException(__METHOD_NAME__, "\"sweepAnglePhi\" must be in range (0 to 2 pi] in samplerplacement \"" + samplerName + "\"");}
        G4double sweepAngleTheta = samplerPlacement.sweepAngleTheta * CLHEP::rad;
        if (sweepAngleTheta <= 0) // default in parser is -1 to flag we should use 2pi now we have units
          {sweepAngleTheta = CLHEP::pi;}
        else if (sweepAngleTheta > CLHEP::pi + 1e-6)
          {throw BDSException(__METHOD_NAME__, "\"sweepAngleTheta\" must be in range (0 to pi] in samplerplacement \"" + samplerName + "\"");}
  
        G4double startAngleTheta = CLHEP::halfpi -0.5*sweepAngleTheta;
        G4double startAnglePhi   = -0.5*sweepAnglePhi;
        
        result = new BDSSamplerSphere(samplerName,
                                      samplerPlacement.aper1 * CLHEP::m,
                                      startAnglePhi,
                                      sweepAnglePhi,
                                      startAngleTheta,
                                      sweepAngleTheta,
                                      samplerPlacement.partIDSetID);
        radius = samplerPlacement.aper1 * CLHEP::m;
        break;
      }
    }
  return result;
}

void BDSParallelWorldSampler::AdjustTransform(G4Transform3D& trans,
                                              BDSSamplerType st) const
{
  switch (st.underlying())
    {
    case BDSSamplerType::cylinderforward:
      {
	auto rmExisting = trans.getRotation();
	G4RotationMatrix rm;
	rm.rotate(-CLHEP::halfpi, {0,0,1});
	rm.rotate(-CLHEP::halfpi, {1,0,0});
	rmExisting *= rm;
	trans = G4Transform3D(rmExisting, trans.getTranslation());
	break;
      }
    case BDSSamplerType::sphereforward:
      {
	auto rmExisting = trans.getRotation();
	G4RotationMatrix rm;
	rm.rotate(-CLHEP::halfpi, {0,1,0});
	rmExisting *= rm;
	trans = G4Transform3D(rmExisting, trans.getTranslation());
	break;
      }
    default:
      {break;}
    }
}

void BDSParallelWorldSampler::Place(const BDSBeamlineElement* element,
				    G4double samplerRadius)
{
  BDSSamplerInfo* samplerInfo = element->GetSamplerInfo();
  if (!samplerInfo)
    {return;}
  BDSSamplerType samplerType = samplerInfo->samplerType;
  if (samplerType == BDSSamplerType::none)
    {return;}
  // else must be a valid sampler
  
  G4String name = samplerInfo->name;
  G4double sEnd = element->GetSPositionEnd();
  
  BDSSampler* sampler = nullptr;
  switch (samplerType.underlying())
    {
    case BDSSamplerType::plane:
      {sampler = samplerInstances[samplerInfo->pdgSetID]; break;}
    case BDSSamplerType::cylinder:
      {// these are built uniquely for each instance so the length matches exactly
        // for a cylindrical sampler we make it 'fit' the component
        G4double boundingRadius = element->GetExtent().TransverseBoundingRadius();
        if (boundingRadius > 0) // protect against bad extent, i.e. 0
          {samplerRadius = boundingRadius;}
        
        G4double length = element->GetChordLength();
        G4double angle  = element->GetAngle();
        if (samplerInfo->startElement && samplerInfo->finishElement)
	  {// to cover a range of split components, e.g. an sbend
	    const auto startElement = samplerInfo->startElement;
	    const auto finishElement = samplerInfo->finishElement;
	    G4ThreeVector connectingVector = finishElement->GetReferencePositionEnd() - startElement->GetReferencePositionStart();
	    G4double chordLength = connectingVector.mag();
     
	    G4ThreeVector directionFrameStart = G4ThreeVector(0,0,1).transform(*(startElement->GetReferenceRotationStart()));
	    G4ThreeVector directionFrameFinish = G4ThreeVector(0,0,1).transform(*(finishElement->GetReferenceRotationEnd()));
	    
	    G4ThreeVector dChordFrameStart = connectingVector.unit() - directionFrameStart;
	    G4ThreeVector dChordFrameFinish = connectingVector.unit() - directionFrameFinish;
	    
      // we purposively ignore possible pole face rotations or a different input normal
      // this is convention for cylindrical samplers where we do the same for drifts
	    G4ThreeVector ipfnCSampler = G4ThreeVector(0,0,-1) + dChordFrameStart;
	    G4ThreeVector opfnCSampler = G4ThreeVector(0,0,1) - dChordFrameFinish;
	    
	    sampler = new BDSSamplerCylinder(name,
					     samplerRadius,
					     chordLength,
					     ipfnCSampler,
					     opfnCSampler,
					     0, CLHEP::twopi,
					     samplerInfo->pdgSetID);
	    
	  }
        else if (BDS::IsFinite(angle))
	  {
	    const auto ipfn = element->GetAcceleratorComponent()->GeometryInputFaceNormal();
	    const auto opfn = element->GetAcceleratorComponent()->GeometryOutputFaceNormal();
	    sampler = new BDSSamplerCylinder(name,
					     samplerRadius,
					     length,
					     ipfn,
					     opfn,
					     0, CLHEP::twopi,
					     samplerInfo->pdgSetID);
	  }
        else
	  {
	    sampler = new BDSSamplerCylinder(name,
					     samplerRadius,
					     length,
					     0, CLHEP::twopi,
					     samplerInfo->pdgSetID);
	  }
        break;
      }
    default: // no spherical samplers for beam line samplers
      {break;} // leave as nullptr - shouldn't occur due to if at top
    }
  
  if (sampler)
    {
      G4Transform3D* pt = new G4Transform3D(*element->GetSamplerPlacementTransform());
      
      G4int samplerID = BDSSamplerRegistry::Instance()->RegisterSampler(name,
									sampler,
									*pt,
									sEnd,
									element,
									samplerType,
									samplerRadius);

      G4VPhysicalVolume* samplerWorld   = GetWorld();
      samplerWorldLV = samplerWorld->GetLogicalVolume();
      const G4bool checkOverlaps = BDSGlobalConstants::Instance()->CheckOverlaps();
      // record placements for cleaning up at destruction.
      G4PVPlacement* pl = new G4PVPlacement(*pt,              // placement transform
					    sampler->GetContainerLogicalVolume(), // logical volume
					    name + "_pv",     // name of placement
					    samplerWorldLV,   // mother volume
					    false,            // no boolean operation
					    samplerID,        // copy number
					    checkOverlaps);

      placements.push_back(pl);
    }
}
