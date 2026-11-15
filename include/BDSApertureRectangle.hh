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
#ifndef BDSAPERTURERECTANGLE_H
#define BDSAPERTURERECTANGLE_H

#include "BDSAperture.hh"
#include "BDSTiltOffset.hh"

#include "G4Types.hh"

#include <algorithm>
#include <array>
#include <cmath>

class BDSExtent;
class BDSPolygon;

/**
 * @brief Rectangular aperture.
 *
 * @author Laurie Nevay
 */

class BDSApertureRectangle: public BDSAperture
{
public:
  BDSApertureRectangle() = delete;
  BDSApertureRectangle(G4double aIn,
                       G4double bIn);
  virtual ~BDSApertureRectangle(){;}

  G4double a;
  G4double b;

  G4bool    Equals(const BDSAperture* other) const override;
  void      CheckInfoOK()                    const override;
  G4double  RadiusToEncompass()              const override {return std::hypot(a,b) + tiltOffset.Radius();}
  BDSExtent Extent()                         const override;
  G4double  MinimumInscribedCircleRadius()   const override {return std::min(a, b);}
  unsigned int MinimumNumberOfPoints()       const override {return 4;}

  BDSApertureRectangle        operator+ (G4double number) const;
  const BDSApertureRectangle& operator+=(G4double number);
  BDSApertureRectangle        operator* (G4double number) const;
  const BDSApertureRectangle& operator*=(G4double number);
  
  BDSAperture* Plus(G4double number) const override;
  BDSAperture* Times(G4double number) const override;
  BDSAperture* Clone() const override;
  std::array<G4double, 7> ApertureNumbers() const override;
  
  BDSPolygon PolygonNPoints(unsigned int nPointsIn) const override;
};

#endif
