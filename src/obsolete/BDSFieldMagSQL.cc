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
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSFieldMagSQL.hh"

#include "G4RotationMatrix.hh"
#include "G4VPhysicalVolume.hh"
#include "G4TouchableHistoryHandle.hh"
#include "G4TouchableHistory.hh"
#include "G4NavigationHistory.hh"

#include <list>
#include <map>

using std::list;

#if 0
BDSFieldMagSQL::BDSFieldMagSQL(const G4String& aFieldFile,
			       G4double aMarkerLength,
			       list<G4String> Quadvol, list<G4double> QuadBgrad,
			       list<G4String> Sextvol, list<G4double> SextBgrad,
			       list<G4String> Octvol, list<G4double> OctBgrad,
			       list<G4String> Fieldvol, list<G4ThreeVector> UniformField):
  ifs(aFieldFile.c_str()),itsMarkerLength(aMarkerLength),FieldFile(aFieldFile),
  itsQuadBgrad(QuadBgrad), itsQuadVol(Quadvol),
  itsSextBgrad(SextBgrad), itsSextVol(Sextvol),
  itsOctBgrad(OctBgrad), itsOctVol(Octvol),
  itsUniformField(UniformField), itsFieldVol(Fieldvol)
{;}
#endif

G4bool BDSFieldMagSQL::GetHasNPoleFields(){return itsHasNPoleFields;}
G4bool BDSFieldMagSQL::GetHasUniformField(){return itsHasUniformField;}
G4bool BDSFieldMagSQL::GetHasFieldMap(){return itsHasFieldMap;}

BDSFieldMagSQL::BDSFieldMagSQL(const G4String& aFieldFile,
			       G4double aMarkerLength,
			       std::map<G4String, G4double> aQuadVolBgrad,
			       std::map<G4String, G4double> aSextVolBgrad,
			       std::map<G4String, G4double> aOctVolBgrad,
			       std::map<G4String, G4ThreeVector> aUniformFieldVolField,
			       G4bool aHasNPoleFields,
			       G4bool aHasUniformField):
  itsHasNPoleFields(aHasNPoleFields),
  itsHasUniformField(aHasUniformField),
  itsHasFieldMap(false),ifs(aFieldFile.c_str()),
  itsMarkerLength(aMarkerLength),
  FieldFile(aFieldFile),
  itsUniformFieldVolField(aUniformFieldVolField),
  itsQuadVolBgrad(aQuadVolBgrad),
  itsSextVolBgrad(aSextVolBgrad),
  itsOctVolBgrad(aOctVolBgrad),
  itsdz(0.0)
{;}

BDSFieldMagSQL::~BDSFieldMagSQL()
{;}

G4ThreeVector BDSFieldMagSQL::GetField(const G4ThreeVector &position,
				       const G4double       /*t*/) const
{
  G4ThreeVector LocalR, LocalB, RLocalR, FieldB, NPoleB;

  auxNavigator->LocateGlobalPointAndSetup(position);
  //  G4TouchableHistory* aTouchable = auxNavigator->CreateTouchableHistory();
  G4TouchableHistoryHandle aTouchable = auxNavigator->CreateTouchableHistoryHandle();
  // const G4AffineTransform GlobalToMarker=aTouchable->GetHistory()->GetTransform(1);
  //  const G4AffineTransform MarkerToGlobal=GlobalToMarker.Inverse();
  RLocalR = position;
  
  if( fabs(RLocalR.z()) > fabs(itsMarkerLength/2) )
  {
      // Outside of mokka region - field should be zero. This is needed
      // because sometimes RKStepper asks for overly large steps (1km)
      return G4ThreeVector();
    }

  G4bool inNPole = false;
  G4bool inField = false;
  G4String VolName = aTouchable->GetVolume()->GetName();

  if(itsHasUniformField || itsHasNPoleFields){
    G4AffineTransform GlobalAffine, LocalAffine;
    GlobalAffine=auxNavigator->GetGlobalToLocalTransform();
    LocalAffine=auxNavigator->GetLocalToGlobalTransform();
    LocalR=GlobalAffine.TransformPoint(position);
    LocalR.setY(-LocalR.y());
    LocalR.setX(-LocalR.x());	  // -ve signs because of Geant Co-ord System
    if(itsHasNPoleFields){
      std::map<G4String, G4double>::const_iterator iter = itsQuadVolBgrad.find(VolName);
      if(iter!=itsQuadVolBgrad.end()){
	NPoleB.setX(iter->second*LocalR.y());
	NPoleB.setY(iter->second*LocalR.x());
      } else {
	iter = itsSextVolBgrad.find(VolName);
	if(iter!=itsSextVolBgrad.end()){
	  NPoleB.setX(LocalR.x()*LocalR.y()*iter->second);
	  NPoleB.setY(-(LocalR.x()*LocalR.x()-LocalR.y()*LocalR.y())*iter->second/2.);
	} else {
	  iter = itsOctVolBgrad.find(VolName);
	  if(iter!=itsOctVolBgrad.end()){
	    NPoleB.setX((3*LocalR.x()*LocalR.x()*LocalR.y() - 
			 LocalR.y()*LocalR.y()*LocalR.y())*iter->second/6.);
	    NPoleB.setY((LocalR.x()*LocalR.x()*LocalR.x() -
			 3*LocalR.x()*LocalR.y()*LocalR.y())*iter->second/6.); // changed formula, factor 3 added. 16/5/14 - JS, to be double checked
	  } 
	}
      }
    }
    if(itsHasUniformField){
      std::map<G4String, G4ThreeVector>::const_iterator iter_u = itsUniformFieldVolField.find(VolName);  
      if(iter_u!=itsUniformFieldVolField.end()){
	FieldB = iter_u->second;
	FieldB = LocalAffine.TransformAxis(FieldB);
	inField=true;
      }
    }
    NPoleB.setZ(0.0);
    NPoleB = LocalAffine.TransformAxis(NPoleB);
    inNPole=true;
  }

  if(itsHasFieldMap){
    if(itsMarkerLength>0) RLocalR.setZ(RLocalR.z()+itsMarkerLength/2);
    else RLocalR.setZ( -(RLocalR.z()+fabs(itsMarkerLength)/2) + fabs(itsMarkerLength));
    G4double tempz = RLocalR.z()/CLHEP::cm;
    if(tempz<0)  //Mokka region resets Z to be positive at starting from one
      //Edge of the region
      {
	// This should NEVER happen. If it does, then the cause is either that
	// the mokka region length is not set properly, or that the BDSRKStepper
	// is asking for a step length greater than the Mokka marker length
	G4cout << "Z position in Mokka region less than 0 - check step lengths!!" << G4endl;
	G4Exception("Quitting BDSIM in BDSFieldMagSQL.cc", "-1", FatalException, "");
      }
    G4double zlow = floor(tempz);
    G4int ilow = (G4int)(zlow);
    G4double zhi = zlow + 1.0;
    if (ilow > (G4int)itsBz.size() ||
	itsBz.size()==0) LocalB = G4ThreeVector(0.,0.,0.);
    else
      {
	// Calculate the field local to MarkerVolume
	// Interpolate the value of the field nearest to the point
	G4double fieldBrr_r = ( (zhi-tempz)*itsBr_over_r[ilow] +
				(tempz-zlow)*itsBr_over_r[ilow+1]);
	// then should divide by (zhi-zlow) but this = 1
	
	G4double fieldBzz = ( (zhi-tempz)*itsBz[ilow] +
			      (tempz-zlow)*itsBz[ilow+1]);
	// then should divide by (zhi-zlow) but this = 1
	LocalB.setX(fieldBrr_r*(RLocalR.x()));
	LocalB.setY(fieldBrr_r*(RLocalR.y()));
	LocalB.setZ(fieldBzz);
	// Now rotate to give BField on Global Reference Frame
	//LocalB.transform(Rotation().inverse());
      }
    //LocalB=G4ThreeVector(0.,0.,0.); //turn Bfield from Solenoid off
  }

  if(inField) LocalB+=FieldB;
  if(inNPole) LocalB+=NPoleB;

#ifdef BDSDEBUG 
  LocalB.rotateY(10e-3); //to track from incoming beamline perspective
  // this needs the be the crossing angle plus any marker rotation applied
  // for IR solenoid case
  G4cout << RLocalR.x()/CLHEP::m << " "<<RLocalR.y()/CLHEP::m << " "<<RLocalR.z()/CLHEP::m << " "<< LocalB.x()/CLHEP::tesla << " " << LocalB.y()/CLHEP::tesla << " " << LocalB.z()/CLHEP::tesla << G4endl;
#endif
  //  delete aTouchable;
  //  aTouchable = nullptr;
  return LocalB;
}


void BDSFieldMagSQL::Prepare(G4VPhysicalVolume* /*referenceVolume*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  if(FieldFile=="")
    {itsHasFieldMap = false;}
  else
    {
      itsHasFieldMap = true;
      G4cout << __METHOD_NAME__ << "creating SQL field map" << G4endl;
    
    if(!ifs)
      {
	G4cerr << __METHOD_NAME__ << "Unable to open Field Map File: " << FieldFile << G4endl;
	G4Exception("Aborting Program", "-1", FatalException, "");
      }
    else
      {G4cout << "Loading SQL Field Map file: " << FieldFile << G4endl;}
    
    if(FieldFile.contains("inverse")) itsMarkerLength*=-1;
    double temp_z=0.0;
    double temp_Bz=0.0;
    double temp_solB=0.0;
    while(!ifs.eof()){
      if(FieldFile.contains("SiD"))
	ifs >> temp_z >> temp_Bz >> temp_solB;
      
      if(FieldFile.contains("LD"))
	ifs >> temp_z >> temp_Bz >> temp_solB >> temp_solB;
      
      if(FieldFile.contains("TESLA"))
	ifs >> temp_z >> temp_Bz;
      
      itsZ.push_back(temp_z*CLHEP::m);
      itsBz.push_back(temp_Bz*CLHEP::tesla);
    }
    
    itsdz = itsZ[1] - itsZ[0];
    
    //first element:
    itsdBz_by_dz.push_back( (itsBz[1] - itsBz[0]) / itsdz );
    itsBr_over_r.push_back(0.5 * itsdBz_by_dz[0] );
    
    for(G4int j=1; j<(G4int)itsBz.size()-2; j++)
      {
	  itsdBz_by_dz.push_back( (itsBz[j+1] - itsBz[j-1]) / (2*itsdz) );
	  itsBr_over_r.push_back(0.5 * itsdBz_by_dz[j] );
	}
      
      //last element:
      itsdBz_by_dz.push_back( (itsBz[itsBz.size()-1] - itsBz[itsBz.size()-2]) / itsdz );
      itsBr_over_r.push_back(0.5 * itsdBz_by_dz[itsdBz_by_dz.size()-1] );
    }

  //const G4RotationMatrix* rot = referenceVolume->GetFrameRotation();
  /*if (rot)
    {SetOriginRotation(*rot);}
  SetOriginTranslation(referenceVolume->GetFrameTranslation());*/
}
