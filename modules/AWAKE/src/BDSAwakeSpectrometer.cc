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
#include "BDSAwakeSpectrometer.hh"

#include "BDSGlobalConstants.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldType.hh"
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"
#include "BDSSampler.hh"
#include "BDSSDManager.hh"
#include "BDSSDSampler.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSSDType.hh"
#include "BDSSpectrVacChamb.hh"
#include "BDSTunnelInfo.hh"
#include "BDSCCDCamera.hh"
#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4SubtractionSolid.hh"
#include "BDSDebug.hh"
#include "BDSAwakeMultilayerScreen.hh"
#include "BDSScreenFrameRectangular.hh"
//#include "UltraFresnelLens.hh"
//#include "UltraFresnelLensParameterisation.hh"

//============================================================
BDSAwakeSpectrometer::BDSAwakeSpectrometer (G4String aName,
                                            G4double magnetOffsetXIn,
                                            G4double length=2.7*CLHEP::m,
                                            BDSFieldInfo* fieldInfoIn = nullptr,
                                            G4double poleStartZIn=62.733*CLHEP::cm,
                                            G4String materialIn="lanex",
                                            G4double thicknessIn = 0.3 * CLHEP::mm,
                                            G4double screenPSizeIn=25*CLHEP::um,
                                            G4double windowScreenGapIn=0,
                                            G4double angleIn = -45*CLHEP::pi/180.0,
                                            G4double windowThicknessIn=0,
                                            G4String windowMaterialIn="G4_Al",
                                            G4double mountThicknessIn=0,
                                            G4String mountMaterialIn="G4_BAKELITE",
                                            G4double screenEndZIn = (258-62.733)*CLHEP::cm,
                                            G4String specIn="",
                                            G4double screenWidthIn=1*CLHEP::m):
  BDSAcceleratorComponent(aName, length, 0, "awakespectrometer"),
  _fieldInfo(fieldInfoIn),
  _magnetOffsetX(magnetOffsetXIn),
  _totalThickness(0.507*CLHEP::mm),
  _screenWidth(screenWidthIn),
  _frame_x_dim(0),
  _mlScreen(nullptr),
  _camera(nullptr),
  _material(materialIn),
  _thickness(thicknessIn),
  _screenPSize(screenPSizeIn),
  _windowScreenGap(windowScreenGapIn),
  _screenAngle(angleIn),
  _windowThickness(windowThicknessIn),
  _windowMaterial(windowMaterialIn),
  _mountThickness(mountThicknessIn),
  _mountMaterial(mountMaterialIn),
  _screenEndZ(screenEndZIn),
  _poleStartZ(poleStartZIn)
{
  //Change sign of angle
  _screenAngle*=-1;

  try{
    _vacuumChamberType=BDS::GetParameterValueInt(specIn,"vacuumChamberType");
  } catch(std::invalid_argument&){
    //If the cast fails, set vacuum chamber type to 1
    _vacuumChamberType=1;
  }

  try{
    _magnetGeometryType=BDS::GetParameterValueInt(specIn,"magnetGeometryType");
  } catch(std::invalid_argument&){
    //If the cast fails, set magnet geometry type to 1
    _magnetGeometryType=1;
  }

    //Vacuum chamber dimensions.
    _vacThickness=6*CLHEP::mm;
    _vacInnerWidth=64*CLHEP::mm;
    _vacInnerHeight=_vacInnerWidth;
    _vacHeight=_vacInnerHeight+2*_vacThickness;

    try{
        _windowOffsetX=BDS::GetParameterValueDouble(specIn,"windowOffsetX");
    } catch(std::invalid_argument&){
        _windowOffsetX=64.8*CLHEP::mm; //Total offset of front of flange from beam axis
    }
          _windowOffsetXFromVCEdge=_windowOffsetX -_vacInnerWidth/2.0-_vacThickness; //Distance to outer edge of beam pipe

   G4cout << "Window Offset from VC Edge is " << _windowOffsetXFromVCEdge << G4endl;

    try{
        _strutSizeX=BDS::GetParameterValueDouble(specIn,"strutSizeX");
    } catch(std::invalid_argument&){
        _strutSizeX=0;
    }

  try{
    _strutSizeZ=BDS::GetParameterValueDouble(specIn,"strutSizeZ");
  } catch(std::invalid_argument&){
    _strutSizeZ=0;
  }

  try{
    _strutMaterial=BDS::GetParameterValueString(specIn,"strutMaterial");
  } catch(std::invalid_argument&){
    _strutMaterial="G4_STAINLESS-STEEL";
  }

  //Screen width 1m by default.
  if(_screenWidth<=0)
  {_screenWidth = 1*CLHEP::m;}
  _screenHeight=64*CLHEP::mm;

  //Set the rotation of the screen
  _screenRotationMatrix = new G4RotationMatrix();
  _screenRotationMatrix->rotateY(-CLHEP::pi/2.0+_screenAngle);

  _vacRotationMatrix = new G4RotationMatrix();

  _magRotationMatrix = new G4RotationMatrix();
}

void BDSAwakeSpectrometer::MagnetDefaults(){
  //Initialise geometry pointers.
  itsCoilLog=nullptr;
  //Gap between the pole faces.
  itsPoleAperture=65*CLHEP::mm;
  //Gap between the coil faces.
  itsCoilAperture=180*CLHEP::mm;
  //Part of pole extending below coil
  G4double outerPoleSize=0.5*50*CLHEP::mm;
  //Initialise dimensions.
  itsYokeSize.set(
                  902*CLHEP::mm,
                  1168*CLHEP::mm,
                  1*CLHEP::m
                  );

  itsCoilSize.set(
                  //  (320+320+262)*CLHEP::mm,
                  262*CLHEP::mm,
                  180*CLHEP::mm,
                  (1000+320+285)*CLHEP::mm
                  );

  itsPoleSize.set(
                  320*CLHEP::mm,
                  //itsCoilSize.y()+
                  outerPoleSize,
                  itsYokeSize.z()
                  );

  itsYokeMiddleSize=itsYokeSize;
  itsYokeMiddleSize.setY(2*itsCoilSize.y()+itsCoilAperture);
  G4double yokeCoilGap=30*CLHEP::mm;
  itsYokeMiddleSize.setX(itsYokeSize.x()-itsPoleSize.x()-itsCoilSize.x()-yokeCoilGap);
  itsYokeUpperSize=itsYokeSize;
  itsYokeUpperSize.setY((itsYokeSize.y()-itsYokeMiddleSize.y())/2.0);
  itsYokeLowerSize=itsYokeUpperSize;

  itsMiddleCoilSize.set(
                  itsPoleSize.x(),
                  itsCoilSize.y(),
                  itsCoilSize.x()
                  );

  //Yoke apertures.
  itsAperture1Size.set(
                       262*CLHEP::mm+yokeCoilGap,
                       itsCoilAperture+2*itsCoilSize.y(),
                       itsYokeSize.z()
                       );

  itsAperture2Size.set(
                       itsPoleSize.x(),
                       itsPoleAperture,
                       itsYokeSize.z()
                       );

  //the position of the magnet centre relative to the marker volume.
  itsPolePos.set(
                 _magnetOffsetX,
                 0,
                 _poleStartZ+itsYokeSize.z()/2.0
                 );

  //The field map centre corresponds with the pole centre.
  //  itsBmapZOffset=(-itsLength/2.0 + 62.733*CLHEP::cm)*0.5;
  itsBmapZOffset=itsPolePos.z();
  itsBmapXOffset=itsPolePos.x();

  //The position of the yoke relative to the marker volume
  itsYokePos.set(
                 itsPolePos.x()+(itsPoleSize.x()-itsYokeSize.x())/2.0,
                 itsPolePos.y(),
                 itsPolePos.z()
                 );

  itsYokeUpperPos=itsYokePos;
  itsYokeUpperPos.setX(itsPolePos.x()+(itsPoleSize.x()-itsYokeUpperSize.x())/2.0);
  itsYokeUpperPos.setY(itsYokePos.y()+itsYokeMiddleSize.y()/2.0+itsYokeUpperSize.y()/2.0);

  itsYokeMiddlePos=itsYokePos;
  //  itsYokeMiddlePos.setX(-itsYokeSize.x()/2.0+itsYokeMiddleSize.x()/2.0);
  itsYokeMiddlePos.setX(itsYokePos.x()-itsYokeSize.x()/2.0+itsYokeMiddleSize.x()/2.0);

  itsYokeLowerPos=itsYokePos;
  itsYokeLowerPos.setX(itsPolePos.x()+(itsPoleSize.x()-itsYokeUpperSize.x())/2.0);
  itsYokeLowerPos.setY(itsYokePos.y()-itsYokeMiddleSize.y()/2.0-itsYokeUpperSize.y()/2.0);

  itsUpstreamCoilLength=285*CLHEP::mm; //The part of the could that sticks out of the upstream/downstream end;
  itsDownstreamCoilLength=320*CLHEP::mm;
  //The coil position relative to the marker volume.
  itsCoilPos.set(
                 // (itsYokeSize.x() + itsCoilSize.x())/2.0 - (320+262)*CLHEP::mm,
                 itsYokeSize.x()/2.0 -itsAperture1Size.x() - itsAperture2Size.x() + yokeCoilGap + itsCoilSize.x()/2.0,
                 0,
                 (itsDownstreamCoilLength-itsUpstreamCoilLength)/2.0
                 );
  itsCoilPos += itsYokePos;


  //Aperture position relative to the centre of the yoke.
  itsAperture2Pos.set(
                     (itsYokeSize.x()-itsAperture2Size.x())/2.0, //C-type magnet, aperture is to the side.
                     0, //Aperture is centred vertically around the magnet centre.
                     0
                     );

  itsAperture1Pos.set(
                      itsYokeSize.x()/2.0-itsAperture2Size.x()-itsAperture1Size.x()/2.0, //C-type magnet, aperture is to the side.
                      0, //Aperture is centred vertically around the magnet centre.
                      0
                      );

  //Upper coil position relative to marker volume centre.
  itsUpperCoilPos.set(
                      itsCoilPos.x(),
                      (itsCoilAperture+itsCoilSize.y())/2.0,
                      itsCoilPos.z()
                      );
  itsLowerCoilPos.set(
                      itsCoilPos.x(),
                      -(itsCoilAperture+itsCoilSize.y())/2.0,
                      itsCoilPos.z()
                      );

  itsUpperFrontCoilPos.set(
                           itsUpperCoilPos.x()+itsCoilSize.x()/2.0+itsMiddleCoilSize.x()/2.0,
                           itsUpperCoilPos.y(),
                           itsCoilPos.z()+itsCoilSize.z()/2.0-itsMiddleCoilSize.z()/2.0
                           );

  itsLowerFrontCoilPos.set(
                           itsUpperCoilPos.x()+itsCoilSize.x()/2.0+itsMiddleCoilSize.x()/2.0,
                           itsLowerCoilPos.y(),
                           itsCoilPos.z()+itsCoilSize.z()/2.0-itsMiddleCoilSize.z()/2.0
                           );

  itsUpperRearCoilPos.set(
                          itsUpperCoilPos.x()+itsCoilSize.x()/2.0+itsMiddleCoilSize.x()/2.0,
                          itsUpperCoilPos.y(),
                          itsCoilPos.z()-itsCoilSize.z()/2.0+itsMiddleCoilSize.z()/2.0
                          );

  itsLowerRearCoilPos.set(
                          itsUpperCoilPos.x()+itsCoilSize.x()/2.0+itsMiddleCoilSize.x()/2.0,
                          itsLowerCoilPos.y(),
                          itsCoilPos.z()-itsCoilSize.z()/2.0+itsMiddleCoilSize.z()/2.0
                          );


  itsUpperLeftCoilPos.set(
                          itsUpperCoilPos.x()+itsAperture2Size.x()+itsCoilSize.x(),
                          itsUpperCoilPos.y(),
                          itsUpperCoilPos.z()
                          );

  itsLowerLeftCoilPos.set(
                          itsLowerCoilPos.x()+itsAperture2Size.x()+itsCoilSize.x(),
                          itsLowerCoilPos.y(),
                          itsLowerCoilPos.z()
                          );

  itsUpperPolePos.set(
                      itsPolePos.x(),
                      itsPolePos.y()+(itsPoleSize.y()+itsPoleAperture)/2.0,
                      itsPolePos.z()
                      );

  itsLowerPolePos.set(
                      itsPolePos.x(),
                      itsPolePos.y()-(itsPoleSize.y()+itsPoleAperture)/2.0,
                      itsPolePos.z()
                      );
}

void BDSAwakeSpectrometer::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.3,0.4,0.2));
  itsVisAttributes->SetForceWireframe(true);

  _visAttFront=new G4VisAttributes(G4Colour(1.0,0.0,0.0,0.5));
  _visAttScint=new G4VisAttributes(G4Colour(0.0,1.0,0.0,0.5));
  _visAttBase =new G4VisAttributes(G4Colour(0.3,0.3,0.3,0.5));
  _visAttSampler=new G4VisAttributes(G4Colour(0.2,0.2,0.0,0.5));


  _visAttFront->SetForceSolid(true);
  _visAttScint->SetForceSolid(true);
  _visAttBase->SetForceSolid(true);
  _visAttSampler->SetForceSolid(true);
  _visAttSampler->SetVisibility(true);
}

void BDSAwakeSpectrometer::BuildField()
{
    //The position of the field is relative to the pole position of the magnet. The field is translated accordingly.
    G4ThreeVector poleTranslation = G4ThreeVector(itsBmapXOffset,0,itsBmapZOffset);
    G4cout << __METHOD_NAME__ << itsBmapXOffset << " " << itsBmapZOffset << G4endl;
    _fieldInfo->Translate(poleTranslation);

  if (_fieldInfo->FieldType() == BDSFieldType::dipole)
  {// pure dipole field
    BDSFieldBuilder::Instance()->RegisterFieldForConstruction(_fieldInfo,
                                                              _vacChamb->InnerBoxLogVol2(),
                                                              true);
    BDSFieldBuilder::Instance()->RegisterFieldForConstruction(_fieldInfo,
                                                              _vacChamb->InnerTrapLogVol(),
                                                              true);
  }
  else
  {// field map
    BDSFieldBuilder::Instance()->RegisterFieldForConstruction(_fieldInfo,
                                                              containerLogicalVolume,
                                                              true);
  }



}

void BDSAwakeSpectrometer::BuildMagnet(){
    BuildYoke();
    BuildCoils();
}

void BDSAwakeSpectrometer::BuildCoils(){
  G4ThreeVector size = itsCoilSize;
  G4VSolid*  coilSolid = new G4Box("coilSolid",size.x()/2.0,size.y()/2.0,size.z()/2.0);
  G4VSolid*  middleCoilSolid = new G4Box("middleCoilSolid",itsMiddleCoilSize.x()/2.0,itsMiddleCoilSize.y()/2.0,itsMiddleCoilSize.z()/2.0);
  /*
  G4ThreeVector subtractPos;
  G4VSolid* coilSubtract = new G4Box("coilSubtract", itsAperture2Size.x()/2.0, size.y()/2.0,size.z()/2.0);
  subtractPos.set(
                  itsPolePos.x()-itsCoilPos.x(),
                  0,
                  size.z()/2.0
                  );
  G4VSolid* coilSolid = new G4SubtractionSolid("coilSolid", coilSolid1, coilSubtract, nullRotationMatrix,
                                               subtractPos);
  */

  itsCoilLog = new G4LogicalVolume(coilSolid, BDSMaterials::Instance()->GetMaterial("G4_Cu"),"itsCoilLog",0,0,0);

  itsMiddleCoilLog = new G4LogicalVolume(middleCoilSolid, BDSMaterials::Instance()->GetMaterial("G4_Cu"),"itsMiddleCoilLog",0,0,0);

  G4VisAttributes* CoilVisAtt = new G4VisAttributes(G4Color(0.0,0.5,0.5,0.5));
  CoilVisAtt->SetForceSolid(true);
  CoilVisAtt->SetVisibility(true);
  itsCoilLog->SetVisAttributes(CoilVisAtt);
  itsMiddleCoilLog->SetVisAttributes(CoilVisAtt);
}

void BDSAwakeSpectrometer::PlaceCoils(){
  if(itsCoilLog == nullptr){
    BuildCoils();
  }
  new G4PVPlacement(_magRotationMatrix,itsUpperCoilPos,itsCoilLog,"CoilUpper",
                    containerLogicalVolume,false,0,BDSGlobalConstants::Instance()->CheckOverlaps());

  new G4PVPlacement(_magRotationMatrix,itsLowerCoilPos,itsCoilLog,"CoilLower",
                    containerLogicalVolume,false,0,BDSGlobalConstants::Instance()->CheckOverlaps());

  new G4PVPlacement(_magRotationMatrix,itsUpperLeftCoilPos,itsCoilLog,"CoilUpperLeft",
                    containerLogicalVolume,false,0,BDSGlobalConstants::Instance()->CheckOverlaps());

  new G4PVPlacement(_magRotationMatrix,itsLowerLeftCoilPos,itsCoilLog,"CoilLowerLeft",
                    containerLogicalVolume,false,0,BDSGlobalConstants::Instance()->CheckOverlaps());

  new G4PVPlacement(_magRotationMatrix,itsUpperFrontCoilPos,itsMiddleCoilLog,"CoilUpperFront",
                    containerLogicalVolume,false,0,BDSGlobalConstants::Instance()->CheckOverlaps());

  new G4PVPlacement(_magRotationMatrix,itsLowerFrontCoilPos,itsMiddleCoilLog,"CoilLowerFront",
                    containerLogicalVolume,false,0,BDSGlobalConstants::Instance()->CheckOverlaps());

  new G4PVPlacement(_magRotationMatrix,itsUpperRearCoilPos,itsMiddleCoilLog,"CoilUpperRear",
                    containerLogicalVolume,false,0,BDSGlobalConstants::Instance()->CheckOverlaps());

  new G4PVPlacement(_magRotationMatrix,itsLowerRearCoilPos,itsMiddleCoilLog,"CoilLowerRear",
                    containerLogicalVolume,false,0,BDSGlobalConstants::Instance()->CheckOverlaps());
}



void BDSAwakeSpectrometer::BuildYoke(){
  G4VSolid* YokeUpperSolid = new G4Box("YokeSolid1",itsYokeUpperSize.x()/2.0,itsYokeUpperSize.y()/2.0,itsYokeUpperSize.z()/2.0);

  itsYokeUpperLog = new G4LogicalVolume(YokeUpperSolid, BDSMaterials::Instance()->GetMaterial("G4_Fe"),"itsYokeUpperLog",0,0,0);

  G4VSolid* YokeMiddleSolid = new G4Box("YokeSolid1",itsYokeMiddleSize.x()/2.0,itsYokeMiddleSize.y()/2.0,itsYokeMiddleSize.z()/2.0);

  itsYokeMiddleLog = new G4LogicalVolume(YokeMiddleSolid, BDSMaterials::Instance()->GetMaterial("G4_Fe"),"itsYokeMiddleLog",0,0,0);

  G4VSolid* YokeLowerSolid = new G4Box("YokeSolid1",itsYokeLowerSize.x()/2.0,itsYokeLowerSize.y()/2.0,itsYokeLowerSize.z()/2.0);

  itsYokeLowerLog = new G4LogicalVolume(YokeLowerSolid, BDSMaterials::Instance()->GetMaterial("G4_Fe"),"itsYokeLowerLog",0,0,0);

  G4VisAttributes* YokeVisAtt = new G4VisAttributes(G4Color(0,1,0,0.5));
  YokeVisAtt->SetForceSolid(true);
  YokeVisAtt->SetVisibility(true);
  itsYokeUpperLog->SetVisAttributes(YokeVisAtt);
  itsYokeMiddleLog->SetVisAttributes(YokeVisAtt);
  itsYokeLowerLog->SetVisAttributes(YokeVisAtt);
}

void BDSAwakeSpectrometer::PlaceYoke(){
  if(itsYokeMiddleLog == nullptr){
    BuildYoke();
  }
  new G4PVPlacement(_magRotationMatrix,itsYokeUpperPos,itsYokeUpperLog,"YokeUpper",
                    containerLogicalVolume,false,0,BDSGlobalConstants::Instance()->CheckOverlaps());
  new G4PVPlacement(_magRotationMatrix,itsYokeMiddlePos,itsYokeMiddleLog,"YokeMiddle",
                    containerLogicalVolume,false,0,BDSGlobalConstants::Instance()->CheckOverlaps());
  new G4PVPlacement(_magRotationMatrix,itsYokeLowerPos,itsYokeLowerLog,"YokeLower",
                    containerLogicalVolume,false,0,BDSGlobalConstants::Instance()->CheckOverlaps());
}

void BDSAwakeSpectrometer::BuildVacuumChamber(){
    G4double lenSaf = BDSGlobalConstants::Instance()->LengthSafety();
  switch(_vacuumChamberType){
  case 0:
    _vacChamb=nullptr;
    break;
  case 1:
    _vacChamb = new BDSSpectrVacChamb(name + "_vacChamb",
                                      chordLength,
                                      _poleStartZ,
                                      //_screenEndZ-std::abs(cos(_screenAngle)*_totalThickness),
                                      _screenEndZ-sin(_screenAngle)*_totalThickness,
                                      _screenWidth+lenSaf,
                                      _screenAngle,
                                      _vacInnerWidth+lenSaf,
                                      _vacInnerHeight+lenSaf,
                                      _vacThickness,
                                      _windowOffsetXFromVCEdge,
                                      _strutSizeX,
                                      _strutSizeZ,
                                      _strutMaterial);
    break;
  default:
    G4String exceptionString = (G4String)"vacuumChamberType: " + _vacuumChamberType + (G4String)" unknown.";
    G4Exception(exceptionString.c_str(), "-1", FatalErrorInArgument, "");
  }
}


void BDSAwakeSpectrometer::PlaceVacuumChamber(){
  // 2018 02 - LN - this first bit seems redundant as BuildVacuumChamber() is called in Build()
  // before this method...
  if(_vacuumChamberType!=0){
    if(!_vacChamb){
      BuildVacuumChamber();
    }
    _vacChamb->Place(containerLogicalVolume);
  }
}

void BDSAwakeSpectrometer::BuildCameraScoringPlane(){
  G4String tmp = "_cameraScoringPlane";
  _scoringPlaneName=name+tmp;
  int nThisSampler= BDSSamplerRegistry::Instance()->NumberOfExistingSamplers() + 1;
  G4String ident="_camera";
  _samplerName = ("Sampler_"+std::to_string(nThisSampler)+"_"+_scoringPlaneName);
  _samplerName2 = ("Sampler_"+std::to_string(nThisSampler)+"_"+_scoringPlaneName+"_2");

  //Build and place the volume...
  itsCameraScoringPlaneSolid = new G4Box("CameraScoringPlaneSolid",100*CLHEP::mm/2.0,500*CLHEP::mm/2.0,_scoringPlaneThickness/2.0);

  itsCameraScoringPlaneLog = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"CameraScoringPlaneLog",0,0,0);
  itsCameraScoringPlaneLog->SetVisAttributes(_visAttSampler);

  G4double dispX=_cameraScreenDist-_scoringPlaneThickness/2.0;
  G4double dispY=0;
  G4double dispZ=-_cameraScreenDist/2.0;;

  G4int samplerID1 = BDSSamplerRegistry::Instance()->RegisterSampler(_samplerName,
                                                                     nullptr);
  G4int samplerID2 = BDSSamplerRegistry::Instance()->RegisterSampler(_samplerName2,
                                                                     nullptr);

  G4RotationMatrix* rotY90 = new G4RotationMatrix();
  rotY90->rotateY(CLHEP::halfpi);
  RegisterRotationMatrix(rotY90);

  new G4PVPlacement(rotY90,
                    G4ThreeVector(dispX,dispY,dispZ),
                    itsCameraScoringPlaneLog,
                    _samplerName,
                    containerLogicalVolume,
                    false,
                    samplerID1,
                    BDSGlobalConstants::Instance()->CheckOverlaps());

  itsCameraScoringPlaneLog2 = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"CameraScoringPlaneLog2",0,0,0);
  itsCameraScoringPlaneLog2->SetVisAttributes(_visAttSampler);

  G4double dispX2=-sin(_screenAngle)*_cameraScreenDist;
  G4double dispY2=0;
  G4double dispZ2=cos(_screenAngle)*_cameraScreenDist-_cameraScreenDist/2.0;


  new G4PVPlacement(_screenRotationMatrix,
                    G4ThreeVector(dispX2,dispY2,dispZ2),
                    itsCameraScoringPlaneLog2,
                    _samplerName2,
                    containerLogicalVolume,
                    false,
                    samplerID2,
                    BDSGlobalConstants::Instance()->CheckOverlaps());

  itsCameraScoringPlaneLog->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());
  itsCameraScoringPlaneLog2->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());

  _samplerName3 = ("Sampler_"+std::to_string(nThisSampler)+"_"+_scoringPlaneName+"_3");
  _samplerName4 = ("Sampler_"+std::to_string(nThisSampler)+"_"+_scoringPlaneName+"_4");

  //Build and place the volume...
  itsCameraScoringPlaneLog3 = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"CameraScoringPlaneLog3",0,0,0);
  itsCameraScoringPlaneLog3->SetVisAttributes(_visAttSampler);

  G4double dispX3=_cameraScreenDist/2.0-_scoringPlaneThickness/2.0;
  G4double dispY3=0;
  G4double dispZ3=-_cameraScreenDist/2.0;;

  G4int samplerID3 = BDSSamplerRegistry::Instance()->RegisterSampler(_samplerName3,
                                                                     nullptr);
  G4int samplerID4 = BDSSamplerRegistry::Instance()->RegisterSampler(_samplerName4,
                                                                     nullptr);

  new G4PVPlacement(rotY90,
                    G4ThreeVector(dispX3,dispY3,dispZ3),
                    itsCameraScoringPlaneLog3,
                    _samplerName3,
                    containerLogicalVolume,
                    false,
                    samplerID3,
                    BDSGlobalConstants::Instance()->CheckOverlaps());

  itsCameraScoringPlaneLog4 = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"CameraScoringPlaneLog4",0,0,0);
  itsCameraScoringPlaneLog4->SetVisAttributes(_visAttSampler);

  G4double dispX4=-sin(_screenAngle)*_cameraScreenDist/2.0;
  G4double dispY4=0;
  G4double dispZ4=cos(_screenAngle)*_cameraScreenDist/2.0-_cameraScreenDist/2.0;


  new G4PVPlacement(_screenRotationMatrix,
                    G4ThreeVector(dispX4,dispY4,dispZ4),
                    itsCameraScoringPlaneLog4,
                    _samplerName4,
                    containerLogicalVolume,
                    false,
                    samplerID4,
                    BDSGlobalConstants::Instance()->CheckOverlaps());

  itsCameraScoringPlaneLog3->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());
  itsCameraScoringPlaneLog4->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());

  _samplerName5 = ("Sampler_"+std::to_string(nThisSampler)+"_"+_scoringPlaneName+"_5");
  _samplerName6 = ("Sampler_"+std::to_string(nThisSampler)+"_"+_scoringPlaneName+"_6");

  //Build and place the volume...
  itsCameraScoringPlaneLog5 = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"CameraScoringPlaneLog5",0,0,0);
  itsCameraScoringPlaneLog5->SetVisAttributes(_visAttSampler);

  G4double dispX5=_cameraScreenDist/4.0-_scoringPlaneThickness/2.0;
  G4double dispY5=0;
  G4double dispZ5=-_cameraScreenDist/2.0;;

  G4int samplerID5 = BDSSamplerRegistry::Instance()->RegisterSampler(_samplerName5,
                                                                     nullptr);
  G4int samplerID6 = BDSSamplerRegistry::Instance()->RegisterSampler(_samplerName6,
                                                                     nullptr);

  new G4PVPlacement(rotY90,
                    G4ThreeVector(dispX5,dispY5,dispZ5),
                    itsCameraScoringPlaneLog5,
                    _samplerName5,
                    containerLogicalVolume,
                    false,
                    samplerID5,
                    BDSGlobalConstants::Instance()->CheckOverlaps());

  itsCameraScoringPlaneLog6 = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"CameraScoringPlaneLog6",0,0,0);
  itsCameraScoringPlaneLog6->SetVisAttributes(_visAttSampler);

  G4double dispX6=-sin(_screenAngle)*_cameraScreenDist/4.0;
  G4double dispY6=0;
  G4double dispZ6=cos(_screenAngle)*_cameraScreenDist/4.0-_cameraScreenDist/2.0;


  new G4PVPlacement(_screenRotationMatrix,
                    G4ThreeVector(dispX6,dispY6,dispZ6),
                    itsCameraScoringPlaneLog6,
                    _samplerName6,
                    containerLogicalVolume,
                    false,
                    samplerID6,
                    BDSGlobalConstants::Instance()->CheckOverlaps());

  itsCameraScoringPlaneLog5->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());
  itsCameraScoringPlaneLog6->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());

  itsCameraScoringPlaneLog->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());
}

//void BDSAwakeSpectrometer::BuildFresnelLens(){
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  /*
  G4cout << "#                                                    #" << G4endl ;
  G4cout << "#           Building the Fresnel lens ...            #" << G4endl ;
  G4cout << "#                                                    #" << G4endl ;

  G4double      LensDiameter        = 457*CLHEP::mm ; // Size of the optical active area of the lens.
  G4int      LensNumOfGrooves    = 13 ;
  //G4int      LensNumOfGrooves    = 129 ;
  //G4int      LensNumOfGrooves    = 1287 ;

  G4double      LensBorderThickness = 2.8*CLHEP::mm ;     // Thickness of the border area.
  G4double      LensFocalLength     = 441.973*CLHEP::mm ; // This parameter depends on the lens geometry, etc !!
  G4Material   *LensMaterial        = G4Material::GetMaterial(name = "Acrylic") ;
  G4ThreeVector LensPosition        = UVscopePosition+G4ThreeVector(0.0*CLHEP::mm,0.0*CLHEP::mm,UVscopeHeight/2.0-UVscopeBaffle) ;

  UltraFresnelLens *FresnelLens = new UltraFresnelLens(LensDiameter,LensNumOfGrooves,LensMaterial,_log) ;
  */
//}


void BDSAwakeSpectrometer::BuildScreenScoringPlane(){
  G4String tmp = "_screenScoringPlane";
  _screenScoringPlaneName=name+tmp;
  int nThisSampler= BDSSamplerRegistry::Instance()->NumberOfExistingSamplers() + 1;
  G4String ident="_screen";
  _screenSamplerName  = ("Sampler_"+std::to_string(nThisSampler)+"_"+_screenScoringPlaneName);
  _screenSamplerName2 = ("Sampler_"+std::to_string(nThisSampler)+"_"+_screenScoringPlaneName+"_2");

  //Build and place the volume...
  itsScreenScoringPlaneSolid = new G4Box("ScreenScoringPlaneSolid",_screenWidth/2.0,_screenHeight/2.0,_scoringPlaneThickness/2.0);
  itsScreenScoringPlaneLog = new G4LogicalVolume(itsScreenScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"ScreenScoringPlaneLog",0,0,0);
  itsScreenScoringPlaneLog->SetVisAttributes(_visAttSampler);
  itsScreenScoringPlaneLog2 = new G4LogicalVolume(itsScreenScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"ScreenScoringPlaneLog2",0,0,0);
  itsScreenScoringPlaneLog2->SetVisAttributes(_visAttSampler);
  G4double dispX=0;
  G4double dispY=0;
  G4double dispZ=2*std::cos(std::abs(_screenAngle))*(_screenThickness/2.0+_scoringPlaneThickness/2.0)-_cameraScreenDist/2.0;
  G4double dispZ2=-2*std::cos(std::abs(_screenAngle))*(_screenThickness/2.0+_scoringPlaneThickness/2.0)-_cameraScreenDist/2.0;
  G4int samplerID1 = BDSSamplerRegistry::Instance()->RegisterSampler(_screenSamplerName,
                                                                     nullptr);
  G4int samplerID2 = BDSSamplerRegistry::Instance()->RegisterSampler(_screenSamplerName2,
                                                                     nullptr);
  new G4PVPlacement(_screenRotationMatrix,
                    G4ThreeVector(dispX,dispY,dispZ),
                    itsScreenScoringPlaneLog,
                    _screenSamplerName,
                    containerLogicalVolume,
                    false,
                    samplerID1,
                    BDSGlobalConstants::Instance()->CheckOverlaps());

  new G4PVPlacement(_screenRotationMatrix,
                    G4ThreeVector(dispX,dispY,dispZ2),
                    itsScreenScoringPlaneLog2,
                    _screenSamplerName2,
                    containerLogicalVolume,
                    false,
                    samplerID2,
                    BDSGlobalConstants::Instance()->CheckOverlaps());

  //--
  itsScreenScoringPlaneLog->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());
  //-----------
  itsScreenScoringPlaneLog2->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());

  itsScreenScoringPlaneLog->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());
}

void BDSAwakeSpectrometer::Build()
{
  SetVisAttributes();
  BuildScreen();
  BuildCamera();
  CalculateLengths();
  //  BuildFrame();
  BuildContainerLogicalVolume();
  //      BuildScreenScoringPlane();
  //      BuildCameraScoringPlane();
  //  PlaceFrame();
   PlaceScreen();
  //      PlaceCamera();
  //      }
  RegisterSensitiveVolume(containerLogicalVolume, BDSSDType::energydep);
  switch(_magnetGeometryType){
  case 0:
    break;
  case 1:
    BuildMagnet();
    PlaceMagnet();
    break;
  default:
    G4String exceptionString = (G4String)"magnetGeometryType: " + _magnetGeometryType + (G4String)" unknown.";
    G4Exception(exceptionString.c_str(), "-1", FatalErrorInArgument, "");
  }
  BuildVacuumChamber();
  PlaceVacuumChamber();
  BuildField();
}

void BDSAwakeSpectrometer::PlaceMagnet(){
  PlaceYoke();
  PlaceCoils();
}

void BDSAwakeSpectrometer::BuildCamera(){
  _camera=new BDSCCDCamera();
}
void BDSAwakeSpectrometer::PlaceCamera(){
  _camera->phys(new G4PVPlacement(_screenRotationMatrix,
                                  G4ThreeVector(-1*_cameraScreenDist*sin(_screenAngle),0,1*_cameraScreenDist*cos(_screenAngle)),
                                  _camera->log(),
                                  _camera->name()+"_phys",
                                  containerLogicalVolume,
                                  false,
                                  0,
                                  true)
                );
}

//Frame for the vacuum window.
void BDSAwakeSpectrometer::BuildFrame(){
    G4ThreeVector frameSize(_frameWidth,_frameHeight, _frameThicknessZ);
    G4TwoVector windowSize(_screenWidth, _screenHeight);
    G4double relativeWindowOffsetX = -0.5*(frameSize.x()-windowSize.x())+_windowOffsetXFromVCEdge/std::cos(_screenAngle);
    G4TwoVector windowPos(relativeWindowOffsetX,0);
    _frame = new BDSScreenFrameRectangular((G4String)"asframe",frameSize, windowSize, windowPos, BDSMaterials::Instance()->GetMaterial("G4_STAINLESS-STEEL"));
}

void BDSAwakeSpectrometer::PlaceFrame(){
    G4ThreeVector framePos(_frameCentreX,0,_frameCentreZ);
    _frame->Place(_screenRotationMatrix, framePos, containerLogicalVolume);
}

void BDSAwakeSpectrometer::BuildScreen()
{

  G4cout << "Building BDSAwakeMultilayerScreen...." << G4endl;
  _mlScreen = new BDSAwakeMultilayerScreen(_material,
                                           _thickness,
                                           _windowScreenGap,
                                           _screenPSize,
                                           _windowThickness,
                                           _windowMaterial,
                                           _mountThickness,
                                           _mountMaterial,
                                           _screenWidth,
                                                                                                                                                                         _screenHeight);

#ifdef BDSDEBUG
  G4cout << "finished." << G4endl;
  G4cout << "BDSAwakeSpectrometer: finished building screen" << G4endl;
#endif
}

void BDSAwakeSpectrometer::PlaceScreen(){
    G4double lenSaf = 100*CLHEP::um;
                        //BDSGlobalConstants::Instance()->LengthSafety();
    //G4RotationMatrix* zeroRot = new G4RotationMatrix(0,0,0);
    //Place inside the frame cavity
   // G4double screenZOffInFrame = -0.5*_frameThicknessZ+0.5*_mlScreen->GetSize().z()+10*lenSaf;
         G4ThreeVector screenPosition(_screenCentreX+lenSaf*std::tan(_screenAngle), 0, _screenCentreZ+lenSaf);
    _mlScreen->Place(_screenRotationMatrix, screenPosition, containerLogicalVolume);
}

void BDSAwakeSpectrometer::CalculateLengths(){
#ifdef BDSDEBUG
  std::cout << __METHOD_NAME__ << std::endl;
#endif
  //TODO BDSAcceleratorComponent::CalculateLengths();
  //-------

  std::cout << "... got screen dimensions... " << std::endl;
  //The scoring plane...
  _scoringPlaneThickness=1*CLHEP::um;
  _screenThickness = _mlScreen->GetSize().z();
  //  G4double z_wid = _screenWidth * std::sin(std::abs(90*CLHEP::pi/180.0-_screenAngle));//Length due to the screen width and angle
  //  G4double z_thi = _totalThickness * std::cos(std::abs(90*CLHEP::pi/180.0-_screenAngle));//Length due to the screen thickness
  G4double z_wid = _screenWidth * std::cos(_screenAngle);//Length due to the screen width and angle
  G4double z_thi = _screenThickness * std::sin(_screenAngle);//Length due to the screen thickness
  _screen_z_dim = z_wid+z_thi;
  G4double x_wid = _screenWidth * std::sin(_screenAngle);//Length due to the screen width and angle
  G4double x_thi = _screenThickness * std::cos(_screenAngle);//Length due to the screen thickness
  _screen_x_dim = x_wid+x_thi;

//Vacuum frame dimensions

    _frameThicknessX=48.5*CLHEP::mm;
    _frameHeight=174.6*CLHEP::mm;
    _frameThicknessZ=43*CLHEP::mm;
    _frameWidth=_screenWidth+_windowOffsetXFromVCEdge/std::cos(_screenAngle)+_frameThicknessX;
    //Get the rotated frame dimensions

    _frame_z_dim=_frameWidth*std::cos(_screenAngle)+_frameThicknessZ*std::sin(_screenAngle);
    _frame_x_dim=_frameWidth*std::sin(_screenAngle)+_frameThicknessZ*std::cos(_screenAngle);

  _startZPos = -chordLength/2.0;
  //Pole position
  _poleStartZ += _startZPos;
  //Screen position
  _screenEndZ += _poleStartZ;

//Screen position
  //G4double lenSaf = BDSGlobalConstants::Instance()->LengthSafety();
          _screenCentreZ = _screenEndZ -_screen_z_dim/2.0;
          _screenCentreX = _screen_x_dim/2.0 + _vacInnerWidth/2.0 + _windowOffsetXFromVCEdge;
        //Offset the screen due to the _windowOffsetX

    //Frame position
    _frameCentreZ = _screenEndZ -_frame_z_dim/2.0;
    _frameCentreX = _frame_x_dim/2.0 + _vacInnerWidth/2.0 + _vacThickness;

    G4cout << "Screen position at: " << _screenCentreX << " in x and " << _screenCentreZ << " in z. " << G4endl;

  /*
  itsXLength = itsYLength = BDSGlobalConstants::Instance()->ComponentBoxSize()/2;
  itsXLength = std::max(itsXLength, this->GetTunnelRadius()+2*std::abs(this->GetTunnelOffsetX()) + BDSGlobalConstants::Instance()->TunnelThickness()+BDSGlobalConstants::Instance()->TunnelSoilThickness() + 4*BDSGlobalConstants::Instance()->LengthSafety() );
  itsYLength = std::max(itsYLength, this->GetTunnelRadius()+2*std::abs(BDSGlobalConstants::Instance()->TunnelOffsetY()) + BDSGlobalConstants::Instance()->TunnelThickness()+BDSGlobalConstants::Instance()->TunnelSoilThickness()+4*BDSGlobalConstants::Instance()->LengthSafety() );
  */

  _cameraScreenDist=(4.0)*CLHEP::m;
  //  _cameraScreenDist=4*213*CLHEP::mm;



  //  G4double thi=_totalThickness+2*_cameraScreenDist+2*_camera->size().z()+2*_scoringPlaneThickness;

  MagnetDefaults();


  //  itsXLength = (_screen_x_dim + 2*_vacWidth1)+2*_cameraScreenDist;
  //  itsXLength = std::max(2*(std::abs(_screenCentreX)+_screen_x_dim), itsYokeSize.x()+2*std::abs(itsPolePos.x()));
  //  itsYLength = std::max(std::max(_screenHeight,_camera->size().y()),itsYokeSize.y());
  //  itsYLength = std::max(itsYLength,50*CLHEP::cm);
  //  std::cout << __METHOD_NAME__ << " " << chordLength << " " << itsXLength << " " << itsYLength << std::endl;

  _vacDispZ2=(-chordLength)/2.0+(_vacWidth2)/2.0;


  _vacLength=chordLength;


  std::cout << __METHOD_END__ << std::endl;
}

void BDSAwakeSpectrometer::BuildContainerLogicalVolume()
{
  containerSolid=new G4Box( name+"_marker_solid",
                            BDSGlobalConstants::Instance()->TunnelInfo()->aper1/2, // size to be fixed
                            BDSGlobalConstants::Instance()->TunnelInfo()->aper2/2,
                            chordLength/2.0); //z half length

  containerLogicalVolume=new G4LogicalVolume(containerSolid,
                                             BDSMaterials::Instance()->GetMaterial("vacuum"),
                                             name+"_marker_log");
  G4VisAttributes* visAtt = new G4VisAttributes(G4Color(0,1,0));
  visAtt->SetForceWireframe(true);
  visAtt->SetVisibility(true);
  containerLogicalVolume->SetVisAttributes(visAtt);

  containerLogicalVolume->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());
  _logVols.push_back(containerLogicalVolume);

}

void BDSAwakeSpectrometer::SetUserLimits()
{
    auto defaultUserLimits = BDSGlobalConstants::Instance()->DefaultUserLimits();
    for(unsigned long i=0; i<_logVols.size(); i++) {
        _logVols.at(i)->SetUserLimits(defaultUserLimits);
    }
}

BDSAwakeSpectrometer::~BDSAwakeSpectrometer()
{
  delete _mlScreen;
  delete _camera;
  delete _vacRotationMatrix;
}
