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
#ifndef BDSARRAYOPERATORVALUE_H
#define BDSARRAYOPERATORVALUE_H
#include "G4String.hh"
#include "G4Types.hh"

/**
 * @brief Interface for modifying field values.
 *
 * Apply() method is pure virtual
 * 
 * @author Laurie Nevay
 */

class BDSArrayOperatorValue
{
public:
  explicit BDSArrayOperatorValue(const G4String& nameIn = "None"):
    name(nameIn)
  {;}
  virtual ~BDSArrayOperatorValue(){;}
  
  /// Return a name of the operator for feedback to the user in print out.
  virtual G4String Name() const {return name;}
  
  /// Index arguments are original coordinate space indices, i.e. allowed
  /// to be negative. No action by default.
  virtual BDSFieldValue Apply(BDSFieldValue v,
                              G4int /*xInd*/,
                              G4int /*yInd */ = 0,
                              G4int /*zInd */ = 0,
                              G4int /*tInd */ = 0) const
  {return v;}
  
protected:
  G4String name;
};

#endif
