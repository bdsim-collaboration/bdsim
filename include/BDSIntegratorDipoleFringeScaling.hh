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
#ifndef BDSINTEGRATORDIPOLEFRINGESCALING_H
#define BDSINTEGRATORDIPOLEFRINGESCALING_H

#include "BDSIntegratorDipoleFringe.hh"

#include "globals.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Derived fringe field integrator that does normalise to momentum.
 *
 * @author Will Shields 
 */

class BDSIntegratorDipoleFringeScaling: public BDSIntegratorDipoleFringe
{
public:
  BDSIntegratorDipoleFringeScaling(BDSMagnetStrength const* strength,
				   G4double                 brhoIn,
				   G4Mag_EqRhs*             eqOfMIn,
				   G4double                 minimumRadiusOfCurvature,
				   const G4double&          tiltIn = 0);
  
  virtual ~BDSIntegratorDipoleFringeScaling(){;}

  /// The stepper for integration. Calculates momentum scaling factor
  /// then calls base class stepper.
  virtual void Stepper(const G4double yIn[],
		       const G4double dydx[],
		       const G4double h,
		       G4double       yOut[],
		       G4double       yErr[]);

private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorDipoleFringeScaling() = delete;

  /// Nominal magnetic rigidity
  const G4double bRho;
};

#endif
