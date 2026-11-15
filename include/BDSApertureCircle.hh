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
#ifndef BDSAPERTURECIRCLE_H
#define BDSAPERTURECIRCLE_H

#include "BDSApertureCurved.hh"

#include "G4Types.hh"

#include <array>

class BDSAperture;
class BDSExtent;
class BDSPolygon;

/**
 * @brief Circular aperture.
 *
 * @author Laurie Nevay
 */

class BDSApertureCircle: public BDSApertureCurved
{
public:
  BDSApertureCircle() = delete;
  explicit BDSApertureCircle(G4double     radiusIn,
                             unsigned int nPointsIn = 0);
  BDSApertureCircle(const BDSApertureCircle& other);
  BDSApertureCircle& operator=(const BDSApertureCircle& other) noexcept;
  virtual ~BDSApertureCircle(){;}

  G4double radius;

  G4bool    Equals(const BDSAperture* other) const override;
  void      CheckInfoOK()                    const override;
  G4double  RadiusToEncompass()              const override {return radius;}
  BDSExtent Extent()                         const override;
  G4double  MinimumInscribedCircleRadius()   const override {return radius;}
  unsigned int MinimumNumberOfPoints() const override {return 10;}

  BDSApertureCircle        operator+ (G4double number) const;
  const BDSApertureCircle& operator+=(G4double number);
  BDSApertureCircle        operator* (G4double number) const;
  const BDSApertureCircle& operator*=(G4double number);
  
  BDSAperture* Plus(G4double number) const override;
  BDSAperture* Times(G4double number) const override;
  BDSAperture* Clone() const override;
  std::array<G4double, 7> ApertureNumbers() const override;
  
  BDSPolygon PolygonNPoints(unsigned int nPointsIn) const override;
};

#endif
