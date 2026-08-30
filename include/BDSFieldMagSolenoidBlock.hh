/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2022.

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
#ifndef BDSFIELDMAGSOLENOIDBLOCK_H
#define BDSFIELDMAGSOLENOIDBLOCK_H

#include "BDSFieldMag.hh"

#include "G4RotationMatrix.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <utility>

class BDSArray2DCoords;
class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic field due to a square annulus of current.
 *
 * Looking in the z,r plane we would see two square blocks with an inner edge at +-r
 * and centered at z = 0. In r, the square extends from innerRadiusIn to innerRadiusIn+radialThicknessIn.
 * There is a +- this in r with the top one being the sign of the strength and the bottom one
 * being the return of the annulus with current in the opposite direction. In phi, it is circular.
 *
 * @author Rohan Kamath, Paul Bogdan Jurj
 */

class BDSFieldMagSolenoidBlock: public BDSFieldMag
{
public:
  BDSFieldMagSolenoidBlock() = delete;
  /// This constructor uses "field" and geometry parameters from BDSMagnetStrength.
  BDSFieldMagSolenoidBlock(BDSMagnetStrength const* strength,
                           G4double innerRadiusIn);
  /// Explicit constructor. gridPointsPerMmIn=0 means no grid (analytic only).
  BDSFieldMagSolenoidBlock(G4double        strength,
                           G4bool          strengthIsCurrent,
                           G4double        innerRadiusIn,
                           G4double        radialThicknessIn,
                           G4double        fullLengthZIn,
                           G4double        tiltXIn,
                           G4double        tiltYIn,
                           G4double        tiltZIn,
                           G4double        toleranceIn,
                           G4int           nSheetsIn,
                           G4double        gridPointsPerMmIn = 1.0,
                           const G4String& interpolatorIn    = "linear");
  virtual ~BDSFieldMagSolenoidBlock() = default;

  /// Calculate the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
                                 const G4double       t = 0) const;

  /// @{ Accessor.
  inline G4double GetB0()          const {return B0;}
  inline G4double GetI()           const {return I;}
  inline G4double GetZHalfExtent() const {return zHalfExtent;}
  /// @}

private:
  static std::pair<G4double,G4double> SumSheetFields(
      G4double rho, G4double z,
      G4double innerRadius, G4double radialThickness, G4double fullLengthZ,
      G4int nSheets, G4double currentDensity, G4double spatialLimit);

  static BDSArray2DCoords* BuildGrid(
      G4double innerRadius, G4double radialThickness, G4double fullLengthZ,
      G4int nSheets, G4double currentDensity,
      G4double zHalfExtent, G4double spatialLimit,
      G4double pointsPerMm);

  static BDSArray2DCoords* GetGrid(
      G4double innerRadius, G4double radialThickness, G4double fullLengthZ,
      G4int nSheets, G4double currentDensity,
      G4double zHalfExtent, G4double spatialLimit,
      G4double pointsPerMm);

  G4double a;
  G4double radialThickness;
  G4double fullLengthZ;
  G4double halfLength;
  G4double B0;
  G4double I;
  G4double currentDensity;
  G4double spatialLimit;
  G4double tiltX;
  G4double tiltY;
  G4double tiltZ;
  G4double coilTolerance;
  G4int    nSheetsBlock;
  G4double zHalfExtent; ///< z beyond which block field is negligible.
  G4bool   hasTilt;
  G4RotationMatrix rotation;
  G4RotationMatrix inverseRotation;
  G4bool            useGrid;
  G4String          interpolator;
  BDSArray2DCoords* grid; ///< Not owned; shared via static cache.
};

#endif
