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

#ifndef BDSINTEGRATORRMATRIXTHIN_HH
#define BDSINTEGRATORRMATRIXTHIN_HH

#include "BDSIntegratorMag.hh"

#include "globals.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator that just moves the particle parallel to the s axis
 *
 * Teleporter for moving particles without changing angle but only advances the particle
 * by h.
 *
 * If the new particle x,y coordinates are greater than maximumRadius, they are clipped
 * to this value.
 *
 * @author Stewart Boogert
 */

class BDSIntegratorRMatrixThin: public BDSIntegratorMag
{
public:
  BDSIntegratorRMatrixThin(BDSMagnetStrength const* strength,
			   G4Mag_EqRhs* eqOfMIn,
			   G4double maximumRadiusIn);

  virtual ~BDSIntegratorRMatrixThin(){;}

  virtual void Stepper(const G4double y[],
                       const G4double dydx[],
                       const G4double h,
                       G4double       yOut[],
                       G4double       yErr[]);

private:
  BDSIntegratorRMatrixThin();

protected:
  // matrix elements set differently in derived cavity fringe class.
  G4double kick1;
  G4double kick2;
  G4double kick3;
  G4double kick4;
  G4double rmat11;
  G4double rmat12;
  G4double rmat13;
  G4double rmat14;
  G4double rmat21;
  G4double rmat22;
  G4double rmat23;
  G4double rmat24;
  G4double rmat31;
  G4double rmat32;
  G4double rmat33;
  G4double rmat34;
  G4double rmat41;
  G4double rmat42;
  G4double rmat43;
  G4double rmat44;

  G4double maximumRadius;
};

#endif
