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
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSBeamlineIntegral.hh"
#include "BDSBeamlinePlacementBuilder.hh"
#include "BDSComponentFactory.hh"
#include "BDSDebug.hh"
#include "BDSDetectorConstruction.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSFieldFactory.hh"
#include "BDSFieldInfo.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSParser.hh"
#include "BDSPlacementToMake.hh"
#include "BDSSimpleComponent.hh"
#include "BDSUtilities.hh"

#include "parser/element.h"
#include "parser/placement.h"

#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4Types.hh"
#include "G4VSolid.hh"

#include <vector>


BDSBeamline* BDS::BuildPlacementGeometry(const std::vector<GMAD::Placement>& placements,
                                         const BDSBeamline* parentBeamLine,
                                         BDSComponentFactory* componentFactory,
                                         const BDSParticleDefinition* designParticle)
{
  if (placements.empty())
    {return nullptr;} // don't do anything - no placements
  
  BDSBeamline* placementBL = new BDSBeamline();
  std::vector<BDSPlacementToMake> fieldPlacements;
  
  BDSBeamlineIntegral startingIntegral(*designParticle);
  BDSBeamlineIntegral* integral = new BDSBeamlineIntegral(startingIntegral);

  for (const auto& placement : placements)
    {
      // if a sequence is specified, it's a beam line and will be constructed
      // elsewhere - skip it!
      if (!placement.sequence.empty())
        {continue;}
      
      BDSAcceleratorComponent* comp;
      G4bool hasAField = false;
      G4String fieldPlacementName;
      G4double chordLength;
      
      if (placement.name.empty())
        {
          G4cerr << "Problem with unnamed placement, its contents are:" << G4endl;
          placement.print();
          throw BDSException(__METHOD_NAME__, "a placement must be defined with a name");
        }
      
      G4bool elementSpecified  = !placement.bdsimElement.empty();
      G4bool geometrySpecified = !placement.geometryFile.empty();
      if (elementSpecified && geometrySpecified)
        {
          G4String msg = "only one of \"geometryFile\" or \"bdsimElement\" can be specified in placement \"" + placement.name + "\"";
          throw BDSException(__METHOD_NAME__, msg);
        }
      else if (!elementSpecified && !geometrySpecified)
        {
          G4String msg = "at least one of \"geometryFile\" or \"bdsimElement\" must be specified in placement \"" + placement.name + "\"";
          throw BDSException(__METHOD_NAME__, msg);
        }
      
      if (geometrySpecified)
        {// it's a geometryFile + optional field map placement
          hasAField = !placement.fieldAll.empty();
          BDSFieldInfo* fieldRecipe = nullptr;
          if (hasAField)
            {
              fieldRecipe = new BDSFieldInfo(*(BDSFieldFactory::Instance()->GetDefinition(placement.fieldAll)));
              fieldRecipe->SetUsePlacementWorldTransform(true);
            }
          
          auto geom = BDSGeometryFactory::Instance()->BuildGeometry(placement.name,
                                                                    placement.geometryFile,
                                                                    nullptr,
                                                                    placement.autoColour,
                                                                    0,
                                                                    0,
                                                                    nullptr,
                                                                    placement.sensitive,
                                                                    BDSSDType::energydep,
                                                                    BDSSDType::energydepvacuum,
                                                                    placement.stripOuterVolume,
                                                                    nullptr,
                                                                    placement.dontReloadGeometry);
          
          chordLength = geom->GetExtent().DZ();
          comp = new BDSSimpleComponent(placement.name + "_" + geom->GetName(), geom, chordLength);
      
          if (hasAField)
            {
              comp->SetField(fieldRecipe);
              fieldPlacementName = comp->GetName() + "_" + fieldRecipe->NameOfParserDefinition();
            }
        }
      else
        {// it's a bdsim-built component
          const GMAD::Element* element = BDSParser::Instance()->GetPlacementElement(placement.bdsimElement);
          if (!element)
            {throw BDSException(__METHOD_NAME__, "no such element definition by name \"" + placement.bdsimElement + "\" found for placement.");}
          comp = componentFactory->CreateComponent(element, nullptr, nullptr, *integral); // note no current arc length for RF time offset
          hasAField = comp->HasAField();
          if (hasAField)
            {comp->SetFieldUsePlacementWorldTransform();}
          fieldPlacementName = comp->GetName() + "_field";
          chordLength = comp->GetChordLength();
        }
      
      comp->Initialise();
      
      G4Transform3D transform = BDSDetectorConstruction::CreatePlacementTransform(placement, parentBeamLine);
      
      /// Here we're assuming the length is along z which may not be true, but
      /// close enough for this purpose as we rely only on the centre position.
      G4ThreeVector halfLengthBeg = G4ThreeVector(0,0,-chordLength*0.5);
      G4ThreeVector halfLengthEnd = G4ThreeVector(0,0, chordLength*0.5);
      G4ThreeVector midPos        = transform.getTranslation();
      G4ThreeVector startPos = midPos + transform * (HepGeom::Point3D<G4double>)halfLengthBeg;
      G4ThreeVector endPos   = midPos + transform * (HepGeom::Point3D<G4double>)halfLengthEnd;
      G4RotationMatrix* rm   = new G4RotationMatrix(transform.getRotation());
      
      BDSBeamlineElement* el = new BDSBeamlineElement(comp,
                                                      startPos,
                                                      midPos,
                                                      endPos,
                                                      rm,
                                                      new G4RotationMatrix(*rm),
                                                      new G4RotationMatrix(*rm),
                                                      startPos,
                                                      midPos,
                                                      endPos,
                                                      new G4RotationMatrix(*rm),
                                                      new G4RotationMatrix(*rm),
                                                      new G4RotationMatrix(*rm),
                                                      -1,-1,-1, 0);

      placementBL->AddBeamlineElement(el);
  
      if (hasAField)
        {
          G4VSolid* containerSolidClone = comp->GetContainerSolid()->Clone();
          G4LogicalVolume* lv = new G4LogicalVolume(containerSolidClone, nullptr, fieldPlacementName+"_lv");
          fieldPlacements.emplace_back(BDSPlacementToMake(transform, lv, fieldPlacementName+"_pv"));
        }
    }
  
  BDSAcceleratorModel::Instance()->RegisterPlacementFieldPlacements(fieldPlacements);
  
  delete integral;
  
  return placementBL;
}
