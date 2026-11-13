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
#ifndef BDSFIELDMAGINTERPOLATED2LAYER_H
#define BDSFIELDMAGINTERPOLATED2LAYER_H
#include "BDSExtent.hh"
#include "BDSFieldMag.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

class BDSFieldMagInterpolated;

/**
 * @brief Two interpolated fields in one. One takes precedence in a subregion.
 *
 * @author Laurie Nevay
 */

class BDSFieldMagInterpolated2Layer: public BDSFieldMag
{
public:
  BDSFieldMagInterpolated2Layer(BDSFieldMagInterpolated* mainFieldIn,
				BDSFieldMagInterpolated* subFieldIn);
  virtual ~BDSFieldMagInterpolated2Layer();
  
  /// Return the interpolated field value at a given point.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const G4double       t = 0) const;
  
  virtual G4bool TimeVarying() const;

private:
  BDSFieldMagInterpolated* mainField;
  BDSFieldMagInterpolated* subField;
  BDSExtent subFieldExtent;
};

#endif
