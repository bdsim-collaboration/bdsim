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
#include "BDSArray2DCoords.hh"
#include "BDSFieldMagSolenoidBlock.hh"
#include "BDSFieldMagSolenoidSheet.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolatorRoutines.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <tuple>
#include <utility>




BDSFieldMagSolenoidBlock::BDSFieldMagSolenoidBlock(BDSMagnetStrength const* strength,
                                                   G4double innerRadiusIn):
  BDSFieldMagSolenoidBlock((*strength)["field"], false, innerRadiusIn,
                           (*strength)["coilRadialThickness"], (*strength)["length"],
                           0.0, 0.0, 0.0, 0.0, 1)
{;}

BDSFieldMagSolenoidBlock::BDSFieldMagSolenoidBlock(G4double        strength,
                                                   G4bool          strengthIsCurrent,
                                                   G4double        innerRadiusIn,
                                                   G4double        radialThicknessIn,
                                                   G4double        fullLengthZIn,
                                                   G4double        tiltXIn,
                                                   G4double        tiltYIn,
                                                   G4double        tiltZIn,
                                                   G4double        toleranceIn,
                                                   G4int           nSheetsIn,
                                                   G4int           gridPointsPerMmIn,
                                                   const G4String& interpolatorIn):
  a(innerRadiusIn),
  radialThickness(radialThicknessIn),
  fullLengthZ(fullLengthZIn),
  halfLength(0.5 * fullLengthZIn),
  B0(0),
  I(0),
  currentDensity(0),
  spatialLimit(std::min(1e-5 * innerRadiusIn, 1e-5 * fullLengthZIn)),
  tiltX(tiltXIn),
  tiltY(tiltYIn),
  tiltZ(tiltZIn),
  coilTolerance(std::max(toleranceIn, 1e-6)),
  nSheetsBlock(nSheetsIn),
  zHalfExtent(std::numeric_limits<G4double>::max()),
  hasTilt(BDS::IsFinite(tiltXIn) || BDS::IsFinite(tiltYIn) || BDS::IsFinite(tiltZIn)),
  useGrid(false),
  interpolator(interpolatorIn),
  grid(nullptr)
{
  finiteStrength = BDS::IsFinite(std::abs(strength));
  if (strengthIsCurrent)
    {I = strength;}
  else
    {
      B0 = strength;
      I  = B0 * 2 * a / CLHEP::mu0;
    }

  currentDensity = I * radialThickness * fullLengthZ / nSheetsBlock;

  G4double dr = radialThickness / nSheetsBlock;
  auto innerCoil = std::make_unique<BDSFieldMagSolenoidSheet>(
    currentDensity * nSheetsBlock,
    true,
    a + 0.5 * dr,
    fullLengthZ,
    0.0, 0.0, 0.0,
    toleranceIn);
  zHalfExtent = innerCoil->GetZHalfExtent();

  if (hasTilt)
    {
      rotation.rotateZ(tiltZIn);
      rotation.rotateY(tiltYIn);
      rotation.rotateX(tiltXIn);
      inverseRotation = rotation.inverse();
    }

  if (gridPointsPerMmIn > 0)
    {
      grid    = GetGrid(a, radialThickness, fullLengthZ, nSheetsBlock,
                        currentDensity, zHalfExtent, spatialLimit, gridPointsPerMmIn);
      useGrid = true;
    }
}


std::pair<G4double, G4double>
BDSFieldMagSolenoidBlock::SumSheetFields(G4double rho, G4double z,
                                         G4double innerRadius,
                                         G4double radialThicknessIn,
                                         G4double fullLengthZIn,
                                         G4int    nSheets,
                                         G4double currentDensityIn,
                                         G4double spatialLimitIn)
{
  G4double hL    = 0.5 * fullLengthZIn;
  G4double dr    = radialThicknessIn / nSheets;
  G4double scale = CLHEP::mu0 * currentDensityIn / (CLHEP::pi * fullLengthZIn);

  G4double brSum = 0.0;
  G4double bzSum = 0.0;
  for (G4int s = 0; s < nSheets; s++)
    {
      G4double sheetA = innerRadius + (s + 0.5) * dr;
      auto [normBr, normBz] = BDSFieldMagSolenoidSheet::ComputeAnalyticField(
          rho, z, sheetA, hL, spatialLimitIn);
      brSum += normBr;
      bzSum += normBz;
    }
  return {scale * brSum, scale * bzSum};
}


BDSArray2DCoords* BDSFieldMagSolenoidBlock::BuildGrid(
    G4double innerRadius, G4double radialThicknessIn, G4double fullLengthZIn,
    G4int nSheets, G4double currentDensityIn, G4double zExtent, G4double spatialLimitIn,
    G4int pointsPerMm)
{
  const G4int NRho = std::max(2, (G4int)std::round(innerRadius) * pointsPerMm);
  const G4int NZ   = std::max(2, (G4int)std::round(zExtent)    * pointsPerMm);
  G4double rhoMax  = innerRadius;
  G4double zMax    = zExtent;
  G4double drho    = rhoMax / (NRho - 1);
  G4double dz      = 2.0 * zMax / (NZ - 1);

  auto* array = new BDSArray2DCoords(NRho, NZ, 0.0, rhoMax, -zMax, zMax);

  for (G4int iRho = 0; iRho < NRho; iRho++)
    {
      G4double rho = iRho * drho;
      for (G4int iZ = 0; iZ < NZ; iZ++)
        {
          G4double z = -zMax + iZ * dz;
          auto [Brho, Bz] = SumSheetFields(
              rho, z, innerRadius, radialThicknessIn, fullLengthZIn,
              nSheets, currentDensityIn, spatialLimitIn);
          (*array)(iRho, iZ) = BDSFieldValue(Brho, 0.0, Bz);
        }
    }

  return array;
}

BDSArray2DCoords* BDSFieldMagSolenoidBlock::GetGrid(
    G4double innerRadius, G4double radialThicknessIn, G4double fullLengthZIn,
    G4int nSheets, G4double currentDensityIn, G4double zExtent, G4double spatialLimitIn,
    G4int pointsPerMm)
{
  static std::map<std::tuple<G4double,G4double,G4double,G4int,G4double,G4int>,
                  std::unique_ptr<BDSArray2DCoords>> gridCache;

  G4double aKey  = std::round(innerRadius);
  G4double rtKey = std::round(radialThicknessIn);
  G4double lzKey = std::round(fullLengthZIn);
  G4double jKey  = std::round(currentDensityIn);

  auto key = std::make_tuple(aKey, rtKey, lzKey, nSheets, jKey, pointsPerMm);
  auto it = gridCache.find(key);
  if (it != gridCache.end())
    {return it->second.get();}

  auto result = gridCache.emplace(key,
      BuildGrid(innerRadius, radialThicknessIn, fullLengthZIn,
                nSheets, currentDensityIn, zExtent, spatialLimitIn, pointsPerMm));
  return result.first->second.get();
}


G4ThreeVector BDSFieldMagSolenoidBlock::GetField(const G4ThreeVector& position,
                                                 const G4double       /*t*/) const
{
  G4ThreeVector localPosition = hasTilt ? inverseRotation * position : position;
  G4double z   = localPosition.z();
  G4double rho = localPosition.perp();

  if (std::abs(z) > zHalfExtent)
    {return G4ThreeVector();}

  G4double Brho = 0.0;
  G4double Bz   = 0.0;

  if (useGrid && rho >= grid->XStep() && rho <= grid->XMax() && std::abs(z) <= grid->YMax())
    {
      G4double fx, fy;
      BDSFieldValue result;
      if (interpolator == "cubic")
        {
          BDSFieldValue localData[4][4];
          grid->ExtractSection4x4(rho, z, localData, fx, fy);
          result = BDS::Cubic2D(localData, fx, fy);
        }
      else
        {
          BDSFieldValue localData[2][2];
          grid->ExtractSection2x2(rho, z, localData, fx, fy);
          result = BDS::Linear2D(localData, fx, fy);
        }
      Brho = result.x();
      Bz   = result.z();
    }
  else
    {
      auto [brSum, bzSum] = SumSheetFields(
          rho, z, a, radialThickness, fullLengthZ,
          nSheetsBlock, currentDensity, spatialLimit);
      Brho = brSum;
      Bz   = bzSum;
    }

  G4double Bx = (rho > spatialLimit) ? Brho * localPosition.x() / rho : 0.0;
  G4double By = (rho > spatialLimit) ? Brho * localPosition.y() / rho : 0.0;
  G4ThreeVector localField(Bx, By, Bz);
  return hasTilt ? rotation * localField : localField;
}
