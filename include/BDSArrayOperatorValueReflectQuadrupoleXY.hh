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
#ifndef BDSARRAYOPERATORVALUEREFLECTQUADRUPOLEXY_H
#define BDSARRAYOPERATORVALUEREFLECTQUADRUPOLEXY_H
#include "BDSArrayOperatorValue.hh"
#include "BDSFieldValue.hh"

#include "G4String.hh"
#include "G4Types.hh"

/**
 * @brief Reflect field values for a quadrupole field in the positive quadrant.
 *
 * Note this must work in combination with a coordinate reflection in x and y.
 * Ultimately, the goal is this:
 *
 * \verbatim
 *       y
 *       ∧
 *       |
 * <---  |  ∧
 *    |  |  | original
 *    v  |  --->
 *       |
 * -------------------> x
 * <---  |     ∧
 *    |  |     |
 *    v  |  <---
 *       |
 *       |
 * \endverbatim
 *
 * 
 * @author Laurie Nevay
 */

class BDSArrayOperatorValueReflectQuadrupoleXY: public BDSArrayOperatorValue
{
public:
  BDSArrayOperatorValueReflectQuadrupoleXY():
    BDSArrayOperatorValue("ReflectQuadrupoleXY")
  {;}
  virtual ~BDSArrayOperatorValueReflectQuadrupoleXY(){;}
  
  virtual BDSFieldValue Apply(BDSFieldValue v,
                              G4int xInd,
                              G4int yInd = 0,
                              G4int /*zInd*/ = 0,
                              G4int /*tInd*/ = 0) const
  {
    if (yInd < 0)
      {v[0] *= -1;}
    if (xInd < 0)
      {v[1] *= -1;}
    return v;
  }
};

#endif
