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
#include "BDSSpectrVacChamb.hh"
#include "BDSMaterials.hh"
#include "BDSGlobalConstants.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "BDSDebug.hh"
#include "G4SubtractionSolid.hh"

#include <cmath>

BDSSpectrVacChamb::BDSSpectrVacChamb(const G4String &nameIn,
				     G4double lengthZIn,
				     G4double magStartZIn,
				     G4double vacuumEndZIn,
				     G4double screenWidthIn,
				     G4double screenAngleIn,
				     G4double sizeXIn,
				     G4double sizeYIn,
				     G4double thicknessIn,
				     G4double windowOffsetXIn=0.68*CLHEP::cm,
				     G4double strutSizeXIn=0,
                     G4double strutSizeZIn=0,
                     G4String strutMaterialIn="G4_STAINLESS-STEEL"):
  _name(nameIn),
  _lengthZ(lengthZIn),       // Length of the vacuum chamber
  _magStartZ(magStartZIn),   // Start Z position of the magnet
  _vacuumEndZ(vacuumEndZIn), // End Z position of the vacuum chamber
  _screenWidth(screenWidthIn),
  _screenAngle(screenAngleIn),
  _sizeX(sizeXIn),                 // Size of the vacuum window/scintillator screen
  _sizeY(sizeYIn),
  _thickness(thicknessIn),         // Thickness of the vacuum chamber walls
  _windowOffsetX(windowOffsetXIn), // Offset X between the edge of the window and the edge of the beam pipe
  _strutSizeX(strutSizeXIn),       // The size of the support strut
  _strutSizeZ(strutSizeZIn),
  _strutMaterial(strutMaterialIn)  // The material of the support strut
{
  CalculateGeometry();
  Build();
  SetUserLimits();

  /*
  _innerLogVol = new G4LogicalVolume(_innerSolid,
				     BDSMaterials::Instance()->GetMaterial("vacuum"),
				     _name+"_inner_log");
				     
  G4double screenAngle2=shapeOuter->screenAngle2();
  G4double deltaMagStart=std::abs(thickness*cos(screenAngle2));
  G4double deltaScreenEnd=-std::abs(thickness*cos(screenAngle));
  BDSSpectrVacChambShape* shapeInner = new BDSSpectrVacChambShape(_name+"_inner", lengthZ, magStartZ+deltaMagStart, vacuumEndZ+deltaScreenEnd, screenWidth, screenAngle, sizeX, sizeY, thickness);
  */
}

BDSSpectrVacChamb::~BDSSpectrVacChamb(){;}

void BDSSpectrVacChamb::Build(){
  if((_strutSizeX>0)&&(_strutSizeZ>0)) {
    _bBuildStrut=true;
  } else {
    _bBuildStrut=false;
  }
  
  BuildBox();
  BuildTrap();
  BuildSideWall();
  BuildBoxInner();
  if(_bBuildStrut) BuildStrut();
}

void BDSSpectrVacChamb::Place(G4LogicalVolume* motherVolume){

  new G4PVPlacement(_innerTrapRotation,
		    _innerTrapTranslation,
		    _innerLogVolTrap,
		    _name+"_physi_trap_inner",
		    motherVolume,
		    false,
		    0,
		    BDSGlobalConstants::Instance()->CheckOverlaps());

  new G4PVPlacement(_upperTrapRotation,
		    _upperTrapTranslation,
		    _upperLogVolTrap,
		    _name+"_physi_trap_upper",
		    motherVolume,
		    false,
		    0,
		    BDSGlobalConstants::Instance()->CheckOverlaps());

  new G4PVPlacement(_lowerTrapRotation,
		    _lowerTrapTranslation,
		    _lowerLogVolTrap,
		    _name+"_physi_trap_lower",
		    motherVolume,
		    false,
		    0,
		    BDSGlobalConstants::Instance()->CheckOverlaps());

  new G4PVPlacement(_box1Rotation,
		   _box1Translation,
		   _logVolBox1,
		   _name+"_physi_box1",
		    motherVolume,
		    false,
		    0,
		    BDSGlobalConstants::Instance()->CheckOverlaps());

  new G4PVPlacement(_box1RotationInner,
		   _box1TranslationInner,
		   _innerLogVolBox1,
		   _name+"_physi_box1_inner",
		    _logVolBox1,
		    false,
		    0,
		    BDSGlobalConstants::Instance()->CheckOverlaps());

  new G4PVPlacement(_box2Rotation,
		   _box2Translation,
		   _logVolBox2,
		   _name+"_physi_box2",
		    motherVolume,
		    false,
		    0,
		    BDSGlobalConstants::Instance()->CheckOverlaps());

  new G4PVPlacement(_box2RotationInner,
		   _box2TranslationInner,
		   _innerLogVolBox2,
		   _name+"_physi_box2_inner",
		    _logVolBox2,
		    false,
		    0,
		    BDSGlobalConstants::Instance()->CheckOverlaps());

  new G4PVPlacement(_box3Rotation,
		   _box3Translation,
		   _logVolBox3,
		   _name+"_physi_box3",
		    motherVolume,
		    false,
		    0,
		    BDSGlobalConstants::Instance()->CheckOverlaps());

  new G4PVPlacement(_box3RotationInner,
		   _box3TranslationInner,
		   _innerLogVolBox3,
		   _name+"_physi_box3_inner",
		    _logVolBox3,
		    false,
		    0,
		    BDSGlobalConstants::Instance()->CheckOverlaps());

  new G4PVPlacement(_rotSideWall,
		    _transSideWall,
		    _logVolSideWall,
		    _name+"_physi_side_wall",
		    motherVolume,
		    false,
		    0,
		    BDSGlobalConstants::Instance()->CheckOverlaps());
  
  if(_bBuildStrut){
    new G4PVPlacement(_rotStrut,
		      _transStrut,
		      _logVolStrut,
		      _name+"_physi_strut",
		      _innerLogVolTrap,
		      false,
		      0,
		      BDSGlobalConstants::Instance()->CheckOverlaps());
  }
}

void BDSSpectrVacChamb::BuildBox(){
  BuildBox1(); //Upstream box.
  BuildBox2(); //Intermediate box.
  BuildBox3(); //Downstream box.
}

void BDSSpectrVacChamb::BuildBox1(){
  _box1Solid = new G4Box(_name+"_box1Solid", 
			(_sizeX+2*_thickness)/2.0,
			(_sizeY+2*_thickness)/2.0,
			_box1LengthZ/2.0);

  _logVolBox1 = new G4LogicalVolume(_box1Solid,
				    BDSMaterials::Instance()->GetMaterial("G4_Fe"),
				    _name+"_log");
    _logVols.push_back(_logVolBox1);

  G4VisAttributes* Box1VisAtt = new G4VisAttributes(G4Color(1.0,0.0,0.0,0.5));
  Box1VisAtt->SetForceSolid(true);
  Box1VisAtt->SetVisibility(true);
  _logVolBox1->SetVisAttributes(Box1VisAtt);
}

void BDSSpectrVacChamb::BuildBox2(){
  _box2Solid = new G4Box(_name+"_box2Solid", 
			(_sizeX+2*_thickness)/2.0,
			(_sizeY+2*_thickness)/2.0,
			_box2LengthZ/2.0);

  _logVolBox2 = new G4LogicalVolume(_box2Solid,
				    BDSMaterials::Instance()->GetMaterial("G4_Fe"),
				    _name+"_log");
    _logVols.push_back(_logVolBox2);

  G4VisAttributes* Box2VisAtt = new G4VisAttributes(G4Color(1.0,0.0,0.0,0.5));
  Box2VisAtt->SetForceSolid(true);
  Box2VisAtt->SetVisibility(true);
  _logVolBox2->SetVisAttributes(Box2VisAtt);
}

void BDSSpectrVacChamb::BuildBox3(){
  _box3Solid = new G4Box(_name+"_box3Solid", 
			 (_sizeX+2*_thickness)/2.0,
			 (_sizeY+2*_thickness)/2.0,
			 _box3LengthZ/2.0);

  _logVolBox3 = new G4LogicalVolume(_box3Solid,
				    BDSMaterials::Instance()->GetMaterial("G4_Fe"),
				    _name+"_log");
    _logVols.push_back(_logVolBox3);

  G4VisAttributes* Box3VisAtt = new G4VisAttributes(G4Color(1.0,0.0,0.0,0.5));
  Box3VisAtt->SetForceSolid(true);
  Box3VisAtt->SetVisibility(true);
  _logVolBox3->SetVisAttributes(Box3VisAtt);
}

void BDSSpectrVacChamb::BuildBoxInner(){
  BuildBox1Inner();
  BuildBox2Inner();
  BuildBox3Inner();
}
  void BDSSpectrVacChamb::BuildBox1Inner(){
  _box1SolidInner = new G4Box(_name+"_box1SolidInner", 
			     _sizeX/2.0,
			     _sizeY/2.0,
			     _box1LengthZ/2.0);
  
  _innerLogVolBox1 = new G4LogicalVolume(_box1SolidInner,
					BDSMaterials::Instance()->GetMaterial("vacuum"),
					_name+"_log");

      _logVols.push_back(_innerLogVolBox1);

  G4VisAttributes* Box1InnerVisAtt = new G4VisAttributes(G4Color(0.0,1.0,0.0,0.5));
  Box1InnerVisAtt->SetForceSolid(true);
  Box1InnerVisAtt->SetVisibility(true);
  _innerLogVolBox1->SetVisAttributes(Box1InnerVisAtt);
}
  void BDSSpectrVacChamb::BuildBox2Inner(){
  _box2SolidInner = new G4Box(_name+"_box2SolidInner", 
			      (_sizeX+_thickness)/2.0,
			     _sizeY/2.0,
			     _box2LengthZ/2.0);
  
  _innerLogVolBox2 = new G4LogicalVolume(_box2SolidInner,
					BDSMaterials::Instance()->GetMaterial("vacuum"),
					_name+"_log");

      _logVols.push_back(_innerLogVolBox2);

  G4VisAttributes* Box2InnerVisAtt = new G4VisAttributes(G4Color(0.0,1.0,0.0,0.5));
  Box2InnerVisAtt->SetForceSolid(true);
  Box2InnerVisAtt->SetVisibility(true);
  _innerLogVolBox2->SetVisAttributes(Box2InnerVisAtt);
}
  void BDSSpectrVacChamb::BuildBox3Inner(){
  _box3SolidInner = new G4Box(_name+"_box3SolidInner", 
			     _sizeX/2.0,
			     _sizeY/2.0,
			     _box3LengthZ/2.0);
  
  _innerLogVolBox3 = new G4LogicalVolume(_box3SolidInner,
					BDSMaterials::Instance()->GetMaterial("vacuum"),
					_name+"_log");

      _logVols.push_back(_innerLogVolBox3);

  G4VisAttributes* Box3InnerVisAtt = new G4VisAttributes(G4Color(0.0,1.0,0.0,0.5));
  Box3InnerVisAtt->SetForceSolid(true);
  Box3InnerVisAtt->SetVisibility(true);
  _innerLogVolBox3->SetVisAttributes(Box3InnerVisAtt);
}


void BDSSpectrVacChamb::BuildTrap(){
  _innerTrapSolid = new G4GenericTrap(_name+"_trapSolid",
				      _trapLengthX/2.0, //Half length of the trapezoid component.
				      _innerTrapVertices
				      );

  _upperTrapSolid = new G4GenericTrap(_name+"_upperTrapSolid",
				      _trapLengthX/2.0, //Half length of the trapezoid component.
				      _upperTrapVertices
				      );

  _lowerTrapSolid = _upperTrapSolid;

  _innerLogVolTrap = new G4LogicalVolume(_innerTrapSolid,
				    BDSMaterials::Instance()->GetMaterial("vacuum"),
				    _name+"_inner_trap_log");

  _upperLogVolTrap = new G4LogicalVolume(_upperTrapSolid,
				    BDSMaterials::Instance()->GetMaterial("G4_Fe"),
				    _name+"_upper_trap_log");

  _lowerLogVolTrap = new G4LogicalVolume(_lowerTrapSolid,
				    BDSMaterials::Instance()->GetMaterial("G4_Fe"),
				    _name+"_lower_trap_log");

    _logVols.push_back(_innerLogVolTrap);
    _logVols.push_back(_upperLogVolTrap);
    _logVols.push_back(_lowerLogVolTrap);

  G4VisAttributes* InnerTrapVisAtt = new G4VisAttributes(G4Color(1.0,0.0,0.0,0.1));
  InnerTrapVisAtt->SetForceSolid(true);
  InnerTrapVisAtt->SetVisibility(true);
  _innerLogVolTrap->SetVisAttributes(InnerTrapVisAtt);
  G4VisAttributes* OuterTrapVisAtt = new G4VisAttributes(G4Color(0.0,1.0,0.0,0.1));
  OuterTrapVisAtt->SetForceSolid(true);
  OuterTrapVisAtt->SetVisibility(true);
  _lowerLogVolTrap->SetVisAttributes(OuterTrapVisAtt);
  _upperLogVolTrap->SetVisAttributes(OuterTrapVisAtt);
}

void BDSSpectrVacChamb::BuildSideWall(){
  _sideWallSolid = new G4Box(_name+"_sideWallSolid", 
			     (_thickness)/2.0,
			     (_sizeY+2*_thickness)/2.0,
			     _sideWallLength/2.0);

  _logVolSideWall = new G4LogicalVolume(_sideWallSolid,
				    BDSMaterials::Instance()->GetMaterial("G4_Fe"),
				    _name+"_sideWall_log");

    _logVols.push_back(_logVolSideWall);

  G4VisAttributes* SideWallVisAtt = new G4VisAttributes(G4Color(0.0,0.0,1.0,0.4));
  SideWallVisAtt->SetForceSolid(true);
  SideWallVisAtt->SetVisibility(true);
  _logVolSideWall->SetVisAttributes(SideWallVisAtt);
}

void BDSSpectrVacChamb::BuildStrut(){
  _strutSolid = new G4Box(_name+"_strutSolid", 
			     _strutSizeZ/2.0, //depth
			     _sizeY/2.0,      //height
			     _strutSizeX/2.0); //width

  _logVolStrut = new G4LogicalVolume(_strutSolid,
				     BDSMaterials::Instance()->GetMaterial(_strutMaterial.c_str()),
				    _name+"_strut_log");

    _logVols.push_back(_logVolStrut);

  G4VisAttributes* StrutVisAtt = new G4VisAttributes(G4Color(1.0,0.0,0.0,0.4));
  StrutVisAtt->SetForceSolid(true);
  StrutVisAtt->SetVisibility(true);
  _logVolStrut->SetVisAttributes(StrutVisAtt);
}

void BDSSpectrVacChamb::CalculateGeometry(){
  _trapLengthZ=_vacuumEndZ-_magStartZ;
  G4double trapLengthZ2=std::abs((_screenWidth)*cos(_screenAngle))+std::abs(_windowOffsetX*std::tan(_screenAngle));
  _trapLengthX=std::abs((_screenWidth)*sin(_screenAngle) + _windowOffsetX);
  
  G4double y1=-_sizeY/2.0;
  G4double y2= _sizeY/2.0;

  G4double x1=-_trapLengthZ/2.0;
  G4double x3=x1+_trapLengthZ;
  G4double x2=x3-trapLengthZ2;

  _innerTrapVertices.push_back(G4TwoVector(x1,y1));
  _innerTrapVertices.push_back(G4TwoVector(x1,y2));
  _innerTrapVertices.push_back(G4TwoVector(x3,y2));
  _innerTrapVertices.push_back(G4TwoVector(x3,y1));
  _innerTrapVertices.push_back(G4TwoVector(x2,y1));
  _innerTrapVertices.push_back(G4TwoVector(x2,y2));
  _innerTrapVertices.push_back(G4TwoVector(x2,y2));
  _innerTrapVertices.push_back(G4TwoVector(x2,y1));

  G4double y1Up=-_thickness/2.0;
  G4double y2Up= _thickness/2.0;

  _upperTrapVertices.push_back(G4TwoVector(x1,y1Up));
  _upperTrapVertices.push_back(G4TwoVector(x1,y2Up));
  _upperTrapVertices.push_back(G4TwoVector(x3,y2Up));
  _upperTrapVertices.push_back(G4TwoVector(x3,y1Up));
  _upperTrapVertices.push_back(G4TwoVector(x2,y1Up));
  _upperTrapVertices.push_back(G4TwoVector(x2,y2Up));
  _upperTrapVertices.push_back(G4TwoVector(x2,y2Up));
  _upperTrapVertices.push_back(G4TwoVector(x2,y1Up));

  _innerTrapTranslation.setX(  (_trapLengthX+_sizeX)/2.0 + _thickness);
  G4double localMagStartZ = _magStartZ + _lengthZ/2.0;
  G4double localVacuumEndZ = _vacuumEndZ + _lengthZ/2.0;
  _innerTrapTranslation.setZ(-_lengthZ/2.0 + _trapLengthZ/2.0 + localMagStartZ );
  _innerTrapRotation = new G4RotationMatrix();
  _innerTrapRotation->rotateX(CLHEP::pi);
  _innerTrapRotation->rotateY(-CLHEP::pi/2.0);

  _upperTrapRotation=_innerTrapRotation;
  _upperTrapTranslation = _innerTrapTranslation;
  _upperTrapTranslation.setY(_upperTrapTranslation.y() + _sizeY/2.0 + _thickness/2.0);
  _lowerTrapRotation=_innerTrapRotation;
  _lowerTrapTranslation = _innerTrapTranslation;
  _lowerTrapTranslation.setY(_lowerTrapTranslation.y() - _sizeY/2.0 - _thickness/2.0);

  _box1LengthZ=localMagStartZ;
  _box3LengthZ=_lengthZ-localVacuumEndZ;
  _box2LengthZ=_lengthZ-_box3LengthZ-_box1LengthZ;

  G4double trapCentre = _magStartZ+_trapLengthZ/2.0;

  _box1Rotation=new G4RotationMatrix();
  _box1RotationInner=new G4RotationMatrix();
  _box1Translation.setZ(_magStartZ - _box1LengthZ/2.0);

  _box2Rotation=new G4RotationMatrix();
  _box2RotationInner=new G4RotationMatrix();
  _box2TranslationInner.setX(_thickness/2.0);
  _box2Translation.setZ(trapCentre);

  _box3Rotation=new G4RotationMatrix();
  _box3RotationInner=new G4RotationMatrix();
  _box3Translation.setZ(_vacuumEndZ+_box3LengthZ/2.0);

  G4double trapLengthZ1 = _trapLengthZ-trapLengthZ2;
  _sideWallLength=std::hypot(trapLengthZ1,_trapLengthX);
  G4double sideWallAngle=atan(_trapLengthX/trapLengthZ1);
	//Reduce the side wall length to avoid overlaps with the scintillator screen frame
	_sideWallLength -= 0.5*std::abs(_thickness*std::sin(sideWallAngle));
  _rotSideWall = new G4RotationMatrix();
  _rotSideWall->rotateY(-sideWallAngle);
  //  _rotSideWall->rotateY(-CLHEP::pi/2.0);

  _transSideWall.setX(_sizeX/2.0 + _thickness + _trapLengthX/2.0 +  (_thickness/2.0)*cos(sideWallAngle));
  _transSideWall.setZ(_magStartZ + trapLengthZ1/2.0 - (_thickness/2.0)*sin(sideWallAngle));

  _rotStrut=new G4RotationMatrix();
  _rotStrut->rotateY(_screenAngle);
  _transStrut.setX(_trapLengthZ/2.0-trapLengthZ2/2.0-std::abs((_strutSizeZ/2.0)*cos(_screenAngle)));
  _transStrut.setY(0);
  _transStrut.setZ(-std::abs((_strutSizeZ/2.0)*sin(_screenAngle)));//-_trapLengthX/2.0+);
  printGeom();
}

void BDSSpectrVacChamb::printGeom(){
  G4cout << __METHOD_NAME__ << G4endl;
  printTrapVertices(_innerTrapVertices, "_innerTrapVertices");
  printTrapVertices(_upperTrapVertices, "_upperTrapVertices");
}

void BDSSpectrVacChamb::printTrapVertices(std::vector<G4TwoVector> vertices, const G4String& name){
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << name << ":" << G4endl;
  G4cout << "(n,  x,  y)" << G4endl;
  int n=0;
  for(std::vector<G4TwoVector>::const_iterator vertIt = vertices.begin();
      vertIt!=vertices.end();
      ++vertIt){
    G4cout << "(" << n << ", " << (*vertIt).x() << ", " << (*vertIt).y() << ")" << G4endl;
    n++;
  }
}

void BDSSpectrVacChamb::SetUserLimits()
{
    auto userLimits = BDSGlobalConstants::Instance()->DefaultUserLimits();
    for(unsigned long i=0; i<_logVols.size(); i++) {
    _logVols.at(i)->SetUserLimits(userLimits);
    }
}


