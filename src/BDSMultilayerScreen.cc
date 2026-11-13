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
#include "BDSColourWheel.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSMaterials.hh"
#include "BDSMultilayerScreen.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSScreenLayer.hh"

#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4OpticalSurface.hh"
#include "G4PVPlacement.hh"               
#include "G4VSolid.hh"
#include "G4Version.hh"

BDSMultilayerScreen::BDSMultilayerScreen(const G4TwoVector& xysizeIn,
					 const G4String&    nameIn):
  xysize(xysizeIn),
  name(nameIn),
  log(nullptr),
  phys(nullptr),
  solid(nullptr),
  colourWheel(new BDSColourWheel())
{
  size.setX(xysize.x()); 
  size.setY(xysize.y());
  size.setZ(0); // starts as 0, but expands as layers added
}

BDSMultilayerScreen::~BDSMultilayerScreen()
{
  delete colourWheel;
  for (auto layer : screenLayers)
    {delete layer;}
}

void BDSMultilayerScreen::AddScreenLayer(G4double thickness,
					 G4String material,
					 G4String nameIn,
					 G4int    isSampler,
					 G4double grooveWidth,
					 G4double grooveSpatialFrequency)
{
  G4String layerName;
  if (isSampler)
    {
      G4int nThisSampler = BDSSamplerRegistry::Instance()->NumberOfExistingSamplers() + 1;
      G4String tempString = "Sampler_" + std::to_string(nThisSampler);
      layerName = tempString + "_" + nameIn;
    }
  else
    {layerName = nameIn;}

  G4ThreeVector layerSize(xysize.x(), xysize.y(), thickness);
  BDSScreenLayer* screen = new BDSScreenLayer(layerSize, layerName, material,
					      grooveWidth,grooveSpatialFrequency);
  AddScreenLayer(screen, isSampler);
}

void BDSMultilayerScreen::AddScreenLayer(BDSScreenLayer* layer, G4int isSampler)
{
  colourWheel->Spin();
  layer->SetColour(colourWheel->Colour());
  if (isSampler)
    {layer->AssignSampler();}
  screenLayers.push_back(layer);
  screenLayerNames[layer->GetName()] = layer;
}

BDSScreenLayer* BDSMultilayerScreen::ScreenLayer(const G4String& layerName)
{
  auto result = screenLayerNames.find(layerName);
  if (result == screenLayerNames.end())
    {throw BDSException(__METHOD_NAME__, "screen layer \"" + layerName + "\" not found");}
  else
    {return result->second;}
}

void BDSMultilayerScreen::Build()
{
  ComputeDimensions();
  BuildMotherVolume();
  PlaceLayers();
}

void BDSMultilayerScreen::Place(G4RotationMatrix* rot,
				const G4ThreeVector& pos,
				G4LogicalVolume* motherVol)
{
  SetPhys(new G4PVPlacement(rot,
			    pos,
			    log,
			    "multilayerScreen",
			    motherVol,
			    false,
			    0,
			    true
			    ));             
}

void BDSMultilayerScreen::ReflectiveSurface(G4int layer1, G4int layer2)
{
  G4OpticalSurface* OpSurface=new G4OpticalSurface("OpSurface");
  //  G4LogicalBorderSurface* LogSurface =
  new G4LogicalBorderSurface("LogSurface",
			     ScreenLayer(layer1)->GetPhys(),
			     ScreenLayer(layer2)->GetPhys(),
			     OpSurface);
  //  G4LogicalSkinSurface* LogSurface  = new G4LogicalSkinSurface("LogSurface",screenLayer(1)->GetLog(),OpSurface);
  OpSurface->SetType(dielectric_metal);
  OpSurface->SetModel(unified);
  OpSurface->SetFinish(polished);  
  
  G4MaterialPropertiesTable* SMPT = new G4MaterialPropertiesTable();
#if G4VERSION_NUMBER > 1109
  SMPT->AddConstProperty("REFLECTIVITY",0.8,true);
#else
  SMPT->AddConstProperty("REFLECTIVITY",0.8);
#endif
  
  /*
  OpSurface->SetSigmaAlpha(0.7);
  SMPT->AddConstProperty("SPECULARLOBECONSTANT",1.0);
  SMPT->AddConstProperty("SPECULARSPIKECONSTANT",0.0);
  SMPT->AddConstProperty("BACKSCATTERCONSTANT",0.0);
  SMPT->AddConstProperty("EFFICIENCY",0.0);
  //  SMPT->AddConstProperty("RINDEX",1.5750);
  */

  OpSurface->SetMaterialPropertiesTable(SMPT);
}

void BDSMultilayerScreen::RoughSurface(G4int layer1, G4int layer2)
{
  G4OpticalSurface* OpSurface=new G4OpticalSurface("OpSurface");
  OpSurface->SetType(dielectric_dielectric);
  OpSurface->SetFinish(ground);
  OpSurface->SetModel(unified);

  //  G4LogicalBorderSurface* LogSurface = 
  new G4LogicalBorderSurface("LogSurface",
			     ScreenLayer(layer1)->GetPhys(),
			     ScreenLayer(layer2)->GetPhys(),
			     OpSurface);

  G4double sigma_alpha=0.7;
  OpSurface->SetSigmaAlpha(sigma_alpha);
  G4MaterialPropertiesTable* SMPT = new G4MaterialPropertiesTable();
#if G4VERSION_NUMBER > 1109
  SMPT->AddConstProperty("REFLECTIVITY",0.0, true);
#else
  SMPT->AddConstProperty("REFLECTIVITY",0.0);
#endif
  OpSurface->SetMaterialPropertiesTable(SMPT);
}

void BDSMultilayerScreen::ComputeDimensions()
{
  if (screenLayers.empty())
    {throw BDSException(__METHOD_NAME__, "Screen \"" + name +"\" has no layers.");}

  //Compute the total z thickness.
  G4double temp = 0;
  for (const auto& layer : screenLayers)
    {temp += layer->GetSize().z();}
  size.setZ(temp);
  
  //Compute the z positions of all the layers.
  //Position each layer after the previous one.
  G4double pos = screenLayers[0]->GetSize().z()/2.0 -1.0*size.z()/2.0;
  screenLayerZPos.push_back(pos);
  for (unsigned int i=1; i<screenLayers.size(); i++)
    {
      pos += (screenLayers[i-1]->GetSize().z()+screenLayers[i]->GetSize().z())/2.0;
      screenLayerZPos.push_back(pos);
    }
}

void BDSMultilayerScreen::BuildMotherVolume()
{  
  // Make container marginally bigger to avoid overlaps
  G4double lengthSafety = BDSGlobalConstants::Instance()->LengthSafety();
  
  solid  = new G4Box((name+"_solid").c_str(),
		     size.x()/2.0 + lengthSafety,
		     size.y()/2.0 + lengthSafety,
		     size.z()/2.0 + lengthSafety);
  G4Material* mat = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->VacuumMaterial());
  
  log = new G4LogicalVolume(solid,
			    mat,
			    (name+"_log").c_str());
  
  G4VisAttributes* visAtt = new G4VisAttributes(G4Colour(0.0,0.0,1.0,0.3));
  visAtt->SetForceWireframe(true);
  log->SetVisAttributes(visAtt);
}

void BDSMultilayerScreen::PlaceLayers()
{
  G4ThreeVector pos(0,0,0);

  for (unsigned int i=0; i<screenLayers.size(); i++)
    {
      pos.setZ(screenLayerZPos[i]);
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << ": placing screen layer with ID: "
	     << screenLayers[i]->GetSamplerID() << G4endl;
#endif
      //Create a new physical volume placement for each groove in the screen.
      screenLayers[i]->SetPhys(new G4PVPlacement(nullptr,
						 pos,
						 screenLayers[i]->GetLog(),
						 screenLayers[i]->GetName(),
						 log,
						 false,
						 screenLayers[i]->GetSamplerID(),
						 true));
    }
}
