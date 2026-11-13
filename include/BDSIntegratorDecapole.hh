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
#ifndef BDSINTEGRATORDECAPOLE_H
#define BDSINTEGRATORDECAPOLE_H

#include "BDSIntegratorEulerOld.hh"

#include "globals.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator for Decapolar field.
 * 
 * @author Laurie Nevay
 */

class BDSIntegratorDecapole: public BDSIntegratorEulerOld
{
public:
  BDSIntegratorDecapole(BDSMagnetStrength const* strength,
			G4double                 brho,
			G4Mag_EqRhs*             eqOfMIn);
  
  virtual ~BDSIntegratorDecapole(){;}

protected:
  /// Calculate the new particle coordinates.
  virtual void AdvanceHelix(const G4double yIn[],
			    G4double       h,
			    G4double       yDec[],
			    G4double       yErr[]);
  
private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorDecapole() = delete;

  /// 4th derivative of magnetic field
  G4double bQuadruplePrime;
};

#endif
