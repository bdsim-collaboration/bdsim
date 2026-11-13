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
#ifndef BDSINTEGRATORSOLENOID_H
#define BDSINTEGRATORSOLENOID_H

#include "BDSIntegratorMag.hh"

#include "globals.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator that ignores the field and uses the analytical solution to a solenoid.
 * 
 * Analytical solution to a solenoid field.  This is implemented with some caveats - this is
 * only really valid for one complete step through the solenoid. If this is used for a partial
 * step, the edge focussing won't be correct.  A work in progress.
 * 
 * @author Laurie Nevay
 */

class BDSIntegratorSolenoid: public BDSIntegratorMag
{
public:
  BDSIntegratorSolenoid(BDSMagnetStrength const* strength,
			G4double                 brho,
			G4Mag_EqRhs*             eqOfMIn);

  virtual ~BDSIntegratorSolenoid(){;}

  /// Calculate the particle motion along step length l in the paraxial approximation.
  virtual void Stepper(const G4double y[],
		       const G4double dydx[],
		       const G4double h,
		       G4double       yOut[],
		       G4double       yErr[]);  


private:
  /// Private default constructor to enforce use of supplied constructor.
  BDSIntegratorSolenoid();

  /// The field strength.
  G4double bField;
};

#endif
