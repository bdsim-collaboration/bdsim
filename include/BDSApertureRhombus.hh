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
#ifndef BDSAPERTURERHOMBUS_H
#define BDSAPERTURERHOMBUS_H

#include "BDSApertureCurved.hh"

#include "G4Types.hh"

#include <array>

class BDSExtent;
class BDSPolygon;

/**
 * @brief A diamond-shaped aperture with optional curved edges.
 *
 * Note, that when this is expanded the horizontal and vertical extent
 * will increase by more than that value. This is because the expansion
 * is done to keep a constant thickness around the edge but this results
 * in a greater extent at each edge (depending on the proportions).
 *
 * @author Laurie Nevay
 */

class BDSApertureRhombus: public BDSApertureCurved
{
public:
  BDSApertureRhombus() = delete;
  BDSApertureRhombus(G4double xIn,
                     G4double yIn,
                     G4double cornerRadiusIn = 0,
                     unsigned int nPointsPerTwoPiIn = 0);
  virtual ~BDSApertureRhombus(){;}

  G4double x;
  G4double y;
  G4double cornerRadius;

  G4bool    Equals(const BDSAperture* other) const override;
  void      CheckInfoOK()                    const override;
  G4double  RadiusToEncompass()              const override;
  BDSExtent Extent()                         const override;
  G4double  MinimumInscribedCircleRadius()   const override;
  unsigned int MinimumNumberOfPoints()       const override;

  BDSApertureRhombus        operator+ (G4double number) const;
  const BDSApertureRhombus& operator+=(G4double number);
  BDSApertureRhombus        operator* (G4double number) const;
  const BDSApertureRhombus& operator*=(G4double number);

  BDSAperture* Plus(G4double number) const override;
  BDSAperture* Times(G4double number) const override;
  BDSAperture* Clone() const override;
  std::array<G4double, 7> ApertureNumbers() const override;

  BDSPolygon PolygonNPoints(unsigned int nPointsIn) const override;

private:
  G4double nPointsMinimum;
};
#endif
