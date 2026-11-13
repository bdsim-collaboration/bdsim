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
#include "BDSIntegratorG4RK4MinStep.hh"
#include "BDSUtilities.hh"

#include "G4ClassicalRK4.hh"
#include "G4Mag_EqRhs.hh"
#include "G4MagIntegratorStepper.hh"

BDSIntegratorG4RK4MinStep::BDSIntegratorG4RK4MinStep(G4Mag_EqRhs* eqOfMIn,
						     G4double     minimumStepSizeIn):
  BDSIntegratorMag(eqOfMIn, 6),
  g4integrator(new G4ClassicalRK4(eqOfMIn, 6)),
  minimumStepSize(minimumStepSizeIn)
{;}

BDSIntegratorG4RK4MinStep::~BDSIntegratorG4RK4MinStep()
{
  delete g4integrator;
}

void BDSIntegratorG4RK4MinStep::Stepper(const G4double yIn[],
					const G4double dydx[],
					const G4double h,
					G4double       yOut[],
					G4double       yErr[])
{
  // in case of zero field or neutral particles do a linear step
  const G4double fcof = eqOfM->FCof();
  if (!BDS::IsFiniteStrength(fcof) || h < minimumStepSize)
    {
      AdvanceDriftMag(yIn,h,yOut,yErr);
      SetDistChord(0);
      return;
    }
  else
    {
      g4integrator->Stepper(yIn, dydx, h, yOut, yErr);
      return;
    }
}


