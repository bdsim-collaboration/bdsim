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
#ifndef BDSAPERTUREPOINTS_H
#define BDSAPERTUREPOINTS_H

#include "BDSAperture.hh"
#include "BDSTiltOffset.hh"

#include "G4Types.hh"

#include <array>
#include <cmath>

class BDSExtent;
class BDSPolygon;

/**
 * @brief Points aperture.
 *
 * Note, this does not load the aperture data in the constructor but relies
 * on the LoadData() function to be called. Ideally, this happens after CheckInfoOk().
 *
 * @author Laurie Nevay
 */

class BDSAperturePoints: public BDSAperture
{
public:
  BDSAperturePoints() = delete;
  BDSAperturePoints(const G4String& pointsFileNameIn,
                    const G4String& unitsStrIn);
  BDSAperturePoints(const BDSAperturePoints& other);
  virtual ~BDSAperturePoints() noexcept;

  G4String pointsFileName;
  G4String unitsStr;

  G4bool    Equals(const BDSAperture* other) const override;
  void      CheckInfoOK()                    const override;
  G4double  RadiusToEncompass()              const override;
  BDSExtent Extent()                         const override;
  G4double  MinimumInscribedCircleRadius()              const override;
  unsigned int MinimumNumberOfPoints()       const override;

  BDSAperturePoints        operator+ (G4double number) const;
  const BDSAperturePoints& operator+=(G4double number);
  BDSAperturePoints        operator* (G4double number) const;
  const BDSAperturePoints& operator*=(G4double number);

  /// Load the aperture data.
  void LoadData() override;

  BDSAperture* Plus(G4double number) const override;
  BDSAperture* Times(G4double number) const override;
  BDSAperture* Clone() const override;
  std::array<G4double, 7> ApertureNumbers() const override;
  
  BDSPolygon PolygonNPoints(unsigned int nPointsIn) const override;

protected:
  BDSPolygon* points;
};

#endif
