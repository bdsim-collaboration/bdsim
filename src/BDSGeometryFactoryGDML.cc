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
#ifdef USE_GDML
#include "BDSAcceleratorModel.hh"
#include "BDSColourFromMaterial.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactoryGDML.hh"
#include "BDSGeometryInspector.hh"
#include "BDSGeometryWriter.hh" // for BDSGeometryWriter::auxType
#include "BDSGDMLPreprocessor.hh"  // also only available with USE_GDML
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSWarning.hh"

#include "globals.hh"
#include "G4Colour.hh"
#include "G4GDMLParser.hh"
#include "G4LogicalVolume.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include <array>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

class G4VisAttributes;
class G4VSolid;


BDSGeometryFactoryGDML::BDSGeometryFactoryGDML()
{;}

BDSGeometryExternal* BDSGeometryFactoryGDML::Build(G4String               componentName,
                                                   G4String               fileName,
                                                   std::map<G4String, G4Colour*>* mapping,
                                                   G4bool                 autoColour,
                                                   G4double             /*suggestedLength*/,
                                                   G4double             /*suggestedHorizontalWidth*/,
                                                   std::vector<G4String>* namedVacuumVolumes,
                                                   G4bool                 makeSensitive,
                                                   BDSSDType              sensitivityType,
                                                   BDSSDType              vacuumSensitivityType,
                                                   G4UserLimits*          userLimitsToAttachToAllLVs,
                                                   G4bool                 detectSchema)
{
  CleanUp();

  // Compensate for G4GDMLParser deficiency in loading more than one file with similar names
  // in objects. Prepend all names with component name.
  G4String processedFile;
  G4bool preprocessGDML       = BDSGlobalConstants::Instance()->PreprocessGDML();
  G4bool preprocessGDMLSchema = BDSGlobalConstants::Instance()->PreprocessGDMLSchema();
  if (preprocessGDML)
    {processedFile = BDS::PreprocessGDML(fileName, componentName, preprocessGDMLSchema, detectSchema);} // use all in one method
  else if (preprocessGDMLSchema) // generally don't process the file but process the schema to local copy only
    {processedFile = BDS::PreprocessGDMLSchemaOnly(fileName, detectSchema);} // use schema only method
  else // no processing
    {processedFile = fileName;}
  G4String preprocessNameToStrip = preprocessGDML ? componentName+"_" : "";
  
  G4GDMLParser* parser = new G4GDMLParser();
  parser->SetOverlapCheck(BDSGlobalConstants::Instance()->CheckOverlaps());
  parser->Read(processedFile, /*validate=*/true);

  G4VPhysicalVolume* containerPV = parser->GetWorldVolume();
  G4LogicalVolume*   containerLV = containerPV->GetLogicalVolume();
  G4VSolid*       containerSolid = containerLV->GetSolid();
  G4ThreeVector gdmlWorldOrigin = G4ThreeVector();
  if (containerPV->GetName() == "world_volume_lv_PV")
    {
      gdmlWorldOrigin = parser->GetPosition("PygdmlOrigin"); // TODO check if Pygdml geometry
      gdmlWorldOrigin[2] = 0.0;
    }

  // record all pvs and lvs used in this loaded geometry
  std::set<G4VPhysicalVolume*> pvsGDML;
  std::set<G4LogicalVolume*>   lvsGDML;
  std::map<G4String, G4Material*> materialsGDML;
  GetAllLogicalPhysicalAndMaterials(containerPV, pvsGDML, lvsGDML, materialsGDML);
  BDSMaterials::Instance()->CheckForConflictingMaterialsAfterLoad(fileName, componentName);
  BDSMaterials::Instance()->CacheMaterialsFromGDML(materialsGDML);

  std::map<G4String, G4Colour*> gdmlColours = BuildColourMap(lvsGDML, parser, componentName);
  if (mapping)
    {gdmlColours.insert(mapping->begin(), mapping->end());}

  G4cout << "Loaded GDML file \"" << processedFile << "\" containing:" << G4endl;
  G4cout << pvsGDML.size() << " physical volumes, and " << lvsGDML.size() << " logical volumes" << G4endl;

  auto visesGDML = ApplyColourMapping(lvsGDML, gdmlColours, autoColour, preprocessNameToStrip);
  
  G4UserLimits* ul = userLimitsToAttachToAllLVs ? userLimitsToAttachToAllLVs : BDSGlobalConstants::Instance()->DefaultUserLimits();
  ApplyUserLimits(lvsGDML, ul);
  
  // make sure container is visible - Geant4 always makes the container invisible.
  G4Colour* c = BDSColourFromMaterial::Instance()->GetColour(containerLV->GetMaterial(), preprocessNameToStrip);
  G4VisAttributes* vis = new G4VisAttributes(*c);
  vis->SetVisibility(true);
  visesGDML.insert(vis);
  containerLV->SetVisAttributes(vis);

  std::pair<BDSExtent, BDSExtent> outerInner = BDS::DetermineExtents(containerSolid);
  
  BDSGeometryExternal* result = new BDSGeometryExternal(containerSolid, containerLV,
                                                        outerInner.first,  /*outer*/
                                                        outerInner.second, /*inner*/
                                                        gdmlWorldOrigin);
  result->RegisterLogicalVolume(lvsGDML);
  result->RegisterPhysicalVolume(pvsGDML);
  result->RegisterVisAttributes(visesGDML);
  result->RegisterVacuumVolumes(GetVolumes(lvsGDML, namedVacuumVolumes, preprocessGDML, componentName));

  if (makeSensitive)
    {
      const auto &vacuumVolumes = result->VacuumVolumes();
      ApplySensitivity(result,
                       result->GetAllLogicalVolumes(),
                       sensitivityType,
                       vacuumVolumes,
                       vacuumSensitivityType);
    }
  
  delete parser;
  return result;
}

std::map<G4String, G4Colour*> BDSGeometryFactoryGDML::BuildColourMap(const std::set<G4LogicalVolume*>& lvsGDML,
                                                                     G4GDMLParser* parser,
                                                                     const G4String& componentName)
{
  std::map<G4String, G4Colour*> result;
  G4int iColour = 0;
  for (auto lv : lvsGDML)
    {
      auto auxInfo = parser->GetVolumeAuxiliaryInformation(lv);
      for (const auto& af : auxInfo)
        {
          if (af.type == "colour")
            {
              std::stringstream ss(af.value);
              std::vector<G4String> colVals((std::istream_iterator<G4String>(ss)), std::istream_iterator<G4String>());
              if (colVals.size() != 4)
                {BDS::Warning(__METHOD_NAME__, "invalid number of colour values for logical volume " + lv->GetName());}
              G4String colourName = componentName + "_colour_"+std::to_string(iColour);
              iColour++;
              G4String colourString = colourName + ":";
              for (const auto& c : colVals)
                {colourString += " " + c;}
              // false = don't normalise to 255 as already done so
              G4Colour* colour = BDSColours::Instance()->GetColour(colourString, false);
              result[lv->GetName()] = colour;
           }
          else if (af.type == BDSGeometryWriter::auxType)
            {
              G4bool visible = false;
              std::array<G4double, 4> rgba = VRGBAStringAndUnitToColourAndStyle(af.value, visible);
              if (visible)
                {
                  G4Colour* colour = BDSColours::Instance()->GetColour(rgba[0], rgba[1], rgba[2], rgba[3]);
                  result[lv->GetName()] = colour;
                }
            }
        }
    }
  return result;
}

std::array<G4double, 4> BDSGeometryFactoryGDML::VRGBAStringAndUnitToColourAndStyle(const G4String& value,
                                                                                   G4bool& visible)
{
  std::array<G4double, 4> result{0,0,0,0};
  std::stringstream ss(value);
  try
  {
    ss >> visible;
    G4int i = 0;
    G4double v = 0;
    while (ss >> v)
      {
        if (i > 3)
          {break;}
        result[i] = v;
        i++;
      }
  }
  catch (std::exception& e)
    {throw BDSException(__METHOD_NAME__, "error in colour definition " + value);}
  return result;
}

G4String BDSGeometryFactoryGDML::PreprocessedName(const G4String& objectName,
                                                  const G4String& acceleratorComponentName) const
{return BDSGDMLPreprocessor::ProcessedNodeName(objectName, acceleratorComponentName);}

void BDSGeometryFactoryGDML::GetAllLogicalPhysicalAndMaterials(const G4VPhysicalVolume*         volume,
                                                               std::set<G4VPhysicalVolume*>&    pvsIn,
                                                               std::set<G4LogicalVolume*>&      lvsIn,
                                                               std::map<G4String, G4Material*>& materialsGDML)
{
  const auto& lv = volume->GetLogicalVolume();
  lvsIn.insert(lv);
  G4Material* mat = lv->GetMaterial();
  materialsGDML[mat->GetName()] = mat;
  for (G4int i = 0; i < (G4int)lv->GetNoDaughters(); i++)
    {
      const auto& pv = lv->GetDaughter(i);
      pvsIn.insert(pv);
      GetAllLogicalPhysicalAndMaterials(pv, pvsIn, lvsIn, materialsGDML); // recurse into daughter
    }
}

void BDSGeometryFactoryGDML::ReplaceStringInFile(const G4String& fileName,
                                                 const G4String& outputFileName,
                                                 const G4String& key,
                                                 const G4String& replacement)
{
  // open input file in read mode
  std::ifstream ifs(fileName);

  // verify file open.
  if (!ifs.is_open())
    {throw BDSException(__METHOD_NAME__, "Cannot open file \"" + fileName + "\"");}

  std::ofstream fout(outputFileName);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Original file:  " << fileName       << G4endl;
  G4cout << __METHOD_NAME__ << "Temporary file: " << outputFileName << G4endl;
#endif

  int lenOfKey = (int)key.size();
  
  // loop over and replace
  std::string buffer;
  while (std::getline(ifs, buffer))
    {// if we find key, replace it
      int f = (int)buffer.find(key);
      if (f != -1)
        {
          std::string outputString = std::string(buffer);
          outputString.replace(f, lenOfKey, replacement);
          fout << outputString << "\n"; // getline strips \n
        }
      else // copy line to temp file as is
        {fout << buffer << "\n";}
    }

  // clean up
  ifs.close();
  fout.close();
}

#else
// insert empty function to avoid no symbols warning
void _SymbolToPreventWarningGeomFacGDML(){;}
#endif
