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
// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: BDSSkewSextStepper.hh,v 1.1 2005/01/22 17:05:31 agapov Exp $
// GEANT4 tag $Name:  $
//
//
// class BDSSkewSextStepper
//
// Class description:
//

// History:
// - Created. G.Blair 1/11/00

#ifndef BDSSKEWSEXTSTEPPER_HH
#define BDSSKEWSEXTSTEPPER_HH
#include "globals.hh"
#include "G4MagIntegratorStepper.hh"
//#include "G4MagHelicalStepper.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"
#include "G4Navigator.hh"

class BDSSkewSextStepper : public G4MagIntegratorStepper
{

  public:  // with description

    BDSSkewSextStepper(G4Mag_EqRhs *EqRhs);

    ~BDSSkewSextStepper();

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
 
     void SetBDblPrime(G4double aBDblPrime);
     G4double GetBDblPrime();

     void StepperName();

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
		       G4double  ySkewSext[]);    // output 
      // A first order Step along a sext inside the field.

  private:
  
    G4ThreeVector yInitial, yMidPoint, yFinal;
      // Data stored in order to find the chord.

    G4Mag_EqRhs*  fPtrMagEqOfMot;

    G4double itsBDblPrime;
  //    G4ThreeVector itsInitialPoint, itsFinalPoint, itsMidPoint,itsDistVec;
    G4double itsDist;

};

inline  void BDSSkewSextStepper::SetBDblPrime(G4double aBDblPrime)
{itsBDblPrime=aBDblPrime;
}
inline G4double BDSSkewSextStepper::GetBDblPrime()
{return itsBDblPrime;}

inline void BDSSkewSextStepper::StepperName()
{G4cout<<"BDSSkewSextStepper"<<G4endl;}

#endif /* BDSSKEWSEXTSTEPPER_HH */
