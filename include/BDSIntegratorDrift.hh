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
#ifndef BDSINTEGRATORDRIFT_H
#define BDSINTEGRATORDRIFT_H

#include "globals.hh"
#include "G4ThreeVector.hh"

/**
 * @brief Routine for drift algorithm.
 *
 * q_{out} = q_{in} + \hat{p}_{in} h
 *
 * @author Laurie nevay
 */

class BDSIntegratorDrift
{
public:
  BDSIntegratorDrift();

  ~BDSIntegratorDrift();

  /// Advance a particle by using a drift algorithm. Uses first 6 variables in
  /// array and assumes at least 6 elements in each array so only suitable
  /// for magnetic field integrators. Sets error to 0. Does not provide
  /// distance from chord parameter as this is by definition 0 for a drift
  /// and should be set elsewhere.
  void AdvanceDriftMag(const G4double yIn[],  ///< Input coordinates x,y,z,px,py,pz,t
		       const G4double h,      ///< Step length in mm
		       G4double       yOut[], ///< Output array [6]
		       G4double       yErr[]) const;  ///< Error array [6] all 0.

  void AdvanceDriftMag(const G4double       yIn[],
		       const G4ThreeVector& unitMomentum,
		       const G4double       h,
		       G4double             yOut[],
		       G4double             yErr[]) const;

};

#endif
