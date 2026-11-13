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
#ifndef BDSINTERPOLATOR3D_H
#define BDSINTERPOLATOR3D_H
#include "BDSArray3DCoords.hh"
#include "BDSDimensionType.hh"
#include "BDSExtent.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator.hh"

#include "G4Types.hh"
#include "G4ThreeVector.hh"

/**
 * @brief Interface for all 3D interpolators.
 *
 * Does not own data - so multiple interpolators could be used on same data.
 *
 * @author Laurie Nevay
 */

class BDSInterpolator3D: public BDSInterpolator
{
public:
  BDSInterpolator3D() = delete;
  explicit BDSInterpolator3D(BDSArray3DCoords* arrayIn);
  virtual ~BDSInterpolator3D(){;}

  /// Public interface to a 3D interpolator. Returns Geant4 type as that's what will be needed.
  G4ThreeVector GetInterpolatedValue(G4double x, G4double y, G4double z) const;

  inline const BDSArray3DCoords* Array() const {return array;}

  /// Accessor for the active dimension this represents (first).
  inline BDSDimensionType FirstDimension() const {return array->FirstDimension();}

  /// Accessor for the active dimension this represents (second).
  inline BDSDimensionType SecondDimension() const {return array->SecondDimension();}

  /// Accessor for the active dimension this represents (second).
  inline BDSDimensionType ThirdDimension() const {return array->ThirdDimension();}
  
  /// Extent of field.
  virtual BDSExtent Extent() const {return array->Extent();}
  
  virtual G4bool TimeVarying() const {return array->TimeVarying();}
  
protected:
  /// Each derived class should implement this function. Note T suffix (was templated)
  /// to distinguish it from GetInterpolatedValue which returns Geant4 types and is
  /// the main interface to should have the clean name.
  virtual BDSFieldValue GetInterpolatedValueT(G4double x, G4double y, G4double z) const = 0;

  /// The field data.
  BDSArray3DCoords* array;
};

#endif
