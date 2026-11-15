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
#ifndef BDSAPERTURECURVED_H
#define BDSAPERTURECURVED_H
#include "BDSAperture.hh"
#include "BDSApertureType.hh"

/**
 * @brief Inclusion of nPoints for aperture.
 * 
 * Control the minimum number of points. Common functionality for curved
 * aperture shapes.
 *
 * @author Laurie Nevay
 */

class BDSApertureCurved: public BDSAperture
{
public:
  BDSApertureCurved(BDSApertureType apertureTypeIn,
                    unsigned int    nPointsIn):
    BDSAperture(apertureTypeIn),
    nPoints(nPointsIn)
  {;}
  BDSApertureCurved(const BDSApertureCurved& other):
    BDSAperture(other),
    nPoints(other.nPoints)
  {;}
  BDSApertureCurved& operator=(const BDSApertureCurved& other) noexcept
  {
    if (this != &other)
      {
        BDSAperture::operator=(other);
        nPoints = other.nPoints;
      }
    return *this;
  }
  virtual ~BDSApertureCurved(){;}

  unsigned int RecommendedNumberOfPoints() const override {return nPoints;}

protected:
  unsigned int nPoints;
};

#endif
