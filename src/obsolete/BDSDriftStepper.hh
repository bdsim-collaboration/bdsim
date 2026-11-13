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
// Class description:
//

// History:
// - Created. G.Blair 1/11/00

#ifndef BDSDRIFTSTEPPER_HH
#define BDSDRIFTSTEPPER_HH
#include "globals.hh"
#include "G4MagIntegratorStepper.hh"
//#include "G4MagHelicalStepper.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"
#include "G4Navigator.hh"

class BDSDriftStepper : public G4MagIntegratorStepper
{

public:  // with description
  
  BDSDriftStepper(G4Mag_EqRhs *EqRhs);
  
  ~BDSDriftStepper();
  
  void Stepper( const G4double y[],
		const G4double dydx[],
		const G4double h,
		G4double yout[],
		G4double yerr[]  );
  // The stepper for the Runge Kutta integration.
  // The stepsize is fixed, equal to h.
  // Integrates ODE starting values y[0 to 6]
  // Outputs yout[] and its estimated error yerr[].
  
  
  void StepperName();
  
  G4double DistChord()const;
  /*
    void DumbStepper( const G4double yInput[],
    G4ThreeVector Bfld,
    G4double hstep,
    G4double yOut[]);
    
  */
public: // without description
  
    G4int IntegratorOrder()const { return 2; }

protected:
  
  //  --- Methods used to implement all the derived classes -----
  
  void AdvanceHelix( const G4double  yIn[],
		     G4ThreeVector Bfld,
		     G4double  h,
		     G4double  yDrift[]);    // output 
  // A first order Step along a drift inside the field.
  
private:
  G4bool verboseStep;
  G4int  verboseEventNumber;
  
};


inline void BDSDriftStepper::StepperName()
{G4cout<<"BDSDriftStepper"<<G4endl;}

#endif /* BDSDRIFTSTEPPER_HH */
