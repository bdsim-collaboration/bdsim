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
#ifndef BDSARRAYOPERATORVALUEREFLECTDIPOLEY_H
#define BDSARRAYOPERATORVALUEREFLECTDIPOLEY_H
#include "BDSArrayOperatorValue.hh"
#include "BDSFieldValue.hh"

#include "G4String.hh"
#include "G4Types.hh"

/**
 * @brief Reflect field values for a dipolar field about the x-z plane.
 *
 * Note this must work in combination with a coordinate reflection.
 *
 * \verbatim
 *
 *
 *     ∧
 *     | original
 *     --->
 *
 * -------------> x,z
 *      ∧
 *      |
 *   <---
 *
 * \endverbatim
 *
 * 
 * @author Laurie Nevay
 */

class BDSArrayOperatorValueReflectDipoleY: public BDSArrayOperatorValue
{
public:
  BDSArrayOperatorValueReflectDipoleY():
    BDSArrayOperatorValue("ReflectDipoleY")
  {;}
  virtual ~BDSArrayOperatorValueReflectDipoleY(){;}
  
  virtual BDSFieldValue Apply(BDSFieldValue v,
                              G4int xInd,
                              G4int yInd = 0,
                              G4int /*zInd*/ = 0,
                              G4int /*tInd*/ = 0) const
  {
    (void)xInd; // enough to prevent a compiler warning
    // only for lower y-half, we flip the x-component
    if (yInd < 0)
      {v[0] *= -1.0;}
    return v;
  }
};

#endif
