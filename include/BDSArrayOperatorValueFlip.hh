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
#ifndef BDSARRAYOPERATORVALUEFLIP_H
#define BDSARRAYOPERATORVALUEFLIP_H
#include "BDSArrayOperatorValue.hh"
#include "BDSFieldValue.hh"

#include "G4String.hh"
#include "G4Types.hh"

#include <array>
#include <string>

/**
 * @brief Flip field component in individual dimensions if out of original array bounds.
 * 
 * @author Laurie Nevay
 */

class BDSArrayOperatorValueFlip: public BDSArrayOperatorValue
{
public:
  BDSArrayOperatorValueFlip():
    BDSArrayOperatorValue("Flip(None)"),
    multiplier{1.0,1.0,1.0,1.0}
  {;}
  explicit BDSArrayOperatorValueFlip(const std::array<G4bool,4>& xyztIn):
    BDSArrayOperatorValueFlip()
  {
    for (G4int i = 0; i < 4; i++)
      {multiplier[i] = xyztIn[i] ? -1.0 : 1.0;}
    
    G4String newName = "Flip(";
    for (const auto& v : multiplier)
      {newName += std::to_string(v < 0);}
    newName += ")";
    name = newName;
  }
  virtual ~BDSArrayOperatorValueFlip(){;}
  
  virtual BDSFieldValue Apply(BDSFieldValue v,
                              G4int /*xInd*/,
                              G4int /*yInd*/ = 0,
                              G4int /*zInd*/ = 0,
                              G4int /*tInd*/ = 0) const
  {
    return BDSFieldValue(v.x()*multiplier[0], v.y()*multiplier[1], v.z()*multiplier[2]);
  }
  
private:
  /// This has to match the type of the field value components (e.g. double or float) to avoid
  /// a possible narrowing conversion.
  std::array<FIELDTYPET,4> multiplier;
};

#endif
