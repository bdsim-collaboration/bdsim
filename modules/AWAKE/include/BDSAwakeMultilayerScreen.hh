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
#ifndef AWAKEMULTILAYERSCREEN_H
#define AWAKEMULTILAYERSCREEN_H

#include "BDSMultilayerScreen.hh"

class BDSAwakeMultilayerScreen : public BDSMultilayerScreen
{
public:
  BDSAwakeMultilayerScreen(G4String material,
			   G4double thickness,
			   G4double windowScreenGap,
			   G4double dgrain,
			   G4double windowThickness,
			   G4String windowMaterial,
			   G4double mountThickness,
			   G4String mountMaterial,
			   G4double width=1*CLHEP::m,
			   G4double height=65*CLHEP::mm);
  virtual ~BDSAwakeMultilayerScreen();
  void surfaces();
  virtual void Place(G4RotationMatrix* rot,
		     const G4ThreeVector& pos,
		     G4LogicalVolume* motherVol);

private:
  typedef BDSMultilayerScreen super;
  void layers();
  void simpleLayers();
  void sampler(G4String name, const char* material="air", G4bool bSampler=true);
  void thinAirLayer();  
  void thinVacuumLayer();  
  void preWindowSampler();
  void postWindowSampler();
  void windowScreenGap();
  void preScreenSampler();
  void postMountSampler();
  void postScreenSampler();
  void windowLayer();
  void backLayer();
  void substrateLayer();
  void mountLayer();
  void binderLayer();
  void backBinderLayer();
		void thickScintLayer();
  void scintillatorLayer(G4double thickness);
  void backScintillatorLayer();
  void frontScintillatorLayer1();
  void frontScintillatorLayer2();
  void frontLayer();
  virtual void ReflectiveSurface();
  virtual void RoughSurface();
  
  G4String _material;
  G4double _thickness;
  G4double _windowScreenGap;
  G4double _gapWidth;
  G4double _gapSpacing;
  G4double _dgrain;
  G4double _windowThickness;
  G4String _windowMaterial;
  G4double _mountThickness;
  G4String _mountMaterial;
  G4double _layerThickness;
  G4double _binderLayerThickness;
  G4double _firstLayerThickness;
  G4double _firstBinderLayerThickness;
  G4double _nScintLayers;
  G4double _fillFactor;

  G4int _scintLayerCount;
  G4int _binderLayerCount;
  std::stringstream _ss;
  G4String _binderLayerName;
  G4String _scintLayerName;
  
  void incBinderLayer();
  void incScintLayer();
  
};

#endif
