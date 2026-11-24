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
#ifndef BDSAPERTURECLICPCL_H
#define BDSAPERTURECLICPCL_H
#include "BDSApertureCurved.hh"
#include "BDSTiltOffset.hh"

#include "G4Types.hh"

#include <algorithm>
#include <array>

class BDSAperture;
class BDSExtent;
class BDSPolygon;

/**
 * @brief CLIC Post-collision line aperture.
 *
 * The shape is composed of an upper and lower ellipse joined together. Both have the
 * same horizontal width. The upper ellipse may be displaced upwards.
 *
 * @author Laurie Nevay
 */

class BDSApertureClicPCL: public BDSApertureCurved
{
public:
  BDSApertureClicPCL() = delete;
  BDSApertureClicPCL(G4double xHalfWidthIn,
                     G4double yTopIn,
                     G4double yBottomIn,
                     G4double ySepIn,
                     unsigned int nPointsIn);
  virtual ~BDSApertureClicPCL(){;}

  G4double x;
  G4double yTop;
  G4double yBottom;
  G4double ySep;

  G4bool    Equals(const BDSAperture* other) const override;
  void      CheckInfoOK()                    const override;
  G4double  RadiusToEncompass()              const override {return std::max({yBottom, ySep+yTop, x}) + tiltOffset.Radius();}
  BDSExtent Extent()                         const override;
  G4double  MaximumInscribedCircleRadius()   const override;
  unsigned int MinimumNumberOfPoints()       const override {return 10;}

  BDSApertureClicPCL        operator+ (G4double number) const;
  const BDSApertureClicPCL& operator+=(G4double number);
  BDSApertureClicPCL        operator* (G4double number) const;
  const BDSApertureClicPCL& operator*=(G4double number);
  
  BDSAperture* Plus(G4double number) const override;
  BDSAperture* Times(G4double number) const override;
  BDSAperture* Clone() const override;
  std::array<G4double, 7> ApertureNumbers() const override;
  
  BDSPolygon PolygonNPoints(unsigned int nPointsIn) const override;
};

#endif
