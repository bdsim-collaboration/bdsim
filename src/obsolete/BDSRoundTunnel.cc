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
#include "BDSRoundTunnel.hh"
#include "BDSGlobalConstants.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "BDSDebug.hh"


BDSRoundTunnel::BDSRoundTunnel(Element element, G4double length, G4double angle) :BDSTunnel(element, length, angle)
{
  G4cout << __METHOD_NAME__ << " - _radius = " <<radius() << G4endl;
  CalculateDimensions();
}

BDSRoundTunnel::~BDSRoundTunnel(){
}

void BDSRoundTunnel::CalculateDimensions(){
  offsetY(_beamlineCeilingHeight-radius());
  _trans.setX(offsetX());
  _trans.setY(offsetY());
  _trans.setZ(0);
  //Size of a block used in geometry construction.
  G4cout << __METHOD_NAME__ << " - _radius = " << radius() << G4endl;
  _blockSize = 4*radius();
  G4cout << __METHOD_NAME__ << " - _blockSize = " << _blockSize << G4endl;
  //A vector used for construction the tunnel floor.
  _floorOffset = G4ThreeVector(0,-_blockSize+radius()-_beamlineCeilingHeight-_floorBeamlineHeight,0);
}

void BDSRoundTunnel::BuildStraightSolids(){

  _tunnelSizedBlock = new G4Box(
				_motherName+"_bnd_sized_block_solid", 
				_blockSize,
				_blockSize,
				_length-BDSGlobalConstants::Instance()->GetLengthSafety()
				);
  
  _solid=new G4Tubs(_motherName+"_tun_solid",
			  0,
			  radius()+thickness(),
			  (_length-BDSGlobalConstants::Instance()->GetLengthSafety())/2.0,
			  0,CLHEP::twopi*CLHEP::radian);
  
  _soilSolid=new G4Tubs(_motherName+"_tun_soil_solid",
			radius()+thickness()+BDSGlobalConstants::Instance()->GetLengthSafety(),
			radius()+thickness()+BDSGlobalConstants::Instance()->GetLengthSafety()+soilThickness(),	
			(_length-BDSGlobalConstants::Instance()->GetLengthSafety())/2.0,
			0,CLHEP::twopi*CLHEP::radian);
  
  _innerSolid=new G4Tubs(_motherName+"_inner_tun_solid",
			       0.,
			       radius(),
			       (_length-BDSGlobalConstants::Instance()->GetLengthSafety())/2,
			       0,CLHEP::twopi*CLHEP::radian);
  
  _largerInnerSolid=new G4Tubs(_motherName+"_inner_tun_solid",
				     0.,
				     radius()+BDSGlobalConstants::Instance()->GetLengthSafety(),
				     (_length-BDSGlobalConstants::Instance()->GetLengthSafety()),
				     0,CLHEP::twopi*CLHEP::radian);
  
  
  
  
  _cavity = new G4SubtractionSolid(
                                         _motherName +"_tun_cavity_solid", 
                                         _innerSolid,
                                         _tunnelSizedBlock,
                                         _nullRotationMatrix,
                                         _floorOffset
                                         );
  
  _largerCavity = new G4SubtractionSolid(
					       _motherName +"_tun_cavity_solid", 
					       _largerInnerSolid,
					       _tunnelSizedBlock,
					       _nullRotationMatrix,
					       _floorOffset
					       );
  
}

void BDSRoundTunnel::BuildAngleSolids(){
  G4double pi_ov_2 = asin(1.);
  _rot->rotateY(pi_ov_2);
  
  G4double xHalfLengthPlus, xHalfLengthMinus, tunHalfLen;
  xHalfLengthMinus = (_length/_angle)*sin(_angle/2)
    - fabs(cos(_angle/2)) * (radius() + thickness()) * tan(_angle/2)/2;
  
  xHalfLengthPlus = (_length/_angle)*sin(_angle/2)
    + fabs(cos(_angle/2)) * (radius() + thickness()) * tan(_angle/2)/2;
  
  tunHalfLen = std::max(xHalfLengthPlus,xHalfLengthMinus);
  _trans.setZ(0);
  _trans.setX(0);
  _trans.setY(_offsetY);
  
  
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Building a block" << G4endl;
#endif
    _tunnelSizedBlock = new G4Box(
                                    _motherName+"_bnd_sized_block_solid", 
                                    _blockSize,
                                    _blockSize,
                                    tunHalfLen
				    );

    
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "Building a tunnel solid" << G4endl;
#endif

    _solid = new G4IntersectionSolid(
				     _motherName+"_tun_solid",
				     new G4Tubs(
						_motherName+"_temp_tun_solid",
						0,
						radius()+thickness(),
						tunHalfLen,
						0,CLHEP::twopi*CLHEP::radian),			    
				     _motherVolume->GetSolid(),
				     BDSGlobalConstants::Instance()->RotYM90(),
				     _nullThreeVector
				     ); 
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "Building a soil tunnel solid" << G4endl;
#endif
    _soilSolid = new G4IntersectionSolid(
                                           _motherName+"_soil_solid",
                                           new G4Tubs(
                                                      _motherName+"_temp_soil_solid",
                                                      radius()+thickness()+BDSGlobalConstants::Instance()->GetLengthSafety(),
                                                      radius()+thickness()+BDSGlobalConstants::Instance()->GetLengthSafety()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness(),
                                                      tunHalfLen,
                                                      0,CLHEP::twopi*CLHEP::radian),
                                           _motherVolume->GetSolid(),
                                           BDSGlobalConstants::Instance()->RotYM90(),
                                           _nullThreeVector
					 ); 
#ifdef BDSDEBUG   
    G4cout << __METHOD_NAME__ << "Building inner tunnel solid" << G4endl;
#endif
    _innerSolid=new G4IntersectionSolid(
                                                _motherName+"_inner_tun_solid",
                                                new G4Tubs(_motherName+"_temp_inner_tun_solid",
                                                           0.,
                                                           radius(),
                                                           tunHalfLen,
                                                           0,CLHEP::twopi*CLHEP::radian),
                                                _motherVolume->GetSolid(),
                                                BDSGlobalConstants::Instance()->RotYM90(),
                                                _nullThreeVector
                                                ); 
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "Building larger inner tunnel solid" << G4endl;
#endif
    _largerInnerSolid= new G4Tubs(_motherName+"_temp_inner_tun_solid",
                                          0.,
                                          radius()+BDSGlobalConstants::Instance()->GetLengthSafety(),
                                          2*tunHalfLen,
                                          0,CLHEP::twopi*CLHEP::radian);
                                                       
    
    
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "Building tunnel cavity" << G4endl;
#endif
    _cavity = new G4SubtractionSolid(
                                         _motherName +"_tun_cavity_solid", 
                                         _innerSolid,
                                         _tunnelSizedBlock,
                                         _nullRotationMatrix,
                                         _floorOffset
                                         );
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "Building larger tunnel cavity" << G4endl;
#endif
   _largerCavity = new G4SubtractionSolid(
                                         _motherName +"_tun_cavity_solid", 
                                         _largerInnerSolid,
                                         _tunnelSizedBlock,
                                         _nullRotationMatrix,
                                         _floorOffset
                                         );
}

void BDSRoundTunnel::BuildSolidVolumes(){
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  if ( angle() ==0 ){ //Build a straight tunnel using tubes
    BuildStraightSolids();
  } else {
    BuildAngleSolids();
  }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Building tunnel walls" << G4endl;
#endif
  _wallsSolid = new G4SubtractionSolid(
					_motherName+"_tun_walls",
					_solid,
					_largerCavity
					);
#ifdef BDSDEBUG
  G4cout << __METHOD_END__ << G4endl;
#endif
}

void BDSRoundTunnel::SetVisAttributes(){
  BDSTunnel::SetVisAttributes();
  _visAtt1->SetForceSolid(true);
}
