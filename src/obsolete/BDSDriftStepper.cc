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

void BDSDriftStepper::AdvanceHelix( const G4double  yIn[],
				    G4ThreeVector /*Bfld*/,
				    G4double  h,
				    G4double  yDrift[])
{
  G4ThreeVector positionMove, endTangent;

  const G4double *pIn = yIn+3;
  G4ThreeVector v0= G4ThreeVector( pIn[0], pIn[1], pIn[2]);  


  G4double InitMag=v0.mag();

      positionMove  = (h/InitMag) * v0;


      yDrift[0]   = yIn[0] + positionMove.x(); 
      yDrift[1]   = yIn[1] + positionMove.y(); 
      yDrift[2]   = yIn[2] + positionMove.z(); 
				
      yDrift[3] = v0.x();
      yDrift[4] = v0.y();
      yDrift[5] = v0.z();


      // dump step information for particular event
      //G4bool verboseStep       = BDSGlobalConstants::Instance()->GetVerboseStep();
      //G4int verboseEventNumber = BDSGlobalConstants::Instance()->GetVerboseEventNumber();
      if(verboseStep && verboseEventNumber == G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID())
	{
	  int G4precision = G4cout.precision();
	  G4cout.precision(10);
	  G4cout<<" h="<<h/CLHEP::m<<G4endl;
	  G4cout<<"xIn="<<yIn[0]/CLHEP::m<<" yIn="<<yIn[1]/CLHEP::m<<
	    " zIn="<<yIn[2]/CLHEP::m<<" v0="<<v0<<G4endl;
	  G4cout<<"xOut="<<yDrift[0]/CLHEP::m<<" yOut="<<yDrift[1]/CLHEP::m<<
	    "zOut="<<yDrift[2]/CLHEP::m<<G4endl;
	  // set precision back
	  G4cout.precision(G4precision);
	}
}

void BDSDriftStepper::Stepper( const G4double yInput[],
		     const G4double[],
		     const G4double hstep,
		     G4double yOut[],
		     G4double yErr[]      )
{  
   const G4int nvar = 6 ;

   G4int i;
   for(i=0;i<nvar;i++) yErr[i]=0;

   //   G4cout<<G4endl;
   //     G4cout<<"yInput="<<yInput[0]<<" "<< yInput[1]<<" "<<yInput[2]<<G4endl;
   AdvanceHelix(yInput,(G4ThreeVector)0,hstep,yOut);

   //     G4cout<<"yOut="<<yOut[0]<<" "<< yOut[1]<<" "<<yOut[2]<<G4endl;

   //   G4cout<<"hstep="<<hstep<<G4endl;
}

G4double BDSDriftStepper::DistChord()   const 
{
return 0;
}

BDSDriftStepper::~BDSDriftStepper()
{}
