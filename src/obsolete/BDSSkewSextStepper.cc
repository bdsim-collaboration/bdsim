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
#include "G4TransportationManager.hh"

extern G4double BDSLocalRadiusOfCurvature;

BDSSkewSextStepper::BDSSkewSextStepper(G4Mag_EqRhs *EqRhs)
  : G4MagIntegratorStepper(EqRhs,6),  // integrate over 6 variables only !!
                                      // position & velocity
    itsBDblPrime(0.0), itsDist(0.0)
{
  fPtrMagEqOfMot = EqRhs;
}


void BDSSkewSextStepper::AdvanceHelix( const G4double  yIn[],
                                   G4ThreeVector,
				   G4double  h,
				   G4double  ySkewSext[])
{  
  const G4double *pIn = yIn+3;
  G4ThreeVector v0= G4ThreeVector( pIn[0], pIn[1], pIn[2]);  
  G4ThreeVector InitMomDir=v0.unit();

  G4ThreeVector GlobalPosition= G4ThreeVector( yIn[0], yIn[1], yIn[2]);  

  G4double InitMag=v0.mag();

  G4double kappa= - fPtrMagEqOfMot->FCof()*itsBDblPrime/InitMag;

  // This is kappa in (T/m)*m^-2, so convert to standard Geant4 units:
  // it turns out that (1./(m*m))/(tesla/m) =1, so do nothing...!
   
  // relevant momentum scale is p_z, not P_tot:
  // check that the approximations are valid, else do a linear step:
  if(fabs(kappa)<1.e-12)
     {
       G4ThreeVector positionMove  = (h/InitMag) * v0;
       
       ySkewSext[0]   = yIn[0] + positionMove.x(); 
       ySkewSext[1]   = yIn[1] + positionMove.y(); 
       ySkewSext[2]   = yIn[2] + positionMove.z(); 
				
       ySkewSext[3] = v0.x();
       ySkewSext[4] = v0.y();
       ySkewSext[5] = v0.z();

       itsDist=0;
     }
  else 
    { 
      G4Navigator* SkewSextNavigator=
	G4TransportationManager::GetTransportationManager()->
	GetNavigatorForTracking();

      G4AffineTransform LocalAffine=SkewSextNavigator->
	GetLocalToGlobalTransform();

       // gab_dec03>>
      // position 
      // G4ThreeVector LocalR = SkewSextNavigator->GetCurrentLocalCoordinate();
       // position derivative r' (normalised to unity)
      // G4ThreeVector LocalRp= (SkewSextNavigator->ComputeLocalAxis(v0)).unit();
      G4AffineTransform GlobalAffine=SkewSextNavigator->
	GetGlobalToLocalTransform();
      G4ThreeVector LocalR=GlobalAffine.TransformPoint(GlobalPosition); 
      G4ThreeVector LocalRp=GlobalAffine.TransformAxis(InitMomDir);
      // gab_dec03<<
     

       G4double x0= LocalR.x(); 
       G4double y0= LocalR.y(); 

       //       G4double z0=LocalR.z();

       // Evaluate field at the approximate midpoint of the step.
       x0=x0+LocalRp.x()*h/2;
       y0=y0+LocalRp.y()*h/2;

       G4double xp=LocalRp.x();
       G4double yp=LocalRp.y();
       G4double zp=LocalRp.z();


       G4double x02My02=(x0*x0-y0*y0);

       // local r'' (for curvature)
       G4ThreeVector LocalRpp;
       LocalRpp.setX(2*zp*x0*y0);
       LocalRpp.setY(zp*x02My02);
       LocalRpp.setZ(-2*xp*x0*y0-yp*x02My02);

       LocalRpp*=kappa/2; // 2 is actually a 2! factor.;

       // determine effective curvature
      G4double R_1 = LocalRpp.mag();
      if(R_1>0.)
	{
	   G4double h2=h*h;
	   // chord distance (simple quadratic approx)
	   itsDist= h2*R_1/8;

	   // Save for Synchrotron Radiation calculations:
	   BDSLocalRadiusOfCurvature=1./R_1;

	   // central kick approximation
           G4double dx=LocalRp.x()*h + LocalRpp.x()*h2/2; 
	   G4double dy=LocalRp.y()*h + LocalRpp.y()*h2/2;

           G4double dz=sqrt(h2*(1.-h2*R_1*R_1/12)-dx*dx-dy*dy);

	   // check for precision problems
           G4double ScaleFac=(dx*dx+dy*dy+dz*dz)/h2;
    
	   if(ScaleFac>1.0000001)
	     {
               ScaleFac=sqrt(ScaleFac);
               dx/=ScaleFac;
               dy/=ScaleFac;
               dz/=ScaleFac;
	     }

	  
           LocalR.setX(LocalR.x()+dx);
           LocalR.setY(LocalR.y()+dy);
           LocalR.setZ(LocalR.z()+dz);
	 
	   LocalRp  = LocalRp + h*LocalRpp;
	}
       else
	 {LocalR += h*LocalRp;}

      GlobalPosition=LocalAffine.TransformPoint(LocalR); 
      G4ThreeVector GlobalTangent=
	LocalAffine.TransformAxis(LocalRp)* InitMag;
	  
       ySkewSext[0]   = GlobalPosition.x(); 
       ySkewSext[1]   = GlobalPosition.y(); 
       ySkewSext[2]   = GlobalPosition.z(); 
       
       ySkewSext[3] = GlobalTangent.x();
       ySkewSext[4] = GlobalTangent.y();
       ySkewSext[5] = GlobalTangent.z();      
    }
}

void BDSSkewSextStepper::Stepper( const G4double yInput[],
		     const G4double[],
		     const G4double hstep,
		     G4double yOut[],
		     G4double yErr[]      )
{  
   const G4int nvar = 6 ;

   G4int i;
   for(i=0;i<nvar;i++) yErr[i]=0;

   AdvanceHelix(yInput,(G4ThreeVector)0,hstep,yOut);
}

G4double BDSSkewSextStepper::DistChord()   const 
{
return itsDist;
  // This is a class method that gives distance of Mid 
  //  from the Chord between the Initial and Final points.
}

BDSSkewSextStepper::~BDSSkewSextStepper()
{}
