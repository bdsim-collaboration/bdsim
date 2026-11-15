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
#ifndef BDSAPERTURERACETRACK_H
#define BDSAPERTURERACETRACK_H
#include "BDSApertureCurved.hh"
#include "BDSTiltOffset.hh"

#include "G4Types.hh"

#include <algorithm>
#include <array>

class BDSAperture;
class BDSExtent;
class BDSPolygon;

/**
 * @brief Racetrack aperture.
 *
 * @author Laurie Nevay
 */

class BDSApertureRaceTrack: public BDSApertureCurved
{
public:
  BDSApertureRaceTrack() = delete;
  BDSApertureRaceTrack(G4double xIn,
                       G4double yIn,
                       G4double radiusIn,
                       unsigned int nPointsIn);
  virtual ~BDSApertureRaceTrack(){;}

  G4double x;
  G4double y;
  G4double radius;

  G4bool    Equals(const BDSAperture* other) const override;
  void      CheckInfoOK()                    const override;
  G4double  RadiusToEncompass()              const override {return radius + tiltOffset.Radius();}
  BDSExtent Extent()                         const override;
  G4double  MaximumInscribedCircleRadius()   const override {return std::min(x, y);}
  unsigned int MinimumNumberOfPoints()       const override {return 16;}

  BDSApertureRaceTrack        operator+ (G4double number) const;
  const BDSApertureRaceTrack& operator+=(G4double number);
  BDSApertureRaceTrack        operator* (G4double number) const;
  const BDSApertureRaceTrack& operator*=(G4double number);
  
  BDSAperture* Plus(G4double number) const override;
  BDSAperture* Times(G4double number) const override;
  BDSAperture* Clone() const override;
  std::array<G4double, 7> ApertureNumbers() const override;
  
  BDSPolygon PolygonNPoints(unsigned int nPointsIn) const override;
};

#endif
