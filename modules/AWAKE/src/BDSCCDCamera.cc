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
#include "BDSGlobalConstants.hh" 
#include "BDSCCDCamera.hh"
#include "BDSCCDPixel.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "BDSDebug.hh"

#include <map>
#include "G4TwoVector.hh"
#include "BDSMaterials.hh"

BDSCCDCamera::BDSCCDCamera ()
{
  _name="ccdCamera";
  defaultDimensions();
  build();
}

void BDSCCDCamera::defaultDimensions(){
  _cavityLength = 40*CLHEP::mm;
  _size.setX(110.7*CLHEP::mm);
  _size.setY(137.2*CLHEP::mm);
  _size.setZ(231.0*CLHEP::mm+_cavityLength);
}

void BDSCCDCamera::build(){
  buildMotherVolume();
  buildObjectLens();
  buildImageLens();
  buildCCDChip();
  buildCavity();
  placeComponents();
}

void BDSCCDCamera::buildMotherVolume(){
  _solid=new G4Box( _name+"_solid",
		    _size.x()/2.0,
		    _size.y()/2.0,
		    _size.z()/2.0);
  
  _log=new G4LogicalVolume
    (_solid, 
     BDSMaterials::Instance()->GetMaterial("G4_POLYSTYRENE"),
     _name+"_log");
  _log->SetVisAttributes(new G4VisAttributes(G4Color(0,1.0,0)));
}

void BDSCCDCamera::buildCavity(){
  //  G4double xSize=std::max(_objectLens->diameter(),_imageLens->diameter());
  //  xSize=std::max(xSize,_ccdChip->size().x());
  //  G4double ySize=std::max(_objectLens->diameter(),_imageLens->diameter());
  //  ySize=std::max(ySize,_ccdChip->size().y());
 
  G4Tubs* cavityTubs = new G4Tubs("CCDCameraCavity_solid",0,_objectLens->diameter()/2.0,_cavityLength/2.0,0,CLHEP::twopi*CLHEP::rad);
  _cavityLog = new G4LogicalVolume(				   
				   cavityTubs,     
				   BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->VacuumMaterial()),
				   _name+"_cavity_log");
  _cavityLog->SetVisAttributes(new G4VisAttributes(G4Color(1.0,0.0,0)));
}

void BDSCCDCamera::placeCavity(){
  G4ThreeVector placementVec;
  placementVec.setX(0);
  placementVec.setY(0);
  placementVec.setZ(-_size.z()/2.0+_cavityLength/2.0);
  new G4PVPlacement(nullptr,
		    placementVec,
		    _cavityLog,
		    _name+"_cavity_phys",
		    _log,
		    false,
		    0,
		    false);
}

void BDSCCDCamera::buildObjectLens(){
  G4double factor =1.0;
  _objectLens = new BDSLens(_name+"ObjectLens",factor*25.4*CLHEP::mm, 1029.8*CLHEP::mm,factor*2.2*CLHEP::mm); //Focal length 1m lens (Thorlabs LB1409-A)
}

void BDSCCDCamera::buildImageLens(){
  //  _imageLens = new BDSLens(_name+"ImageLens",12.7*CLHEP::mm, 14.6*CLHEP::mm, 4.7*CLHEP::mm); //Focal length 15CLHEP::mm lens (Thorlabs LB1092-A) (magnification factor = 66.4)
  _imageLens = new BDSLens(_name+"ImageLens",25.4*CLHEP::mm, 25.5*CLHEP::mm, 9.0*CLHEP::mm); //Focal length 25.4CLHEP::mm lens (Thorlabs LB1761-A)(back focal length 22.2CLHEP::mm)
}
void BDSCCDCamera::buildCCDChip(){
  G4ThreeVector pixelSize;
  G4TwoVector nPixels;

  pixelSize.setX(13.5e-3*CLHEP::mm);
  pixelSize.setY(13.5e-3*CLHEP::mm*512);
  pixelSize.setZ(13.5e-3*CLHEP::mm); //Assume that the pixels are cubes for the moment.
  nPixels.setX(2048);
  nPixels.setY(1);

  _ccdChip = new BDSCCDChip((G4String)(_name+"_CCDChip"), pixelSize, nPixels);
}

void BDSCCDCamera::placeComponents(){
  placeCavity();
  placeObjectLens();
  placeImageLens();
  placeCCDChip();
}

void BDSCCDCamera::placeObjectLens(){
  G4ThreeVector placementVec;
  placementVec.setX(0);
  placementVec.setY(0);
  placementVec.setZ(-_cavityLength/2.0+_objectLens->centreThickness()/2.0);
  new G4PVPlacement(nullptr,
		    placementVec,
		    _objectLens->log(),
		    _objectLens->name()+"_phys",
		    _cavityLog,
		    false,
		    0,
		    false);
}

void BDSCCDCamera::placeImageLens(){
  G4ThreeVector placementVec;
  placementVec.setX(0);
  placementVec.setY(0);
  placementVec.setZ(_cavityLength/2.0-_imageLens->centreThickness()/2.0-22.2*CLHEP::mm);
  new G4PVPlacement(nullptr,
		    placementVec,
		    _imageLens->log(),
		    _imageLens->name()+"_phys",
		    _cavityLog,
		    false,
		    0,
		    false);
}
void BDSCCDCamera::placeCCDChip(){
  G4ThreeVector placementVec;
  placementVec.setX(0);
  placementVec.setY(0);
  placementVec.setZ(_cavityLength/2.0-_ccdChip->size().z()/2.0);
  new G4PVPlacement(nullptr,
		    placementVec,
		    _ccdChip->log(),
		    _ccdChip->name()+"_phys",
		    _cavityLog,
		    false,
		    0,
		    false);
}

BDSCCDCamera::~BDSCCDCamera()
{
  delete _objectLens;
  delete _imageLens;
  delete _ccdChip;

}

