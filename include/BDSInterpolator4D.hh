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
#ifndef BDSINTERPOLATOR4D_H
#define BDSINTERPOLATOR4D_H
#include "BDSArray4DCoords.hh"
#include "BDSExtent.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator.hh"

#include "G4Types.hh"
#include "G4ThreeVector.hh"

class BDSArray4DCoords;

/**
 * @brief Interface for all 4D interpolators.
 *
 * Does not own data - so multiple interpolators could be used on same data.
 *
 * @author Laurie Nevay
 */

class BDSInterpolator4D: public BDSInterpolator
{
public:
  BDSInterpolator4D() = delete;
  explicit BDSInterpolator4D(BDSArray4DCoords* arrayIn);
  virtual ~BDSInterpolator4D(){;}

  /// Public interface to a 4D interpolator. Returns Geant4 type as that's what will be needed.
  G4ThreeVector GetInterpolatedValue(G4double x, G4double y, G4double z, G4double t) const;

  inline const BDSArray4DCoords* Array() const {return array;}
  
  /// Extent of field.
  virtual BDSExtent Extent() const {return array->Extent();}
  
  virtual G4bool TimeVarying() const {return array->TimeVarying();}
  
protected:
  /// Each derived class should implement this function. Note T suffix (was templated)
  /// to distinguish it from GetInterpolatedValue which returns Geant4 types and is
  /// the main interface to should have the clean name.
  virtual BDSFieldValue GetInterpolatedValueT(G4double x,
					      G4double y,
					      G4double z,
					      G4double t) const = 0;

  /// The field data.
  BDSArray4DCoords* array;
};

#endif
