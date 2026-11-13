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
#ifndef BDSINTEGRATORG4RK4MINSTEP_H
#define BDSINTEGRATORG4RK4MINSTEP_H

#include "BDSIntegratorMag.hh"

#include "globals.hh"

class G4Mag_EqRhs;
class G4MagIntegratorStepper;

/**
 * @brief Integrator that wraps a G4ClassicalRK4 and below a minimum step size uses a drift.
 * 
 * @author Laurie Nevay
 */

class BDSIntegratorG4RK4MinStep: public BDSIntegratorMag
{
public:
  BDSIntegratorG4RK4MinStep(G4Mag_EqRhs* eqOfMIn,
			    G4double     minimumStepSizeIn);
  
  virtual ~BDSIntegratorG4RK4MinStep();

  /// Check if the quadrupole has finite strength and use drift if not. If finite strength,
  /// convert to local curvilinear coordinates and check for paraxial approximation. If paraxial,
  /// use thick quadrupole matrix for transport, else use the G4ClassicalRK4 backup stepper.
  virtual void Stepper(const G4double y[],
		       const G4double dydx[],
		       const G4double h,
		       G4double       yOut[],
		       G4double       yErr[]);

private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorG4RK4MinStep();

  G4MagIntegratorStepper* g4integrator;
  G4double minimumStepSize;
};

#endif
