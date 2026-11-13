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
#include "BDSIntegratorEuler.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

#include <cmath>

BDSIntegratorEuler::BDSIntegratorEuler(G4Mag_EqRhs* eqOfMIn):
  BDSIntegratorMag(eqOfMIn, 6)
{;}

void BDSIntegratorEuler::Stepper(const G4double yIn[],
				 const G4double dydx[],
				 G4double       h,
				 G4double       yOut[],
				 G4double       yErr[])
{
  // In case of zero field or neutral particles do a linear step:
  if (zeroStrength || !BDS::IsFiniteStrength(eqOfM->FCof()))
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      SetDistChord(0);
      return;
    }
  
  const G4ThreeVector a_start = G4ThreeVector(dydx[3], dydx[4], dydx[5]);
  if (a_start.mag2() < 1e-60)
    {// no potential as no magnetic field - use drift routine
      AdvanceDriftMag(yIn, h, yOut, yErr);
      SetDistChord(0);
      return;
    }
  
  const G4ThreeVector pos     = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  const G4ThreeVector mom     = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  // unit momentum already provided for us
  const G4ThreeVector momU    = G4ThreeVector(dydx[0], dydx[1], dydx[2]); 
  const G4double      hSqrd   = std::pow(h, 2);
  const G4double      hHalf   = h*0.5;

  // calcualte the position half step length for drifting with no force
  // 'phalf' => plus half step
  G4ThreeVector pos_phalf = pos + momU*hHalf;

  // calculate the mid point if the particle were to drift. Momentum
  // stays the same of course.
  G4double midPointDrift[8];
  G4double midPointDriftErr[8];
  AdvanceDriftMag(yIn, hHalf, midPointDrift, midPointDriftErr);

  G4double potential[8]; // output array for g4 query
  RightHandSide(midPointDrift, potential); // query field and calculate vector potential
  // get the vector potential bit from the array
  G4ThreeVector a_phalf = G4ThreeVector(potential[3], potential[4], potential[5]);

  // new coordinates
  // pos_new = pos + p.unit()*h + (A*h^2)/2*p.mag()
  // mom_new = mom + A*h
  G4double      factor  = hSqrd*0.5/mom.mag();  // calculate common factors first
  G4ThreeVector pos_new = pos + momU*h + a_phalf*factor;
  G4ThreeVector mom_new = mom + a_phalf*h;

  // error calculation
  // use the difference in potential from beginning and midpoint to estimate
  // how different output coordinates would've been
  G4ThreeVector a_diff  = a_phalf - a_start;
  G4ThreeVector pos_err = a_diff*factor; // proportional to h^2
  G4ThreeVector mom_err = a_diff*h;

  // write out output values and errors to arrays
  for (G4int i = 0; i < 3; i++)
    {
      yOut[i]   = pos_new[i];
      yOut[i+3] = mom_new[i];
      yErr[i]   = std::abs(pos_err[i]);
      yErr[i+3] = std::abs(mom_err[i]);
    }
  
  // ((average of new and old) - mid point from drift ) .mag()
  // both are straight lines, but it's an approximately close
  G4double dc = (0.5*(pos_new + pos) - pos_phalf).mag();
  SetDistChord(dc);
}
