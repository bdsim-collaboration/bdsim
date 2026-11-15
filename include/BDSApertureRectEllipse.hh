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
#ifndef BDSAPERTURERECTELLIPSE_H
#define BDSAPERTURERECTELLIPSE_H
#include "BDSApertureCurved.hh"
#include "BDSTiltOffset.hh"

#include "G4Types.hh"

#include <algorithm>
#include <array>

class BDSAperture;
class BDSExtent;
class BDSPolygon;

/**
 * @brief RectEllipse aperture.
 *
 * @author Laurie Nevay
 */

class BDSApertureRectEllipse: public BDSApertureCurved
{
public:
  BDSApertureRectEllipse() = delete;
  explicit BDSApertureRectEllipse(G4double rectangleAIn,
                                  G4double rectangleBIn,
                                  G4double ellipseAIn,
                                  G4double ellipseBIn,
                                  unsigned int nPointsIn = 24);
  virtual ~BDSApertureRectEllipse(){;}

  G4double rectangleA;
  G4double rectangleB;
  G4double ellipseA;
  G4double ellipseB;

  G4bool    Equals(const BDSAperture* other) const override;
  void      CheckInfoOK()                    const override;
  G4double  RadiusToEncompass()              const override {return std::max(ellipseA, ellipseB) + tiltOffset.Radius();}
  BDSExtent Extent()                         const override;
  G4double  MaximumInscribedCircleRadius()   const override;
  unsigned int MinimumNumberOfPoints()       const override {return 24;}

  BDSApertureRectEllipse        operator+ (G4double number) const;
  const BDSApertureRectEllipse& operator+=(G4double number);
  BDSApertureRectEllipse        operator* (G4double number) const;
  const BDSApertureRectEllipse& operator*=(G4double number);
  
  BDSAperture* Plus(G4double number) const override;
  BDSAperture* Times(G4double number) const override;
  BDSAperture* Clone() const override;
  std::array<G4double, 7> ApertureNumbers() const override;
  
  BDSPolygon PolygonNPoints(unsigned int nPointsIn) const override;
};

#endif
