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
#ifndef BDSARRAYOPERATORVALUEREFLECTDIPOLEXY_H
#define BDSARRAYOPERATORVALUEREFLECTDIPOLEXY_H
#include "BDSArrayOperatorValue.hh"
#include "BDSFieldValue.hh"

#include "G4String.hh"
#include "G4Types.hh"

/**
 * @brief Reflect field values for a dipolar field in the positive quadrant.
 *
 * Note this must work in combination with a coordinate reflection in x and y.
 * Ultimately, the goal is this:
 *
 * \verbatim
 *       y
 *       ∧
 *       |
 *    ∧  |  ∧
 *    |  |  | original
 * <---  |  --->
 *       |
 * -------------------> x
 * ∧     |     ∧
 * |     |     |
 * --->  |  <---
 *       |
 *       |
 * \endverbatim
 *
 * 
 * @author Laurie Nevay
 */

class BDSArrayOperatorValueReflectDipoleXY: public BDSArrayOperatorValue
{
public:
  BDSArrayOperatorValueReflectDipoleXY():
    BDSArrayOperatorValue("ReflectDipoleXY")
  {;}
  virtual ~BDSArrayOperatorValueReflectDipoleXY(){;}
  
  virtual BDSFieldValue Apply(BDSFieldValue v,
                              G4int xInd,
                              G4int yInd = 0,
                              G4int /*zInd*/ = 0,
                              G4int /*tInd*/ = 0) const
  {
    // only top left or bottom right quadrant need the x-component flipped
    if ( (xInd < 0 && yInd >= 0) || (xInd >= 0 && yInd < 0) )
      {v[0] *= -1.0;}
    if  ( (yInd < 0) )
      {v[2] *= -1.0;}
    return v;
  }
};

#endif
