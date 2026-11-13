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
/* BDSIM code.    
A scintillator screen.
Work in progress.  
*/

#include "BDSGlobalConstants.hh" 
#include "BDSAwakeScintillatorScreen.hh"
#include "BDSMaterials.hh"
#include "BDSSampler.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSSDSampler.hh"
#include "BDSSDManager.hh"
#include "BDSCCDCamera.hh"
#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "BDSDebug.hh"

#include "BDSAwakeMultilayerScreen.hh"
//#include "UltraFresnelLens.hh"
//#include "UltraFresnelLensParameterisation.hh"

#include "G4Trap.hh"

//============================================================
BDSAwakeScintillatorScreen::BDSAwakeScintillatorScreen (G4String aName,
							G4String materialIn,
							G4double thicknessIn = 0.3 * CLHEP::mm,
							G4double windowScreenGapIn = 0,
							G4double angleIn = -45*CLHEP::pi/180.0,
							G4double windowThicknessIn=0,
							G4String windowMaterialIn=""):
  BDSAcceleratorComponent(aName, 1.0, 0, "awakescintillatorscreen"),
  _mlScreen(nullptr),
  _camera(nullptr),
  _material(materialIn),
  _thickness(thicknessIn),
  _windowScreenGap(windowScreenGapIn),
  _screenAngle(angleIn),
  _windowThickness(windowThicknessIn),
  _windowMaterial(windowMaterialIn)
{
  _vacChambType=2;

  //Set the rotation of the screen
  _screenRotationMatrix = new G4RotationMatrix();
  _screenRotationMatrix->rotateY(_screenAngle);

  _vacRotationMatrix = new G4RotationMatrix();

  RegisterRotationMatrix(_screenRotationMatrix);
  RegisterRotationMatrix(_vacRotationMatrix);
}

void BDSAwakeScintillatorScreen::SetVisAttributes()
{
  G4VisAttributes* itsVisAttributes=new G4VisAttributes(G4Colour(0.3,0.4,0.2));
  itsVisAttributes->SetForceWireframe(true);

  _visAttFront  = new G4VisAttributes(G4Colour(1.0,0.0,0.0,0.5));
  _visAttScint  = new G4VisAttributes(G4Colour(0.0,1.0,0.0,0.5));
  _visAttBase   = new G4VisAttributes(G4Colour(0.3,0.3,0.3,0.5));
  _visAttSampler= new G4VisAttributes(G4Colour(0.2,0.2,0.0,0.5));

  _visAttFront->SetForceSolid(true);
  _visAttScint->SetForceSolid(true);
  _visAttBase->SetForceSolid(true);
  _visAttSampler->SetForceSolid(true);
  _visAttSampler->SetVisibility(true);
}

void BDSAwakeScintillatorScreen::BuildCameraScoringPlane(){
  G4String tmp = "_cameraScoringPlane";
  _scoringPlaneName=name+tmp;
  int nThisSampler= BDSSamplerRegistry::Instance()->NumberOfExistingSamplers() + 1;
  G4String ident="_camera";
  _samplerName = ("Sampler_"+std::to_string(nThisSampler)+"_"+_scoringPlaneName);
  _samplerName2 = ("Sampler_"+std::to_string(nThisSampler)+"_"+_scoringPlaneName+"_2");

  
  //Build and place the volume...
  itsCameraScoringPlaneSolid = new G4Box("CameraScoringPlaneSolid",100*CLHEP::mm/2.0,500*CLHEP::mm/2.0,_scoringPlaneThickness/2.0);

  itsCameraScoringPlaneLog = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial(
          BDSGlobalConstants::Instance()->VacuumMaterial()),"CameraScoringPlaneLog",0,0,0);
  itsCameraScoringPlaneLog->SetVisAttributes(_visAttSampler);

  G4double dispX=_cameraScreenDist-_scoringPlaneThickness/2.0;
  G4double dispY=0;
  G4double dispZ=-_cameraScreenDist/2.0;

  G4RotationMatrix* rotY90 = new G4RotationMatrix();
  rotY90->rotateY(CLHEP::halfpi);
  RegisterRotationMatrix(rotY90);
  
  G4int samplerNameID1 = BDSSamplerRegistry::Instance()->RegisterSampler(_samplerName,
									 nullptr);

  new G4PVPlacement(rotY90,
		    G4ThreeVector(dispX,dispY,dispZ),
		    itsCameraScoringPlaneLog,
		    _samplerName,
		    containerLogicalVolume,
		    false,
		    samplerNameID1,
		    checkOverlaps);
  
  itsCameraScoringPlaneLog2 = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial(
          BDSGlobalConstants::Instance()->VacuumMaterial()),"CameraScoringPlaneLog2",0,0,0);
  itsCameraScoringPlaneLog2->SetVisAttributes(_visAttSampler);

  G4double dispX2=-sin(_screenAngle)*_cameraScreenDist;
  G4double dispY2=0;
  G4double dispZ2=cos(_screenAngle)*_cameraScreenDist-_cameraScreenDist/2.0;

  G4int samplerNameID2 = BDSSamplerRegistry::Instance()->RegisterSampler(_samplerName2,
									 nullptr);
  
  new G4PVPlacement(_screenRotationMatrix,
		    G4ThreeVector(dispX2,dispY2,dispZ2),
		    itsCameraScoringPlaneLog2,
		    _samplerName2,
		    containerLogicalVolume,
		    false,
		    samplerNameID2,
		    checkOverlaps);

  itsCameraScoringPlaneLog->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());
  itsCameraScoringPlaneLog2->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());

  _samplerName3 = ("Sampler_"+std::to_string(nThisSampler)+"_"+_scoringPlaneName+"_3");
  _samplerName4 = ("Sampler_"+std::to_string(nThisSampler)+"_"+_scoringPlaneName+"_4");
  
  //Build and place the volume...
  itsCameraScoringPlaneLog3 = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial(
          BDSGlobalConstants::Instance()->VacuumMaterial()),"CameraScoringPlaneLog3",0,0,0);
  itsCameraScoringPlaneLog3->SetVisAttributes(_visAttSampler);

  G4double dispX3=_cameraScreenDist/2.0-_scoringPlaneThickness/2.0;
  G4double dispY3=0;
  G4double dispZ3=-_cameraScreenDist/2.0;

  new G4PVPlacement(rotY90,
		    G4ThreeVector(dispX3,dispY3,dispZ3),
		    itsCameraScoringPlaneLog3,
		    _samplerName3,
		    containerLogicalVolume,
		    false,
		    0,
		    checkOverlaps);
  
  itsCameraScoringPlaneLog4 = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial(
          BDSGlobalConstants::Instance()->VacuumMaterial()),"CameraScoringPlaneLog4",0,0,0);
  itsCameraScoringPlaneLog4->SetVisAttributes(_visAttSampler);

  G4double dispX4=-sin(_screenAngle)*_cameraScreenDist/2.0;
  G4double dispY4=0;
  G4double dispZ4=cos(_screenAngle)*_cameraScreenDist/2.0-_cameraScreenDist/2.0;

  G4int samplerNameID4 = BDSSamplerRegistry::Instance()->RegisterSampler(_samplerName4,
									 nullptr);
  
  new G4PVPlacement(_screenRotationMatrix,
		    G4ThreeVector(dispX4,dispY4,dispZ4),
		    itsCameraScoringPlaneLog4,
		    _samplerName4,
		    containerLogicalVolume,
		    false,
		    samplerNameID4,
		    checkOverlaps);
  
  itsCameraScoringPlaneLog3->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());
  itsCameraScoringPlaneLog4->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());

  _samplerName5 = ("Sampler_"+std::to_string(nThisSampler)+"_"+_scoringPlaneName+"_5");
  _samplerName6 = ("Sampler_"+std::to_string(nThisSampler)+"_"+_scoringPlaneName+"_6");
  
  //Build and place the volume...
  itsCameraScoringPlaneLog5 = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial(
          BDSGlobalConstants::Instance()->VacuumMaterial()),"CameraScoringPlaneLog5",0,0,0);
  itsCameraScoringPlaneLog5->SetVisAttributes(_visAttSampler);

  G4double dispX5=_cameraScreenDist/4.0-_scoringPlaneThickness/2.0;
  G4double dispY5=0;
  G4double dispZ5=-_cameraScreenDist/2.0;

  G4int samplerNameID5 = BDSSamplerRegistry::Instance()->RegisterSampler(_samplerName5,
									 nullptr);

  new G4PVPlacement(rotY90,
		    G4ThreeVector(dispX5,dispY5,dispZ5),
		    itsCameraScoringPlaneLog5,
		    _samplerName5,
		    containerLogicalVolume,
		    false,
		    samplerNameID5,
		    checkOverlaps);
  
  itsCameraScoringPlaneLog6 = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial(
          BDSGlobalConstants::Instance()->VacuumMaterial()),"CameraScoringPlaneLog6",0,0,0);
  itsCameraScoringPlaneLog6->SetVisAttributes(_visAttSampler);

  G4double dispX6=-sin(_screenAngle)*_cameraScreenDist/4.0;
  G4double dispY6=0;
  G4double dispZ6=cos(_screenAngle)*_cameraScreenDist/4.0-_cameraScreenDist/2.0;

  G4int samplerNameID6 = BDSSamplerRegistry::Instance()->RegisterSampler(_samplerName6,
									 nullptr);

  new G4PVPlacement(_screenRotationMatrix,
		    G4ThreeVector(dispX6,dispY6,dispZ6),
		    itsCameraScoringPlaneLog6,
		    _samplerName6,
		    containerLogicalVolume,
		    false,
		    samplerNameID6,
		    checkOverlaps);
  
  itsCameraScoringPlaneLog5->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());
  itsCameraScoringPlaneLog6->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());

  itsCameraScoringPlaneLog->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());
}

//void BDSAwakeScintillatorScreen::BuildFresnelLens(){
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


void BDSAwakeScintillatorScreen::BuildScreenScoringPlane(){
  G4String tmp = "_screenScoringPlane";
  _screenScoringPlaneName=name+tmp;
  int nThisSampler= BDSSamplerRegistry::Instance()->NumberOfExistingSamplers() + 1;
  G4String ident="_screen";
  _screenSamplerName = ("Sampler_"+std::to_string(nThisSampler)+"_"+_screenScoringPlaneName);
  _screenSamplerName2 = ("Sampler_"+std::to_string(nThisSampler)+"_"+_screenScoringPlaneName+"_2");
  
  //Build and place the volume...
  itsScreenScoringPlaneSolid = new G4Box("ScreenScoringPlaneSolid",_screenWidth/2.0,_screenHeight/2.0,_scoringPlaneThickness/2.0);
  itsScreenScoringPlaneLog = new G4LogicalVolume(itsScreenScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial(
          BDSGlobalConstants::Instance()->VacuumMaterial()),"ScreenScoringPlaneLog",0,0,0);
  itsScreenScoringPlaneLog->SetVisAttributes(_visAttSampler);
  itsScreenScoringPlaneLog2 = new G4LogicalVolume(itsScreenScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial(
          BDSGlobalConstants::Instance()->VacuumMaterial()),"ScreenScoringPlaneLog2",0,0,0);
  itsScreenScoringPlaneLog2->SetVisAttributes(_visAttSampler);
  G4double dispX=0;
  G4double dispY=0;
  G4double dispZ=2*std::cos(std::abs(_screenAngle))*(_screenThickness/2.0+_scoringPlaneThickness/2.0)-_cameraScreenDist/2.0;
  G4double dispZ2=-2*std::cos(std::abs(_screenAngle))*(_screenThickness/2.0+_scoringPlaneThickness/2.0)-_cameraScreenDist/2.0;

  // Register with output - returns ID which is set as copy number and used by SamplerSD
  // for output addressing.
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
		    checkOverlaps);

  new G4PVPlacement(_screenRotationMatrix,
		    G4ThreeVector(dispX,dispY,dispZ2),
		    itsScreenScoringPlaneLog2,
		    _screenSamplerName2,
		    containerLogicalVolume,
		    false,
		    samplerID2,
		    checkOverlaps);
  
  //--
  itsScreenScoringPlaneLog->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());
  //-----------
  itsScreenScoringPlaneLog2->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());
  
  itsScreenScoringPlaneLog->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());
}

void BDSAwakeScintillatorScreen::Build()
{
  SetVisAttributes(); 
  BuildScreen();
  BuildCamera();	//Need the screen and camera for the dimensions.
  ComputeDimensions(); //Need the dimensions in order to build the tunnel.
  BuildContainerLogicalVolume();
  if(_vacChambType==2){
    BuildVacuumChamber2();
  } else {
    BuildVacuumChamber1();
  }

  BuildScreenScoringPlane();
  BuildCameraScoringPlane();
  PlaceScreen();
  //      PlaceCamera();
}

void BDSAwakeScintillatorScreen::BuildCamera(){
  _camera=new BDSCCDCamera();
}
void BDSAwakeScintillatorScreen::PlaceCamera(){
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

void BDSAwakeScintillatorScreen::BuildScreen()
{
  G4cout << "Building BDSAwakeMultilayerScreen...." << G4endl;
  G4double grainSize = 10*1e-6*CLHEP::m;
  _mlScreen = new BDSAwakeMultilayerScreen(_material,_thickness, _windowScreenGap, grainSize, _windowThickness, _windowMaterial, 0.0, "");
  
  G4cout << "BDSAwakeScintillatorScreen: finished building geometry" << G4endl;
}

void BDSAwakeScintillatorScreen::PlaceScreen(){
  double zOffset = 0;//_totalThickness*cos(_screenAngle)/2.0;
  double xOffset = -_totalThickness*sin(_screenAngle)/2.0;
  _mlScreen->Place(_screenRotationMatrix,
		   G4ThreeVector(xOffset,0,-_cameraScreenDist/2.0+zOffset),
		   containerLogicalVolume
		   );
}

void BDSAwakeScintillatorScreen::ComputeDimensions()
{
  //  _cameraScreenDist=(4.0)*CLHEP::m;
  _cameraScreenDist=4*213*CLHEP::mm;

  _screenWidth=_mlScreen->GetSize().x();
  _screenHeight=_mlScreen->GetSize().y();

  //The scoring plane...
  _scoringPlaneThickness=1*CLHEP::um;

  _screenThickness = _mlScreen->GetSize().z();
  
  _totalThickness =  
    _screenThickness + 2*_scoringPlaneThickness;
  
  
  //  G4double thi=_totalThickness+2*_cameraScreenDist+2*_camera->GetSize().z()+2*_scoringPlaneThickness;

  //Compute the marker volume length according to the screen thickness and width.
  G4double z_wid = _screenWidth * std::sin(std::abs(_screenAngle));//Length due to the screen width and angle
  G4double z_thi = _totalThickness * std::cos(std::abs(_screenAngle));//Length due to the screen thickness
  G4double x_wid = _screenWidth * std::cos(std::abs(_screenAngle));//Length due to the screen width and angle
  G4double x_thi = _totalThickness * std::sin(std::abs(_screenAngle));//Length due to the screen thickness
  
  //Vacuum chamber dimensions.
  _vacThickness=6*CLHEP::mm; //MBPS = 2mm
  _vacInnerWidth=7*CLHEP::cm;
  _vacInnerHeight=7*CLHEP::cm;//MBPS = 7cm
  _vacHeight=_vacInnerHeight+2*_vacThickness;
  
  _vacWidth2=x_wid;
  _vacDispX2=-_vacWidth2/4.0;
  
  if(_vacChambType==2){
    _vacKevlarThickness=1e-30;
    _vacMylarThickness=1e-30;
    
  }else{
    _vacKevlarThickness=0.3*CLHEP::mm;
    _vacMylarThickness=0.05*CLHEP::mm;
  }
  
  _vacWidth1=_vacInnerWidth+_vacKevlarThickness+_vacMylarThickness+_vacThickness;
  _vacDispX1=-((sqrt(2.0)/2.0)/2.0)*CLHEP::m-_vacWidth1/2.0-0.15*CLHEP::mm;



  chordLength  = z_wid + z_thi +_cameraScreenDist;  
  itsXLength = (x_wid +x_thi+2*_vacWidth1)+2*_cameraScreenDist;
  itsYLength = std::max(_screenHeight,_camera->size().y());
  itsYLength = std::max(itsYLength,50*CLHEP::cm);
  G4cout << __METHOD_NAME__ << chordLength << " " << itsXLength << " " << itsYLength << std::endl;

  _vacDispZ2=(-chordLength)/2.0+(_vacWidth2)/2.0;
  
  _vacLength=chordLength;
}

void BDSAwakeScintillatorScreen::BuildContainerLogicalVolume()
{
  containerSolid = new G4Box(name+"_marker_solid",
			     itsXLength/2.0,
			     itsYLength/2.0,
			     chordLength/2.0); //z half length 
  
  containerLogicalVolume = new G4LogicalVolume
    (containerSolid, 
     BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->VacuumMaterial()),
     name+"_lv");
  G4VisAttributes* visAtt = new G4VisAttributes(G4Color(0,1,0));
  visAtt->SetForceWireframe(true);
  visAtt->SetVisibility(true);
  containerLogicalVolume->SetVisAttributes(visAtt);
  containerLogicalVolume->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());
}

void BDSAwakeScintillatorScreen::BuildVacuumChamber1(){


  G4VSolid* vacuumOuterSolid = new G4Box("vacuumSolid",_vacWidth1/2.0,
					 _vacHeight/2.0, 
					 _vacLength/2.0);


  G4VSolid* vacuumSolid = new G4Box("vacuumSolid",_vacInnerWidth/2.0, _vacInnerHeight/2.0, _vacLength/2.0);

  G4LogicalVolume* vacuumOuterLog = new G4LogicalVolume(vacuumOuterSolid, BDSMaterials::Instance()->GetMaterial("iron"), "vacuumOuterLog",0,0,0);

  new G4PVPlacement(_vacRotationMatrix, 
		    G4ThreeVector(_vacDispX1,0,0), 
		    vacuumOuterLog, 
		    "awakeScreenOuterVacuumPV", 
		    containerLogicalVolume, 
		    false, 
		    0,
		    checkOverlaps
		    );

  G4LogicalVolume* vacuumLog = new G4LogicalVolume(vacuumSolid, BDSMaterials::Instance()->GetMaterial(
          BDSGlobalConstants::Instance()->VacuumMaterial()), "vacuumLog",0,0,0);

 
  new G4PVPlacement(new G4RotationMatrix(), 
		    G4ThreeVector(_vacThickness/2.0-(_vacMylarThickness+_vacKevlarThickness)/2.0,0,0), 
		    vacuumLog, 
		    "awakeScreenVacuumPV", 
		    vacuumOuterLog, 
		    false, 
		    0,
		    checkOverlaps
		    );
 

  G4VSolid* vacuumWindowSolid = new G4Box("vacuumWindowSolid",(_vacMylarThickness+_vacKevlarThickness)/2.0, _vacInnerHeight/2.0, _vacLength/2.0);
  G4LogicalVolume* vacuumWindowLog = new G4LogicalVolume(vacuumWindowSolid, BDSMaterials::Instance()->GetMaterial(
          BDSGlobalConstants::Instance()->VacuumMaterial()), "vacuumWindowLog",0,0,0);

  G4VSolid* kevlarWindowSolid = new G4Box("kevlarWindowSolid",_vacKevlarThickness/2.0, _vacInnerHeight/2.0, _vacLength/2.0);
  G4LogicalVolume* kevlarWindowLog = new G4LogicalVolume(kevlarWindowSolid, BDSMaterials::Instance()->GetMaterial("G4_KEVLAR"), "kevlarWindowLog",0,0,0);

  G4VSolid* mylarWindowSolid = new G4Box("mylarWindowSolid",_vacMylarThickness/2.0, _vacInnerHeight/2.0, _vacLength/2.0);
  G4LogicalVolume* mylarWindowLog = new G4LogicalVolume(mylarWindowSolid, BDSMaterials::Instance()->GetMaterial("G4_MYLAR"), "mylarWindowLog",0,0,0);

  new G4PVPlacement(nullptr, 
		    G4ThreeVector(_vacWidth1/2.0-(_vacMylarThickness+_vacKevlarThickness)/2.0,0,0), 
		    vacuumWindowLog, 
		    "awakeScreenVacuumWindowPV", 
		    vacuumOuterLog, 
		    false, 
		    0,
		    checkOverlaps
		    );
  
  new G4PVPlacement(nullptr, 
		    G4ThreeVector((_vacKevlarThickness+_vacMylarThickness)/2.0 - _vacKevlarThickness/2.0,0,0), 
		    kevlarWindowLog, 
		    "awakeScreenKevlarVacuumWindowPV", 
		    vacuumWindowLog, 
		    false, 
		    0,
		    checkOverlaps
		    );

  new G4PVPlacement(nullptr, 
		    G4ThreeVector(-(_vacKevlarThickness+_vacMylarThickness)/2.0 + _vacMylarThickness/2.0,0,0), 
		    mylarWindowLog, 
		    "awakeScreenMylarVacuumWindowPV", 
		    vacuumWindowLog, 
		    false, 
		    0,
		    checkOverlaps
		    );

  G4VisAttributes* vacVisAttributes=new G4VisAttributes(G4Colour(0.3,0.0,0.4,0.5));
  vacVisAttributes->SetForceWireframe(true);
  vacVisAttributes->SetVisibility(true);
  vacuumOuterLog->SetVisAttributes(vacVisAttributes);

  G4VisAttributes* winVisAttributes=new G4VisAttributes(G4Colour(1.0,0.0,0.0,0.5));
  winVisAttributes->SetForceSolid(true);
  winVisAttributes->SetVisibility(true);
  vacuumWindowLog->SetVisAttributes(winVisAttributes);
}

void BDSAwakeScintillatorScreen::BuildVacuumChamber2()
{
  G4VSolid* vacuumOuterSolid = new G4Trap("vacuumOuterSolid",
					  _vacWidth2/2.0,
					  1.0*(-acos(0.0)+atan(2.0)),
					  0,
					  _vacThickness/2.0,
					  1e-30,1e-30,
					  0,
					  _vacThickness/2.0,
					  _vacWidth2/2.0, _vacWidth2/2.0,
					  0);

  G4VSolid* vacuumInnerSolid = new G4Trap("vacuumInnerSolid",
					  _vacWidth2/2.0,
					  1.0*(-acos(0.0)+atan(2.0)),
					  0,
					  _vacInnerHeight/2.0,
					  1e-30,1e-30,
					  0,
					  _vacInnerHeight/2.0,
					  _vacWidth2/2.0, _vacWidth2/2.0,
					  0);


  G4LogicalVolume* vacuumOuterLog = new G4LogicalVolume(vacuumOuterSolid, BDSMaterials::Instance()->GetMaterial("iron"), "vacuumOuterLog",0,0,0);

  G4LogicalVolume* vacuumInnerLog = new G4LogicalVolume(vacuumInnerSolid, BDSMaterials::Instance()->GetMaterial(
          BDSGlobalConstants::Instance()->VacuumMaterial()), "vacuumInnerLog",0,0,0);

  _vacRotationMatrix->rotateY(CLHEP::pi);

  new G4PVPlacement(_vacRotationMatrix, 
		    G4ThreeVector(_vacDispX2,(_vacHeight+_vacThickness)/2.0,_vacDispZ2), 
		    vacuumOuterLog, 
		    "awakeScreenUpperVacuumPV", 
		    containerLogicalVolume, 
		    false, 
		    0,
		    checkOverlaps
		    );

  new G4PVPlacement(_vacRotationMatrix, 
		    G4ThreeVector(_vacDispX2,-(_vacHeight+_vacThickness)/2.0,_vacDispZ2), 
		    vacuumOuterLog, 
		    "awakeScreenLowerVacuumPV", 
		    containerLogicalVolume, 
		    false, 
		    0,
		    checkOverlaps
		    );

  new G4PVPlacement(_vacRotationMatrix, 
		    G4ThreeVector(_vacDispX2,0,_vacDispZ2), 
		    vacuumInnerLog, 
		    "awakeScreenInnerVacuumPV", 
		    containerLogicalVolume, 
		    false, 
		    0,
		    checkOverlaps
		    );


  G4VisAttributes* vacVisAttributes=new G4VisAttributes(G4Colour(0.3,0.0,0.4,0.5));
  vacVisAttributes->SetForceWireframe(true);
  vacVisAttributes->SetVisibility(true);
  vacuumOuterLog->SetVisAttributes(vacVisAttributes);
  vacuumInnerLog->SetVisAttributes(vacVisAttributes);

}


BDSAwakeScintillatorScreen::~BDSAwakeScintillatorScreen()
{
  delete _mlScreen;
  delete _camera;
}
