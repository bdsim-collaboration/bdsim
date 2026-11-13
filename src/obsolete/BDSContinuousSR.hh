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
#ifndef BDSCONTINUOUSSR_H
#define BDSCONTINUOUSSR_H

#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"

#include "G4ios.hh" 
#include "globals.hh"
#include "G4VDiscreteProcess.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Field.hh"
#include "G4FieldManager.hh"
#include "G4AffineTransform.hh"
#include "G4NavigationHistory.hh"

class BDSContinuousSR: public G4VDiscreteProcess 
{ 
public:
 
  BDSContinuousSR(const G4String& processName = "contSR");
 
  virtual ~BDSContinuousSR();

  G4bool IsApplicable(const G4ParticleDefinition&);
     
  G4double GetMeanFreePath(const G4Track& track,
			   G4double previousStepSize,
			   G4ForceCondition* condition );
 
  G4VParticleChange *PostStepDoIt(const G4Track& track,         
				  const G4Step&  step);                 

  G4double SynGenC(G4double xmin);
  G4double SynRadC(G4double x);

protected:

private:

  // assignment and copy constructor not implemented nor used
  BDSContinuousSR & operator=(const BDSContinuousSR &right);
  BDSContinuousSR(const BDSContinuousSR&);

  G4double nExpConst;
  G4double CritEngFac;

private:
};

inline G4bool 
BDSContinuousSR::IsApplicable(const G4ParticleDefinition& particle)
{
  return(  (&particle == G4Electron::Electron())
	   ||(&particle == G4Positron::Positron()) );
}

inline G4double 
BDSContinuousSR::GetMeanFreePath(const G4Track& track,
					G4double,
					G4ForceCondition* ForceCondition)
{  
  *ForceCondition = NotForced ;

  G4double rfact = 1.; // mean free path reduction factor 
                        // to reduce fluctuations 

  //   static G4FieldManager* lastFieldManager;

  G4double MeanFreePath;
  G4FieldManager* TheFieldManager=
    track.GetVolume()->GetLogicalVolume()->GetFieldManager();
  
  /*
  G4double SynchOnZPos = (7.184+4.0) * CLHEP::m;
  if(track.GetPosition().z() + BDSGlobalConstants::Instance()->GetWorldSizeZ() < SynchOnZPos)
    return DBL_MAX;
  */
  if(TheFieldManager)
    {
      const G4Field* pField = TheFieldManager->GetDetectorField() ;
      G4ThreeVector  globPosition = track.GetPosition() ;
      G4double  globPosVec[3], FieldValueVec[3]={0.} ;
      globPosVec[0] = globPosition.x() ;
      globPosVec[1] = globPosition.y() ;
      globPosVec[2] = globPosition.z() ;
      
      if(pField)
	pField->GetFieldValue( globPosVec, FieldValueVec ) ; 
	
      G4double Blocal= FieldValueVec[1];
      if ( FieldValueVec[0]!=0.)
	Blocal=sqrt(Blocal*Blocal+FieldValueVec[0]*FieldValueVec[0]); 
 
      if(track.GetMaterial()==BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->VacuumMaterial())
	 && Blocal !=0 )
	{
	  G4ThreeVector InitMag=track.GetMomentum();

	  // transform to the local coordinate frame

	  G4AffineTransform tf(track.GetTouchable()->GetHistory()->GetTopTransform().Inverse());
	  const G4RotationMatrix Rot=tf.NetRotation();
	  const G4ThreeVector Trans=-tf.NetTranslation();
	  InitMag=Rot*InitMag; 


	  G4double Rlocal=(InitMag.z()/CLHEP::GeV)/(0.299792458 * Blocal/CLHEP::tesla) *CLHEP::m;
	  
	  MeanFreePath=
	    fabs(Rlocal)/(track.GetTotalEnergy()*nExpConst);

	  

	 //  G4cout<<"*****************SR*************************"<<G4endl;
// 	  G4cout<<"Track momentum: "<<InitMag<<G4endl;
// 	  G4cout<<"Blocal="<<Blocal/CLHEP::tesla<<"  Rlocal="<<Rlocal/CLHEP::m<<G4endl;
// 	  G4cout<<track.GetVolume()->GetName()<<" mfp="<<MeanFreePath/CLHEP::m<<G4endl;
// 	  G4cout<<"********************************************"<<G4endl;

	  return rfact * MeanFreePath;
	}
      else
	return DBL_MAX;
    }
  else
    return DBL_MAX;
  
}
  


#endif
