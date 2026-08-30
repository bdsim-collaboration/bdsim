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
#include "BDSArray2DCoords.hh"
#include "BDSFieldMagDipoleEnge.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolatorRoutines.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <algorithm>
#include <cmath>
#include <map>
#include <memory>
#include <tuple>




BDSFieldMagDipoleEnge::BDSFieldMagDipoleEnge(G4double        strength,
                                             G4double        apertureRadius,
                                             G4double        coilLength,
                                             G4double        engeCoefficient,
                                             G4bool          useGridIn,
                                             G4double        gridPointsPerMmIn,
                                             const G4String& interpolatorIn):
  D(2*apertureRadius),
  halfLength(0.5*coilLength),
  B0(strength),
  engeCoeff(engeCoefficient),
  engeOverD(engeCoefficient / (2*apertureRadius)),
  normalisation(0.0),
  useGrid(false),
  interpolator(interpolatorIn),
  grid(nullptr)
{
  normalisation = B0 / QueryField(0.0, 0.0).y();

  if (useGridIn && gridPointsPerMmIn > 0)
    {
      grid    = GetGrid(D, halfLength, engeOverD, GetZHalfExtent(), gridPointsPerMmIn);
      useGrid = true;
    }
}

BDSArray2DCoords* BDSFieldMagDipoleEnge::BuildGrid(G4double D,
                                                   G4double halfLength,
                                                   G4double engeOverD,
                                                   G4double zHalfExtent,
                                                   G4double pointsPerMm)
{
  G4double yMax = 0.5 * D;
  G4double zMax = zHalfExtent;

  const G4int NY = std::max(2, (G4int)(std::round(2.0 * yMax) * pointsPerMm));
  const G4int NZ = std::max(2, (G4int)(std::round(2.0 * zMax) * pointsPerMm));

  G4double dy = 2.0 * yMax / (NY - 1);
  G4double dz = 2.0 * zMax / (NZ - 1);

  auto* array = new BDSArray2DCoords(NY, NZ, -yMax, yMax, -zMax, zMax);

  for (G4int iY = 0; iY < NY; iY++)
    {
      G4double y = -yMax + iY * dy;
      for (G4int iZ = 0; iZ < NZ; iZ++)
        {
          G4double z = -zMax + iZ * dz;

          G4double zleft  = z + halfLength + D;
          G4double zright = z - halfLength - D;
          G4double cosY   = std::cos(y * engeOverD);
          G4double sinY   = std::sin(y * engeOverD);
          G4double eL     = std::exp(-zleft  * engeOverD);
          G4double eR     = std::exp( zright * engeOverD);

          G4double denomL = 1 + 2*eL*cosY + eL*eL;
          G4double denomR = 1 + 2*eR*cosY + eR*eR;

          G4double By_unnorm = (1 + eL*cosY) / denomL + (1 + eR*cosY) / denomR - 1.0;
          G4double Bz_unnorm = (    eL*sinY) / denomL + (   -eR*sinY) / denomR;

          (*array)(iY, iZ) = BDSFieldValue(0.0, By_unnorm, Bz_unnorm);
        }
    }

  return array;
}

BDSArray2DCoords* BDSFieldMagDipoleEnge::GetGrid(G4double D,
                                                 G4double halfLength,
                                                 G4double engeOverD,
                                                 G4double zHalfExtent,
                                                 G4double pointsPerMm)
{
  static std::map<std::tuple<G4double,G4double,G4double,G4double>,
                  std::unique_ptr<BDSArray2DCoords>> gridCache;

  auto key = std::make_tuple(std::round(D),
                             std::round(halfLength),
                             std::round(engeOverD * 1e6),
                             pointsPerMm);
  auto it = gridCache.find(key);
  if (it != gridCache.end())
    {return it->second.get();}

  auto result = gridCache.emplace(key, BuildGrid(D, halfLength, engeOverD, zHalfExtent, pointsPerMm));
  return result.first->second.get();
}

G4ThreeVector BDSFieldMagDipoleEnge::GetField(const G4ThreeVector& position,
                                              const G4double       /*t*/) const
{
  G4double z   = position.z();
  G4double y   = position.y();
  G4double rho = position.perp();

  G4double By = 0;
  G4double Bz = 0;

  if (std::abs(z) > halfLength + 5.0*D)
    {return G4ThreeVector();}
  if (rho > D*0.5)
    {return G4ThreeVector();}

  if (useGrid)
    {
      G4double fx, fy;
      BDSFieldValue result;
      if (interpolator == "cubic")
        {
          BDSFieldValue localData[4][4];
          grid->ExtractSection4x4(y, z, localData, fx, fy);
          result = BDS::Cubic2D(localData, fx, fy);
        }
      else
        {
          BDSFieldValue localData[2][2];
          grid->ExtractSection2x2(y, z, localData, fx, fy);
          result = BDS::Linear2D(localData, fx, fy);
        }
      By = result.y() * normalisation;
      Bz = result.z() * normalisation;
    }
  else
    {
      G4double zleft  = z + halfLength + D;
      G4double zright = z - halfLength - D;

      G4double cosY = std::cos(y * engeOverD);
      G4double sinY = std::sin(y * engeOverD);
      G4double eL   = std::exp(-zleft  * engeOverD);
      G4double eR   = std::exp( zright * engeOverD);

      G4double denomL = 1 + 2*eL*cosY + eL*eL;
      G4double denomR = 1 + 2*eR*cosY + eR*eR;

      G4double By_left  = (1 + eL*cosY) / denomL;
      G4double By_right = (1 + eR*cosY) / denomR;
      G4double Bz_left  = (    eL*sinY) / denomL;
      G4double Bz_right = (   -eR*sinY) / denomR;

      By = (By_left + By_right - 1.0) * normalisation;
      Bz = (Bz_left + Bz_right)       * normalisation;
    }

  return G4ThreeVector(0, By, Bz);
}


G4ThreeVector BDSFieldMagDipoleEnge::QueryField(G4double y, G4double z) const
{
  G4double zleft  = z + halfLength + D;
  G4double zright = z - halfLength - D;
  G4double cosY   = std::cos(y * engeOverD);
  G4double eL     = std::exp(-zleft  * engeOverD);
  G4double eR     = std::exp( zright * engeOverD);

  G4double By_left  = (1 + eL*cosY) / (1 + 2*eL*cosY + eL*eL);
  G4double By_right = (1 + eR*cosY) / (1 + 2*eR*cosY + eR*eR);

  return G4ThreeVector(0, By_left + By_right - 1.0, 0);
}
