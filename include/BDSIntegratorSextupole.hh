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
#ifndef BDSINTEGRATORSEXTUPOLE_H
#define BDSINTEGRATORSEXTUPOLE_H

#include "BDSIntegratorEulerOld.hh"

#include "globals.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator for sextupole field.
 */

class BDSIntegratorSextupole: public BDSIntegratorEulerOld
{
public:
  BDSIntegratorSextupole(BDSMagnetStrength const* strength,
			 G4double                 brho,
			 G4Mag_EqRhs*             eqOfMIn);
  
  virtual ~BDSIntegratorSextupole(){;}
  
protected:
  /// Calculate the new particle coordinates.
  virtual void AdvanceHelix(const G4double yIn[],
			    G4double       h,
			    G4double       yOut[],
                G4double       yErr[]);
  
private:
  BDSIntegratorSextupole() = delete;

  /// 2nd derivative of the field
  G4double bDoublePrime;
};

#endif
