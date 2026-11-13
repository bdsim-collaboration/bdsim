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
#include "BDSLens.hh"
#include "BDSMaterials.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Tubs.hh"
#include "BDSDebug.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4Orb.hh"

#include "BDSDebug.hh"

//============================================================
BDSLens::BDSLens (G4String nameIn,
		  G4double diameterIn,
		  G4double rhoIn,
		  G4double centreThicknessIn):
_name(nameIn+"_Lens"),
_diameter(diameterIn),
_rho(rhoIn),
_centreThickness(centreThicknessIn)
{
  build();
}


void BDSLens::visAtt()
{
  _visAtt=new G4VisAttributes(G4Colour(0.5,0.8,0.5,0.3));
  _visAtt->SetForceSolid(true);
  _log->SetVisAttributes(_visAtt);
}

void BDSLens::build(){
  G4Tubs* tube = new G4Tubs(_name+"_tempTube", _diameter, _rho,_rho,0,CLHEP::twopi*CLHEP::radian);
  G4SubtractionSolid* sideA = new G4SubtractionSolid(_name+"_sideA",new G4Orb(_name+"_tempOrbA",_rho),tube);
  G4SubtractionSolid* sideB = new G4SubtractionSolid(_name+"_sideB",new G4Orb(_name+"_tempOrbB",_rho),tube);

  //  G4SubtractionSolid* sideA = new G4Orb(_name+"_tempOrbA",_rho);
  //  G4SubtractionSolid* sideB = new G4Orb(_name+"_tempOrbB",_rho);

  G4ThreeVector transVec;
  transVec.setZ(2*_rho-_centreThickness);
  G4IntersectionSolid* intersectionA = new G4IntersectionSolid(_name+"_intersectionA", sideA, sideB, (G4RotationMatrix*)nullptr,transVec);
  transVec.setZ(-1*_rho+_centreThickness/2.0);
  //  _solid = new G4IntersectionSolid(_name+"_solid", new G4Box(_name+"tempBox",_diameter/2.0,_diameter/2.0,_centreThickness/2.0), intersectionA,(G4RotationMatrix*)nullptr,transVec);
  _solid = new G4IntersectionSolid(_name+"_solid", new G4Tubs(_name+"tempTube2",0,_diameter/2.0,_centreThickness/2.0,0,CLHEP::twopi*CLHEP::radian), intersectionA,(G4RotationMatrix*)nullptr,transVec);
  _log = new G4LogicalVolume(_solid,BDSMaterials::Instance()->GetMaterial("n-bk7"),(_name+"_log").c_str(),0,0,0);
  visAtt();
}

G4String BDSLens::name(){
  return _name;
}

G4LogicalVolume* BDSLens::log(){
  return _log;
}

BDSLens::~BDSLens(){
}
