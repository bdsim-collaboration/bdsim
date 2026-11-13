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

#ifndef BDSINTEGRATORCAVITYFRINGE_HH
#define BDSINTEGRATORCAVITYFRINGE_HH

#include "BDSMagUsualEqRhs.hh"
#include "BDSIntegratorRMatrixThin.hh"

#include "globals.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator for RF cavity fringes. Only the transverse momentum kicks are
 * applied, this integrator will not accelerate any particles.
 *
 * If the new particle x,y coordinates are greater than maximumRadius, they are clipped
 * to this value.
 *
 * @author William Shields
 */

class BDSIntegratorCavityFringe: public BDSIntegratorRMatrixThin
{
public:
  BDSIntegratorCavityFringe() = delete;
	BDSIntegratorCavityFringe(BDSMagnetStrength const* strength,
                            G4Mag_EqRhs* eqOfMIn,
                            G4double brho,
                            G4double maximumRadiusIn);

  virtual ~BDSIntegratorCavityFringe(){;}

  virtual void Stepper(const G4double y[],
                       const G4double dydx[],
                       const G4double h,
                       G4double       yOut[],
                       G4double       yErr[]);

private:
  const	G4double phase;
  G4double efield;
  const	G4double isentrance;

protected:
  BDSMagUsualEqRhs* eq;    ///< BDSIM's eqRhs class to give access to particle properties
};

#endif
