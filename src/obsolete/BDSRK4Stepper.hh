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

#ifndef BDSRK4Stepper_h
#define BDSRK4Stepper_h 1


#include "globals.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4EquationOfMotion.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"
#include "G4Navigator.hh"

class BDSRK4Stepper : public G4MagIntegratorStepper
{
  
public:  
  
  BDSRK4Stepper(G4EquationOfMotion *EqRhs, int nvar=6);

  
  ~BDSRK4Stepper();
  
  void Stepper( const G4double y[],
		const G4double dydx[],
		const G4double h,
		G4double yout[],
		G4double yerr[]  );
  // The stepper for the Runge Kutta integration.
  // The stepsize is fixed, equal to h.
  // Integrates ODE starting values y[0 to 6]
  // Outputs yout[] and its estimated error yerr[].
  
  G4double DistChord()   const;
  // Estimate maximum distance of curved solution and chord ... 
  
    
  void StepperName();
  
  //  void SetVolLength(G4double aVolLength);
  
  
  G4int IntegratorOrder()const { return 4; }

  protected:
  //  --- Methods used to implement all the derived classes -----
  
  void AdvanceHelix( const G4double  yIn[],
		     const G4double dydx[],
		     const G4double  h,
		     G4double  yRK[]);    // output 
  // A first order Step along a quad inside the field.
  
private:
  /// assignment and copy constructor not implemented nor used
  BDSRK4Stepper& operator=(const BDSRK4Stepper&);
  BDSRK4Stepper(BDSRK4Stepper&);
  
  G4ThreeVector yInitial, yMidPoint, yFinal;
  // Data stored in order to find the chord.
  
    G4EquationOfMotion* itsEqRhs;
  //    G4ThreeVector itsInitialPoint, itsFinalPoint, itsMidPoint,itsDistVec;
  //  G4double itsDist;
  
  //  G4double itsVolLength;
  
  G4double *yt, *dydxt, *dydxm, *yTemp, *yIn;

};

// inline  void BDSRK4Stepper::SetVolLength(G4double aVolLength)
// {itsVolLength=aVolLength;}

inline void BDSRK4Stepper::StepperName()
{G4cout<<"BDSRK4Stepper"<<G4endl;}

#endif
