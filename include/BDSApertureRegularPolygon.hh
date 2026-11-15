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
#ifndef BDSAPERTUREREGULARPOLYGON_H
#define BDSAPERTUREREGULARPOLYGON_H

#include "BDSAperture.hh"

#include "G4Types.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <array>
#include <cmath>

class BDSExtent;
class BDSPolygon;

/**
 * @brief Circular aperture.
 *
 * @author Laurie Nevay
 */

class BDSApertureRegularPolygon: public BDSAperture
{
public:
  BDSApertureRegularPolygon() = delete;
  explicit BDSApertureRegularPolygon(G4int    nSidesIn,
                                     G4double radiusIn,
                                     G4double polygonTiltIn);
  BDSApertureRegularPolygon(const BDSApertureRegularPolygon& other);
  BDSApertureRegularPolygon& operator=(const BDSApertureRegularPolygon& other) noexcept;
  virtual ~BDSApertureRegularPolygon(){;}

  G4int nSides; ///< Number of sides (>= 3).
  G4double radius; ///< Radius at a point.
  G4double polygonTilt; ///< Rotation angle of polygon.

  G4bool    Equals(const BDSAperture* other) const override;
  void      CheckInfoOK()                    const override;
  G4double  RadiusToEncompass()              const override {return radius;}
  BDSExtent Extent()                         const override;
  G4double  MaximumInscribedCircleRadius()   const override {return radius*std::cos(CLHEP::twopi / 2*(G4double)nSides);}
  unsigned int MinimumNumberOfPoints() const override {return nSides;}

  BDSApertureRegularPolygon        operator+ (G4double number) const;
  const BDSApertureRegularPolygon& operator+=(G4double number);
  BDSApertureRegularPolygon        operator* (G4double number) const;
  const BDSApertureRegularPolygon& operator*=(G4double number);
  
  BDSAperture* Plus(G4double number) const override;
  BDSAperture* Times(G4double number) const override;
  BDSAperture* Clone() const override;
  std::array<G4double, 7> ApertureNumbers() const override;
  
  BDSPolygon PolygonNPoints(unsigned int nPointsIn) const override;
};

#endif
