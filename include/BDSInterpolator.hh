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
#ifndef BDSINTERPOLATOR_H
#define BDSINTERPOLATOR_H
#include "BDSArray4DCoords.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"

#include <limits>

/**
 * @brief Interface for all interpolators containing basic extent of validity.
 *
 * @author Laurie Nevay
 */

class BDSInterpolator
{
public:
  explicit BDSInterpolator(BDSArray4DCoords* arrayIn = nullptr):
    smallestSpatialStep(std::numeric_limits<double>::max())
  {
    if (arrayIn)
      {smallestSpatialStep = arrayIn->SmallestSpatialStep();}
    else
      {throw BDSException(__METHOD_NAME__, "Invalid array to construct interpolator on.");}
  }
  virtual ~BDSInterpolator(){;}

  /// Interface each derived class must provide.
  virtual BDSExtent Extent() const = 0;
  
  virtual G4bool TimeVarying() const = 0;
  
  /// The minimum *spatial* length between any two points being interpolated in any dimension.
  /// For example, the minimum of the step size in x,y,z in a 3D interpolated grid.
  inline G4double SmallestSpatialStep() const {return smallestSpatialStep;}
  
protected:
  G4double smallestSpatialStep;
};

#endif
