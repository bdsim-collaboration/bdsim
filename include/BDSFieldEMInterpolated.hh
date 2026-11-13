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
#ifndef BDSFIELDEMINTERPOLATED_H
#define BDSFIELDEMINTERPOLATED_H
#include "BDSFieldEM.hh"

#include "G4Transform3D.hh"
#include "G4Types.hh"

class BDSInterpolator;

/**
 * @brief Class to provide scaling and a base class pointer for interpolator fields.
 *
 * @author Laurie Nevay
 */

class BDSFieldEMInterpolated: public BDSFieldEM
{
public:
  BDSFieldEMInterpolated() = delete;
  BDSFieldEMInterpolated(const BDSInterpolator* eInterpolator,
			 const BDSInterpolator* bInterpolator,
			 const G4Transform3D&   offset,
			 G4double               eScalingIn = 1.0,
			 G4double               bScalingIn = 1.0);
  virtual ~BDSFieldEMInterpolated(){;}
  
  virtual G4bool TimeVarying() const {return timeVarying;}

  /// @{ Accessor.
  inline G4double EScaling() const {return eScaling;}
  inline G4double BScaling() const {return bScaling;}
  /// @}

  /// @{ Setter.
  inline void     SetEScaling(G4double eScalingIn) {eScaling = eScalingIn;}
  inline void     SetBScaling(G4double bScalingIn) {bScaling = bScalingIn;}
  /// @}

  inline G4double SmallestSpatialStep() const {return smallestSpatialStep;}

protected:
  G4bool   timeVarying;
  
private:
  G4double eScaling; ///< E field scaling value.
  G4double bScaling; ///< B field scaling value.
  G4double smallestSpatialStep;
};

#endif
