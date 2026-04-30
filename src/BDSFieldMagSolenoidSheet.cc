/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSDebug.hh"
#include "BDSFieldMagSolenoidSheet.hh"
#include "BDSMagnetStrength.hh"
#include "BDSSpecialFunctions.hh"
#include "BDSUtilities.hh"
#include "BDSArray2DCoords.hh"
#include "BDSInterpolatorRoutines.hh"
#include "BDSFieldValue.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <tuple>
#include <memory>
#include <utility>
#include <vector>


std::pair<G4double,G4double> BDSFieldMagSolenoidSheet::ComputeAnalyticField(
    G4double rho, G4double z, G4double a, G4double halfLength, G4double spatialLimit)
{
  G4double zp = z + halfLength;
  G4double zm = z - halfLength;
  if (std::abs(rho - a) < spatialLimit && std::abs(z) < halfLength + 2*spatialLimit)
    {return {0, 0};}
  if (std::abs(rho) < spatialLimit)
    {
      G4double f1 = zp / std::sqrt(zp*zp + a*a);
      G4double f2 = zm / std::sqrt(zm*zm + a*a);
      return {0, 0.5 * CLHEP::pi * (f1 - f2)};
    }
  G4double zpSq        = zp*zp,        zmSq        = zm*zm;
  G4double rhoPlusA    = rho + a,       rhoPlusASq  = rhoPlusA * rhoPlusA;
  G4double aMinusRho   = a - rho,       aMinusRhoSq = aMinusRho * aMinusRho;
  G4double denominatorP = std::sqrt(zpSq + rhoPlusASq);
  G4double denominatorM = std::sqrt(zmSq + rhoPlusASq);
  G4double alphap = a / denominatorP,   alpham = a / denominatorM;
  G4double betap  = zp / denominatorP,  betam  = zm / denominatorM;
  G4double gamma  = aMinusRho / rhoPlusA;
  G4double gammaSq = gamma * gamma;
  G4double kp = std::sqrt(zpSq + aMinusRhoSq) / denominatorP;
  G4double km = std::sqrt(zmSq + aMinusRhoSq) / denominatorM;
  G4double Brho = alphap * BDS::CEL(kp, 1, 1, -1) - alpham * BDS::CEL(km, 1, 1, -1);
  G4double Bz   = (a / rhoPlusA) * (betap * BDS::CEL(kp, gammaSq, 1, gamma)
                                  - betam * BDS::CEL(km, gammaSq, 1, gamma));
  if (std::isnan(Brho)) {Brho = 0;}
  if (std::isnan(Bz))   {Bz   = 1.0;}
  return {Brho, Bz};
}

BDSArray2DCoords* BDSFieldMagSolenoidSheet::BuildGrid(
    G4double a, G4double halfLength, G4double zHalfExtent, G4double spatialLimit, G4double pointsPerMm)
{
  const G4int NRho = std::max(2, (G4int)(std::round(a)               * pointsPerMm));
  const G4int NZ   = std::max(2, (G4int)(std::round(2.0 * zHalfExtent) * pointsPerMm));
  G4double rhoMax  = a;
  G4double zMax    = zHalfExtent;
  G4double drho    = rhoMax / (NRho - 1);
  G4double dz      = 2.0 * zMax / (NZ - 1);
  auto* array = new BDSArray2DCoords(NRho, NZ, 0.0, rhoMax, -zMax, zMax);
  
  for (G4int iRho = 0; iRho < NRho; iRho++)
    {
      G4double rho = iRho * drho;
      for (G4int iZ = 0; iZ < NZ; iZ++)
        {
          G4double z = -zMax + iZ * dz;
          auto [normBrho, normBz] = BDSFieldMagSolenoidSheet::ComputeAnalyticField(rho, z, a, halfLength, spatialLimit);
          (*array)(iRho, iZ) = BDSFieldValue(normBrho, 0.0, normBz);
        }
    }

  return array;
}

BDSArray2DCoords* BDSFieldMagSolenoidSheet::GetGrid(
    G4double a, G4double halfLength, G4double /*B0*/, G4double zHalfExtent, G4double spatialLimit, G4double pointsPerMm)
{
  static std::map<std::tuple<G4double,G4double,G4double,G4double>,
                  std::unique_ptr<BDSArray2DCoords>> gridCache;
  G4double aKey             = std::round(a);
  G4double halfLengthKey    = std::round(halfLength);
  G4double zHalfExtentKey   = std::round(zHalfExtent);

  auto key = std::make_tuple(aKey, halfLengthKey, zHalfExtentKey, pointsPerMm);
  auto it = gridCache.find(key);
  if (it != gridCache.end())
    {return it->second.get();}
  auto result = gridCache.emplace(key, BuildGrid(a, halfLength, zHalfExtent, spatialLimit, pointsPerMm));
  return result.first->second.get();
}

BDSFieldMagSolenoidSheet::BDSFieldMagSolenoidSheet(BDSMagnetStrength const* strength,
                                                   G4double radiusIn,
                                                   G4double toleranceIn):
  BDSFieldMagSolenoidSheet((*strength)["field"], false, radiusIn, (*strength)["length"], 0.0, 0.0, 0.0, toleranceIn)
{;}

BDSFieldMagSolenoidSheet::BDSFieldMagSolenoidSheet(G4double        strength,
                                                   G4bool          strengthIsCurrent,
                                                   G4double        sheetRadius,
                                                   G4double        fullLength,
                                                   G4double        tiltXIn,
                                                   G4double        tiltYIn,
                                                   G4double        tiltZIn,
                                                   G4double        toleranceIn,
                                                   G4double        gridPointsPerMmIn,
                                                   const G4String& interpolatorIn):
  a(sheetRadius),
  halfLength(0.5*fullLength),
  B0(0.0),
  I(0.0),
  spatialLimit(std::min(1e-5*sheetRadius, 1e-5*fullLength)),
  coilTolerance(std::max(1e-6, toleranceIn)),
  zHalfExtent(std::numeric_limits<G4double>::max()),
  hasTilt(BDS::IsFinite(tiltXIn) || BDS::IsFinite(tiltYIn) || BDS::IsFinite(tiltZIn)),
  useGrid(false),
  interpolator(interpolatorIn),
  grid(nullptr)
{
  finiteStrength = BDS::IsFinite(std::abs(strength));
  if (strengthIsCurrent)
    {
      I = strength;
      B0 = CLHEP::mu0 * strength / (CLHEP::pi*2* halfLength);
    }
  else
    {
      B0 = strength;
      I = B0 *(CLHEP::pi*2* halfLength) / CLHEP::mu0;
    }

  zHalfExtent = GetZHalfExtent() + 2*halfLength;

  if (hasTilt)
    {
      rotation.rotateZ(tiltZIn);
      rotation.rotateY(tiltYIn);
      rotation.rotateX(tiltXIn);
      inverseRotation = rotation.inverse();
    }

  if (gridPointsPerMmIn > 0)
    {
      grid = GetGrid(a, halfLength, B0, zHalfExtent, spatialLimit, gridPointsPerMmIn);
      useGrid = true;
    }
}

BDSFieldMagSolenoidSheet::~BDSFieldMagSolenoidSheet()
{
}

G4ThreeVector BDSFieldMagSolenoidSheet::GetField(const G4ThreeVector& position,
                                                 const G4double       /*t*/) const
{
  G4ThreeVector localPosition = hasTilt ? inverseRotation * position : position;
  G4double z = localPosition.z();
  G4double rho = localPosition.perp();

  if (std::abs(rho - a) < spatialLimit && (std::abs(z) < zHalfExtent))
    {return G4ThreeVector();}

  G4double Brho = 0.0;
  G4double Bz   = 0.0;

  if (rho < spatialLimit || (useGrid && rho < grid->XStep()))
    {
      Bz = OnAxisBz(z);
    }
  else if (useGrid)
  {
    if (rho <= grid->XMax() && std::abs(z) <= grid->YMax())
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
        Brho = B0 * result.x();
        Bz   = B0 * result.z();
      }
    else
      {return G4ThreeVector();}
  }

  else
    {
      auto [normBrho, normBz] = ComputeAnalyticField(rho, z, a, halfLength, spatialLimit);
      Brho = B0 * normBrho;
      Bz   = B0 * normBz;
    }

  G4double Bx = (rho > spatialLimit) ? Brho * localPosition.x() / rho : 0.0;
  G4double By = (rho > spatialLimit) ? Brho * localPosition.y() / rho : 0.0;
  G4ThreeVector localField = G4ThreeVector(Bx, By, Bz);

  return hasTilt ? rotation * localField : localField;
}

G4double BDSFieldMagSolenoidSheet::OnAxisBz(G4double z) const
{
  G4double zp = z + halfLength;
  G4double zm = z - halfLength;
  G4double f1 = zp / std::sqrt( zp*zp + a*a );
  G4double f2 = zm / std::sqrt( zm*zm + a*a );
  G4double Bz = 0.5*B0 *CLHEP::pi* (f1 - f2);
  return Bz;
}

G4double BDSFieldMagSolenoidSheet::GetZHalfExtent() const
{

  G4double L      = 2.0 * halfLength;
  G4double absB0  = std::abs(B0);
  G4double cubed  = (absB0 * CLHEP::pi * a * a * L) / (2.0 * std::max(coilTolerance, 1e-6));

  if (!BDS::IsFinite(cubed) || cubed <= 0)
    {return halfLength;} 

  G4double z = std::cbrt(cubed);

  return std::max(z, halfLength);
}