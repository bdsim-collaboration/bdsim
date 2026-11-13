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
#ifndef BDSINTEGRATORSYMPLECTICEULER_H
#define BDSINTEGRATORSYMPLECTICEULER_H

#include "BDSIntegratorMag.hh"

#include "globals.hh"

class G4Mag_EqRhs;

/**
 * @brief BDSIM 2nd order Euler integration.
 * 
 * @author Laurie Nevay
 */

class BDSIntegratorEuler: public BDSIntegratorMag
{
public:
  explicit BDSIntegratorEuler(G4Mag_EqRhs* eqOfMIn);

  virtual ~BDSIntegratorEuler(){;}

  /// Calculate output coordinates.
  virtual void Stepper(const G4double yIn[],     ///< Input coordinates x,y,z,px,py,pz,t
		       const G4double dydx[],    ///< Partial differentials at yInput
		       G4double       h,         ///< Length of trajectory to calculate
		       G4double       yOut[],    ///< Output array
		       G4double       yErr[]);   ///< Output error array

private:
  /// Private default constructor to force use of provided one.
  BDSIntegratorEuler() = delete;
};

#endif
