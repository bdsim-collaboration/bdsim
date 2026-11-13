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
#include "BDSRK4Stepper.hh"
#include "G4ThreeVector.hh"

//////////////////////////////////////////////////////////////////
//
// Constructor sets the number of variables (default = 6)

extern G4double BDSLocalRadiusOfCurvature;

BDSRK4Stepper::BDSRK4Stepper(G4EquationOfMotion* EqRhs, G4int nvar) :
  G4MagIntegratorStepper(EqRhs,nvar)
{
  itsEqRhs = EqRhs;
  
  unsigned int noVariables= std::max(nvar,8); // For Time .. 7+1
 
  dydxm = new G4double[noVariables];
  dydxt = new G4double[noVariables]; 
  yt    = new G4double[noVariables]; 

  yTemp = new G4double[noVariables];
  yIn = new G4double[noVariables];
}

////////////////////////////////////////////////////////////////
//
// Destructor

BDSRK4Stepper::~BDSRK4Stepper()
{
  delete[] dydxm;
  delete[] dydxt;
  delete[] yt;

  delete[] yTemp;
  delete[] yIn;
}

//////////////////////////////////////////////////////////////////////
//
// Given values for the variables y[0,..,n-1] and their derivatives
// dydx[0,...,n-1] known at x, use the classical 4th Runge-Kutta
// method to advance the solution over an interval h and return the
// incremented variables as yout[0,...,n-1], which not be a distinct
// array from y. The user supplies the routine RightHandSide(x,y,dydx),
// which returns derivatives dydx at x. The source is routine rk4 from
// NRC p. 712-713 .

void
BDSRK4Stepper::AdvanceHelix( const G4double  yIn[],
			     const G4double dydx[],
			     const  G4double  h,
			     G4double  yOut[])
{

#ifdef DEBUG
  G4cout<<"stepping by "<<h<<G4endl;
#endif

  const G4int nvar = this->GetNumberOfVariables();   //  fNumberOfVariables(); 

#ifdef DEBUG
  G4cout<<"nvar="<<nvar<<G4endl;
#endif

  G4int i;
  G4double  hh = h*0.5 , h6 = h/6.0  ;

  // Initialise time to t0, needed when it is not updated by the integration.
  //        [ Note: Only for time dependent fields (usually electric) 
  //                  is it neccessary to integrate the time.] 
  yt[7]   = yIn[7]; 
  yOut[7] = yIn[7];

  // Have to calculate total Energy assuming Mass = zero
  // because have no way to see particle type here (and hence no mass info)
  const G4double *pIn = yIn+3;
  G4double itsMomentum = sqrt(pIn[0]*pIn[0]+pIn[1]*pIn[1]+pIn[2]*pIn[2]);
  
  G4double BField[6];
  G4double Pos[4];
  Pos[0] = yIn[0];
  Pos[1] = yIn[1];
  Pos[2] = yIn[2];
  Pos[3] = 0.;
  itsEqRhs->GetFieldObj()->GetFieldValue(Pos,BField);

  G4ThreeVector BVec = G4ThreeVector(BField[0],
				     BField[1],
				     BField[2]);
  G4ThreeVector pVec = G4ThreeVector(pIn[0],
				     pIn[1],
				     pIn[2]);

  G4double Bmag = (BVec.cross(pVec.unit())).mag();
  
  BDSLocalRadiusOfCurvature = (itsMomentum/CLHEP::GeV) / (0.299792458*Bmag/CLHEP::tesla)*CLHEP::m;

#ifdef DEBUG 
  G4cout<<" Pos = ("<<Pos[0]/CLHEP::mm<<" "<<Pos[1]/CLHEP::mm<<" " <<Pos[2]/CLHEP::mm<<") mm"<<G4endl;
  G4cout<<" Mtm = ("<<pIn[0]/CLHEP::GeV<<" "<<pIn[1]/CLHEP::GeV<<" " <<pIn[2]/CLHEP::GeV<<") GeV"<<G4endl;
  G4cout<<" BField = ("<<BField[0]/CLHEP::tesla<<" "<<BField[1]/CLHEP::tesla<<" "<<BField[2]/CLHEP::tesla<<") T"<<G4endl;
  G4cout<<" Local curvature radius = "<<BDSLocalRadiusOfCurvature/CLHEP::m<<" m"<<G4endl;
#endif


  //
  // Now do the stepping
  //
#ifdef DEBUG 
  G4cout<<"===>RK Steps 1-2,  before, dydx : ";
  for(i=0;i<nvar;i++) { 
    G4cout<<dydx[i]<<" ";
  }  
  G4cout<<G4endl;

  G4cout<<"yIn: ";
  for(i=0;i<nvar;i++) { 
    G4cout<<yIn[i]<<" ";
  }  
  G4cout<<G4endl;
#endif

  for(i=0;i<nvar;i++)
    {
    yt[i] = yIn[i] + hh*dydx[i] ;             // 1st Step K1=h*dydx
  }
  RightHandSide(yt,dydxt) ;                   // 2nd Step K2=h*dydxt

#ifdef DEBUG 
  G4cout<<"after, dydx: ";
  for(i=0;i<nvar;i++) { 
    G4cout<<dydxt[i]<<" ";
  }  
  G4cout<<G4endl;
  
  G4cout<<"after, yt: ";
  for(i=0;i<nvar;i++) { 
    G4cout<<yt[i]<<" ";
  }  
  G4cout<<G4endl;
#endif

#ifdef DEBUG 
  G4cout<<"===>RK Steps 3-4"<<G4endl;
#endif

  for(i=0;i<nvar;i++)
  { 
    yt[i] = yIn[i] + hh*dydxt[i] ;
  }
  RightHandSide(yt,dydxm) ;                   // 3rd Step K3=h*dydxm

  for(i=0;i<nvar;i++)
  {
    yt[i]   = yIn[i] + h*dydxm[i] ;
    dydxm[i] += dydxt[i] ;                    // now dydxm=(K2+K3)/h
  }
  RightHandSide(yt,dydxt) ;                   // 4th Step K4=h*dydxt
 
  for(i=0;i<nvar;i++)    // Final RK4 output
  {
    yOut[i] = yIn[i]+h6*(dydx[i]+dydxt[i]+2.0*dydxm[i]); //+K1/6+K4/6+(K2+K3)/3
  }
  // NormaliseTangentVector( yOut );

 //  G4cout<<"out : ";

//   for(i=0;i<nvar;i++) { 
//      G4cout<<yOut[i]<<" ";
//   }  

//   G4cout<<G4endl;


//  itsDist = 0;
  
  return;

}  // end of DumbStepper ....................................................


void BDSRK4Stepper::Stepper( const G4double yInput[],
			     const G4double dydx[],
			     const G4double hstep,
			     G4double yOut[],
			     G4double yErr[]      )
{  
  const G4int nvar = 6 ;
  G4int i;
  const G4double *pIn = yInput+3;
  G4ThreeVector v0= G4ThreeVector( pIn[0], pIn[1], pIn[2]);  
  //G4double InitMag=v0.mag();
  
  //  Saving yInput because yInput and yOut can reference the same array
  
  for(i=0;i<nvar;i++) yIn[i]=yInput[i];
  

  // G4cout<<"Input: ";
  
//   for(i=0;i<nvar;i++) { 
//     G4cout<<yIn[i]<<" ";
//   }  
//   G4cout<<G4endl;

//   G4cout<<"dydx: ";
  
//   for(i=0;i<nvar;i++) { 
//     G4cout<<dydx[i]<<" ";
//   }  
//   G4cout<<G4endl;
  

  G4double h = hstep; 
  //if(h>itsVolLength) h = itsVolLength;
  // Do two half steps
  
  AdvanceHelix(yIn,   dydx,  h, yOut);
 
  //G4cout<<"Full step: ";
  
//   for(i=0;i<nvar;i++) { 
//     G4cout<<yOut[i]<<" ";
//   }  
//   G4cout<<G4endl;
  

  //G4cout<<"Err: ";

  for(i=0;i<nvar;i++) { 
    yErr[i] = h*h*h*(yOut[i] - yIn[i]) ;
    //G4cout<<yErr[i]<<" ";
  }
  
  //G4cout<<G4endl;

}

G4double BDSRK4Stepper::DistChord()   const 
{
  //  return itsDist;
  return 0.0;
}
