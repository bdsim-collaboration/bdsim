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

#ifndef BDSINTEGRATORPARALLELTRANSPORT_HH
#define BDSINTEGRATORPARALLELTRANSPORT_HH

#include "BDSIntegratorMag.hh"

#include "globals.hh"

class G4Mag_EqRhs;

/**
 * @brief Integrator that just moves the particle parallel to the s axis
 *
 * Teleporter for moving particles without changing angle but only advances the particle
 * by h.
 *
 * @author Stewart Boogert
 */

class BDSIntegratorParallelTransport: public BDSIntegratorMag
{
public:
  BDSIntegratorParallelTransport(G4Mag_EqRhs* eqOfMIn);

  virtual ~BDSIntegratorParallelTransport(){;}

  virtual void Stepper(const G4double y[],
                       const G4double dydx[],
                       const G4double h,
                       G4double       yOut[],
                       G4double       yErr[]);

private:
  BDSIntegratorParallelTransport();

};

#endif
