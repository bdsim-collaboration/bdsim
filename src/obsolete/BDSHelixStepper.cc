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
}


void BDSHelixStepper::AdvanceHelix( const G4double  yIn[],
				  G4double  h,
				  G4double  yHelix[])
{
  G4ThreeVector positionMove, endTangent;

  const G4double *pIn = yIn+3;
 
  G4ThreeVector v0= G4ThreeVector( pIn[0], pIn[1], pIn[2]);  
  
  G4ThreeVector GlobalPosition= G4ThreeVector( yIn[0], yIn[1], yIn[2]);  

  G4double InitMag=v0.mag();

  G4ThreeVector LocalR,LocalRp ;

  //G4Navigator* HelixNavigator=
  //  G4TransportationManager::GetTransportationManager()->
  //  GetNavigatorForTracking();


  LocalR=GlobalPosition;
  LocalRp=v0.unit();

  G4ThreeVector itsFinalPoint,itsFinalDir;
  
  G4ThreeVector yhat(0.,1.,0.);

  G4ThreeVector vhat=LocalRp;
  
  G4ThreeVector vnorm=vhat.cross(yhat);
   
  G4double R;

  if(BDSGlobalConstants::Instance()->GetSynchRescale())
    {
      G4double B[3];
      its_EqRhs->GetFieldValue(yIn, B);
      R=-(InitMag/CLHEP::GeV)/(0.299792458 * B[1]/CLHEP::tesla) *CLHEP::m;
    }
  else
    {
      R=-(InitMag/CLHEP::GeV)/(0.299792458 * itsBField/CLHEP::tesla) * CLHEP::m;
    }

 // include the sign of the charge of the particles

  if( its_EqRhs->FCof()<0) R*=-1.;
  else if (its_EqRhs->FCof()==0) R=DBL_MAX;

  // check that the approximations are valid, else do a linear step:
  if(fabs(R)<DBL_MAX) 
    { 
  
      G4double Theta   = h/R;

      G4double CosT_ov_2, SinT_ov_2, CosT, SinT;
      CosT_ov_2=cos(Theta/2);
      SinT_ov_2=sin(Theta/2);

      CosT=(CosT_ov_2*CosT_ov_2)- (SinT_ov_2*SinT_ov_2);
      SinT=2*CosT_ov_2*SinT_ov_2;

      BDSLocalRadiusOfCurvature=R;

      itsDist=fabs(R)*(1.-CosT_ov_2);

      G4ThreeVector dPos=R*(SinT*vhat + (1-CosT)*vnorm);
 	
      itsFinalPoint=LocalR+dPos;
      itsFinalDir=CosT*vhat +SinT*vnorm;


    }
  else
    {
      itsFinalPoint=LocalR + h * LocalRp;
      itsFinalDir=LocalRp;
      itsDist=0.;
    }


  G4ThreeVector GlobalTangent;
  GlobalPosition=itsFinalPoint;
  GlobalTangent=itsFinalDir;
  GlobalTangent*=InitMag;

  yHelix[0] = GlobalPosition.x(); 
  yHelix[1] = GlobalPosition.y(); 
  yHelix[2] = GlobalPosition.z(); 
				
  yHelix[3] = GlobalTangent.x();
  yHelix[4] = GlobalTangent.y();
  yHelix[5] = GlobalTangent.z();
}    


void BDSHelixStepper::Stepper( const G4double yInput[],
			     const G4double[],
			     const G4double hstep,
			     G4double yOut[],
			     G4double yErr[]      )
{  
  const G4int nvar = 6 ;

  for(G4int i=0;i<nvar;i++) yErr[i]=0;
  AdvanceHelix(yInput,hstep,yOut);
}

G4double BDSHelixStepper::DistChord()   const 
{

  return itsDist;
  // This is a class method that gives distance of Mid 
  //  from the Chord between the Initial and Final points.
}

BDSHelixStepper::~BDSHelixStepper()
{}
