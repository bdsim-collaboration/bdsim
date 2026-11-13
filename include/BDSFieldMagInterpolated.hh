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
#ifndef BDSFIELDMAGINTERPOLATED_H
#define BDSFIELDMAGINTERPOLATED_H
#include "BDSExtent.hh"
#include "BDSFieldMag.hh"

#include "G4Transform3D.hh"
#include "G4Types.hh"

class BDSInterpolator;

/**
 * @brief Class to provide scaling and a base class pointer for interpolator fields.
 *
 * @author Laurie Nevay
 */

class BDSFieldMagInterpolated: public BDSFieldMag
{
public:
  BDSFieldMagInterpolated() = delete;
  BDSFieldMagInterpolated(const BDSInterpolator* interpolator,
			  const G4Transform3D&   offset,
			  G4double               scalingIn = 1.0);

  virtual ~BDSFieldMagInterpolated(){;}
  
  virtual G4bool TimeVarying() const {return timeVarying;}

  inline G4double Scaling() const {return scaling;}
  inline void     SetScaling(G4double scalingIn) {scaling = scalingIn;}
  
  /// Extent of field without any offset (ie in its own coordinate frame).
  inline BDSExtent ExtentNoOffset() const {return extentNoOffset;}
  
  /// For now, we will ignore any rotation of the transform. TODO.
  inline BDSExtent Extent() const {return extentNoOffset.Translate(transform.getTranslation());} ///< With offset.
  
  inline G4double SmallestSpatialStep() const {return smallestSpatialStep;}

protected:
  G4bool timeVarying;
  
private:
  G4double  scaling;        ///< Field value scaling.
  BDSExtent extentNoOffset; ///< Extent without offset.
  G4double  smallestSpatialStep;
};

#endif
