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
#include "BDSCCDPixel.hh"
#include "BDSCCDPixelSD.hh"
#include "BDSMaterials.hh"
#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "BDSDebug.hh"

#include "G4SDManager.hh"

extern BDSCCDPixelSD* BDSCCDPixelSensDet;


//============================================================
BDSCCDPixel::BDSCCDPixel (G4ThreeVector sizeIn, G4String nameIn):
  _size(sizeIn), _name(nameIn+"_CCDPixel")
{
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
  _sensDet=new BDSCCDPixelSD(_name);
  SDMan->AddNewDetector(_sensDet);
  build();
  _log->SetSensitiveDetector(_sensDet);
}


void BDSCCDPixel::visAtt()
{
  _visAtt=new G4VisAttributes(G4Colour(0.8,0.2,0.0));
  _visAtt->SetForceWireframe(true);
  _log->SetVisAttributes(_visAtt);
}

void BDSCCDPixel::build(){
  _solid  = new G4Box((_name+"_solid").c_str(),_size.x()/2.0,_size.y()/2.0,_size.z()/2.0);
  _log = new G4LogicalVolume(_solid,BDSMaterials::Instance()->GetMaterial("fusedsilica"),(_name+"_log").c_str(),0,0,0);
  visAtt();
}

G4String BDSCCDPixel::name(){
  return _name;
}

G4LogicalVolume* BDSCCDPixel::log(){
  return _log;
}

G4ThreeVector BDSCCDPixel::size(){
  return _size;
}

BDSCCDPixel::~BDSCCDPixel(){
}
