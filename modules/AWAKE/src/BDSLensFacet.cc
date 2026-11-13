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
#include "BDSLensFacet.hh"
#include "BDSMaterials.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Trap.hh"

//============================================================
BDSLensFacet::BDSLensFacet(G4String nameIn,
			   G4TwoVector sizeIn,
			   G4double angleIn,
			   G4double baseHeightIn,
			   G4String materialIn):
  _name(nameIn+"_LensFacet"),
  _size(sizeIn),
  _angle(angleIn),
  _baseHeight(baseHeightIn),
  _material(materialIn) 
{
  computeDimensions();
  build();
}

void BDSLensFacet::computeDimensions(){
  G4double aSmallNumber=1e-9;
  if(_baseHeight==0) {_baseHeight=aSmallNumber;}
  _totalHeight = _baseHeight + _size.x()*tan(_angle);
  //phi, theta, psi rotation such that the base is perp. to the z direction.
  _rotation = new G4RotationMatrix(0,CLHEP::pi/2.0,0); 
}


void BDSLensFacet::visAtt()
{
  _visAtt=new G4VisAttributes(G4Colour(0.8,0.2,0.0,0.2));
  _visAtt->SetForceSolid(true);
  _log->SetVisAttributes(_visAtt);
}

void BDSLensFacet::build(){
  G4cout << "Building G4Trap with dimensions: " << _size.x() << " " << _size.y()<< " " <<  _totalHeight << " " <<  _baseHeight << G4endl;
  _solid = new G4Trap((_name+"solid").c_str(), _size.x(), _size.y(), _totalHeight, _baseHeight);
 _log = new G4LogicalVolume(_solid,BDSMaterials::Instance()->GetMaterial(_material),(_name+"_log").c_str(),0,0,0);
  visAtt();
}


BDSLensFacet::~BDSLensFacet(){
}
