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
#ifndef BDSARRAYOPERATORVALUEREFLECTSOLENOIDZ_H
#define BDSARRAYOPERATORVALUEREFLECTSOLENOIDZ_H
#include "BDSArrayOperatorValue.hh"
#include "BDSFieldValue.hh"

#include "G4String.hh"
#include "G4Types.hh"

/**
 * @brief Reflect field values for a solenoid-like field in the z axis.
 *
 * Note this must work in combination with a coordinate reflection in x and y.
 * Ultimately, the goal is this:
 *
 * \verbatim
 *         x,y
 *         ∧
 *         |
 *         |  ∧
 *   --->  |  | original
 *      |  |  --->
 *      v  |
 *----------------------> z
 *         |
 *         |
 * \endverbatim
 *
 * 
 * @author Laurie Nevay
 */

class BDSArrayOperatorValueReflectSolenoidZ: public BDSArrayOperatorValue
{
public:
  BDSArrayOperatorValueReflectSolenoidZ():
    BDSArrayOperatorValue("ReflectSolenoidZ")
  {;}
  virtual ~BDSArrayOperatorValueReflectSolenoidZ(){;}
  
  virtual BDSFieldValue Apply(BDSFieldValue v,
                              G4int /*xInd*/,
                              G4int /*yInd*/,
                              G4int zInd = 0,
                              G4int /*tInd*/ = 0) const
  {
    if (zInd < 0)
      {
	v[0] *= -1.0;
	v[1] *= -1.0;
      }
    return v;
  }
};

#endif
