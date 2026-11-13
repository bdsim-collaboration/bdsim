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
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "BDSTunnel.hh"
#include "BDSMaterials.hh"
#include "BDSGlobalConstants.hh"
#include "BDSDebug.hh"
#include <sstream>
#include <float.h>


void BDSTunnel::Defaults(){
  _angle=0;
  _length=0;//Length and angle will be set according to the accelerator component.
  _material = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetTunnelMaterialName());
  _soilMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetSoilMaterialName());
  radius(BDSGlobalConstants::Instance()->GetTunnelRadius());
  _floorBeamlineHeight = BDSGlobalConstants::Instance()->GetFloorBeamlineHeight();

  _beamlineCeilingHeight = BDSGlobalConstants::Instance()->GetBeamlineCeilingHeight();
  _offsetX = BDSGlobalConstants::Instance()->GetTunnelOffsetX()*1000;
  _thickness = BDSGlobalConstants::Instance()->GetTunnelThickness();
  _soilThickness = BDSGlobalConstants::Instance()->GetTunnelSoilThickness();
  _rot = new G4RotationMatrix(0,0,0);
  _trans.set(0,0,0);
  _nullThreeVector.set(0,0,0);
  _nullRotationMatrix = new G4RotationMatrix(0,0,0);
  _bBuildTunnel = true;
}


BDSTunnel::BDSTunnel(Element element, G4double length, G4double angle){//, BDSAcceleratorComponent* component){
  Defaults();
  _length = length;
  _angle = angle;
  SetParameters(element);
//  SetParameters(component);
}


void BDSTunnel::SetParameters(Element element){
 //Set tunnel parameters to parser parameters if they have been set by the parser (i.e. not DBL_MAX).
  if (element.tunnelRadius != DBL_MAX){
    radius(element.tunnelRadius);
  }
  if (element.floorBeamlineHeight != DBL_MAX){
    floorBeamlineHeight(element.floorBeamlineHeight);
  }
  if (element.beamlineCeilingHeight != DBL_MAX){
    beamlineCeilingHeight(element.beamlineCeilingHeight);
  }

  if (element.tunnelOffsetX != DBL_MAX){
    offsetX(element.tunnelOffsetX*1000);
  }
  if (element.tunnelThickness != DBL_MAX){
    thickness(element.tunnelThickness);
  }
  if (element.tunnelSoilThickness != DBL_MAX){
    soilThickness(element.tunnelSoilThickness);
  }
}
/*
void BDSTunnel::SetParameters(BDSAcceleratorComponent* element){
  //Set parameters which are common to both tunnel and accelerator component.
  length(element->GetLength());
  angle(element->GetAngle());
}
*/
double BDSTunnel::radius() const{
  return _radius;
}

void BDSTunnel::radius(G4double val){
  _radius = val;
}


BDSTunnel::BDSTunnel(){
}

BDSTunnel::~BDSTunnel(){
}

/*
void BDSTunnel::motherComponent(BDSAcceleratorComponent* val){
  _motherComponent = val;
  _motherVolume=_motherComponent->GetLogicalVolume();
  _motherName=_motherComponent->GetName();;
}
*/

void BDSTunnel::motherVolume(G4LogicalVolume* val){
  //  _motherComponent = val;
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << " - setting mother volume to " << val->GetName() << G4endl;
#endif
  _motherVolume=val;
  _motherName=_motherVolume->GetName();
#ifdef BDSDEBUG 
  G4cout << __METHOD_END__ << G4endl;
#endif
}


//G4Exception("Error: length of component less than safety length", "-1", FatalErrorInArgument, "");

void BDSTunnel::print(){
  //Print debug statements.
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << "Soil :"
         << " r= " << (radius()+thickness())/CLHEP::m + soilThickness()/CLHEP::m<< " m"
         << " l= " << length()/CLHEP::m << " m"
         << " material = " << _soilMaterial->GetName() 
         << G4endl;
  G4cout << "Outer tunnel :"
         << " r= " << (radius()+thickness())/CLHEP::m << " m"
         << " l= " << length()/CLHEP::m << " m"
         << " material = " << _material->GetName() 
         << G4endl;
  G4cout << "Inner tunnel :"
         << " r= " << radius()/CLHEP::m << " m"
         << " l= " << length()/CLHEP::m << " m"
         << G4endl;
  G4cout << "Offsetx :"
         << " offsetX= " << offsetX()/CLHEP::m << " m"
         << " offsetY= " << offsetY()/CLHEP::m << " m"
	 << " beamlineCeilingHeight= " << _beamlineCeilingHeight/CLHEP::m << " m"
	 << " floorBeamlineHeight= " << _floorBeamlineHeight/CLHEP::m << " m"
         << G4endl;
}

//Check for exceptions (use before building).
void BDSTunnel::CheckExceptions(){
  std::stringstream ss;
  //Make sure length is not too short....
  if(_length <= BDSGlobalConstants::Instance()->GetLengthSafety()){
    ss.str("");
    ss << __METHOD_NAME__ << " - length, " << _length << ", less than or equal to safety length, " << BDSGlobalConstants::Instance()->GetLengthSafety() << ". Not building.";
    G4Exception(ss.str().c_str(), "-1", JustWarning, "");
    _bBuildTunnel=false;
  } else { //...if not, check for other exceptions...
    //Make sure mother volume is set.
    if(_motherVolume==NULL){
      ss.str("");
      ss << "BDSTunnel - mother volume not set.";
      G4Exception(ss.str().c_str(), "-1", FatalException, "");
    }
    /*
    if(_motherComponent==NULL){
      ss.str("");
      ss << "BDSTunnel - mother accelerator component not set.";
      G4Exception(ss.str().c_str(), "-1", FatalException, "");
    }
    */
    //Make sure tunnel radius has been set.
    if(_radius == DBL_MAX){
      ss.str("");
      ss << "BDSTunnel - radius, " << _radius << ", has not been set.";
      G4Exception(ss.str().c_str(), "-1", FatalErrorInArgument, "");
    }
    
    //Make sure values are sensible.
    if((_floorBeamlineHeight == DBL_MAX) && (_beamlineCeilingHeight == DBL_MAX)){ //If both unset...
      ss.str("");
      ss << "BDSTunnel - floorBeamlineHeight and beamlineCeilingHeight not set. Setting defaults.";
      G4Exception(ss.str().c_str(), "-1", JustWarning, "");
      _floorBeamlineHeight = _radius;  //...set them to radius....
      _beamlineCeilingHeight = _radius;
    } else if((_floorBeamlineHeight == DBL_MAX) || (_beamlineCeilingHeight == DBL_MAX)){ //...else if one is unset....
      if(_floorBeamlineHeight == DBL_MAX){
	ss.str("");
	ss << "BDSTunnel - floorBeamlineHeight not set. Setting default.";
	G4Exception(ss.str().c_str(), "-1", JustWarning, "");
	_floorBeamlineHeight = 2*_radius - _beamlineCeilingHeight;  //...set the appropriate one.
      } 
      if(_beamlineCeilingHeight == DBL_MAX){
	ss.str("");
	ss << "BDSTunnel - beamlineCeilingHeight not set. Setting default.";
	G4Exception(ss.str().c_str(), "-1", JustWarning, "");
	_beamlineCeilingHeight = 2*_radius - _floorBeamlineHeight;
      }
    }
    
    if(_offsetX == DBL_MAX){
      _offsetX = 0;
      ss.str("");
      ss << "BDSTunnel - offsetX not set. Setting default.";
      G4Exception(ss.str().c_str(), "-1", JustWarning, "");
    }
    if(_thickness == DBL_MAX){
      ss.str("");
      ss << "BDSTunnel - thickness not set. Setting default.";
      G4Exception(ss.str().c_str(), "-1", JustWarning, "");
      _thickness = 0.5*CLHEP::m;
    }
    if(_soilThickness == DBL_MAX){
      ss.str("");
      ss << "BDSTunnel - soilThickness not set. Setting default.";
      G4Exception(ss.str().c_str(), "-1", JustWarning, "");
      _soilThickness = 5*CLHEP::m;
    }
  }
#ifdef BDSDEBUG
  G4cout << __METHOD_END__ << G4endl;
#endif
}

void BDSTunnel::BuildSolidVolumes(){
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
}

void BDSTunnel::CalculateDimensions(){
}

void BDSTunnel::Build(){
  CheckExceptions();
#ifdef BDSDEBUG
  print();
#endif
  if(_bBuildTunnel){
  BuildSolidVolumes();
  BuildLogicalVolumes(); 
  PlaceLogicalVolumes();
  SetUserLimits();
  SetVisAttributes();
  }
}

void BDSTunnel::BuildLogicalVolumes(){
#ifdef BDSDEBUG
  G4cout << "Building tunnel walls logical volume" << G4endl;
#endif
  
  _wallsLogicalVolume=	
    new G4LogicalVolume(_wallsSolid,
                        _material,
                        _motherName+"_tun_walls_log");
  
#ifdef BDSDEBUG
  G4cout << "Building soil tunnel logical volume" << G4endl;
#endif
  _soilLogicalVolume=	
    new G4LogicalVolume(_soilSolid,
                        _soilMaterial,
                        _motherName+"_tun_soil_log");
}

void BDSTunnel::PlaceLogicalVolumes(){

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " - placing tunnel walls" << G4endl;
#endif
  _physiComp = new G4PVPlacement(
				 _rot,		     // no rotation
				 _trans,	                     // at (0,0,0)
				 _wallsLogicalVolume,  // its logical volume
				 _motherName+"_tun_phys",	     // its name
				 _motherVolume,    // its mother  volume
				 true,		     // no boolean operation
				 0, BDSGlobalConstants::Instance()->GetCheckOverlaps()
				 );		             // copy number
  
  multiplePhysicalVolumes(_physiComp);
  
  /*
#ifdef BDSDEBUG
  G4cout << "Placing soil around tunnel" << G4endl;
#endif
  _physiCompSoil = new G4PVPlacement(
                                             _rot,		     // no rotation
                                             _trans,	                     // at (0,0,0)
                                             _soilLogicalVolume,  // its logical volume
                                             _motherName+"_tun_phys_soil",	     // its name
                                             _motherVolume,    // its mother  volume
                                             true,		     // no boolean operation
                                             0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		             // copy number
  
  multiplePhysicalVolumes(_physiCompSoil);
  */
#ifdef BDSDEBUG
  G4cout << __METHOD_END__ << G4endl;
#endif
}

void BDSTunnel::SetUserLimits(){
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
#ifndef NOUSERLIMITS
  _userLimits = new G4UserLimits("tunnel cuts");
  _soilUserLimits = new G4UserLimits("tunnel soil cuts");
  G4double tcut = BDSGlobalConstants::Instance()->GetThresholdCutCharged();
  if(tcut>0){
    _userLimits->SetUserMinEkine(tcut);
    _soilUserLimits->SetUserMinEkine(tcut);
  }
  _userLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  _soilUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());


  _userLimits->SetMaxAllowedStep(_length);
  _soilUserLimits->SetMaxAllowedStep(_length);

  
  _wallsLogicalVolume->SetUserLimits(_userLimits);
  _soilLogicalVolume->SetUserLimits(_soilUserLimits);

#endif
#ifdef BDSDEBUG
  G4cout << __METHOD_END__ << G4endl;
#endif
}

void BDSTunnel::SetVisAttributes(){
#ifdef BDSDEBUG
  G4cout << "Setting vis attributes" << G4endl;
#endif
  //
  // set visualization attributes
  //
  _visAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
  _visAtt->SetVisibility(false);
  _visAtt->SetForceWireframe(true);  
  _soilLogicalVolume->SetVisAttributes(_visAtt);
  _visAtt1 = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));
  _visAtt1->SetVisibility(BDSGlobalConstants::Instance()->GetShowTunnel());
  //  _visAtt1->SetForceSolid(true);
  _visAtt1->SetForceWireframe(true);
  _wallsLogicalVolume->SetVisAttributes(_visAtt1);
  /*
  _visAtt2 = new G4VisAttributes(G4Colour(0.0, 0.5, 0.5));
  _visAtt2->SetVisibility(false);
  _visAtt2->SetForceSolid(true);
  */

}



