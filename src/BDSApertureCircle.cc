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
#include "BDSAperture.hh"
#include "BDSApertureCurved.hh"
#include "BDSApertureCircle.hh"
#include "BDSApertureType.hh"
#include "BDSExtent.hh"
#include "BDSPolygon.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "G4TwoVector.hh"
#include "G4Types.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <array>
#include <cmath>
#include <vector>

BDSApertureCircle::BDSApertureCircle(G4double     radiusIn,
                                     unsigned int nPointsIn):
  BDSApertureCurved(BDSApertureType::circle, nPointsIn),
  radius(radiusIn)
{;}

BDSApertureCircle::BDSApertureCircle(const BDSApertureCircle& other):
  BDSApertureCurved(other),
  radius(other.radius)
{;}

BDSApertureCircle& BDSApertureCircle::operator=(const BDSApertureCircle& other) noexcept
{
  if (this != &other)
    {
      BDSApertureCurved::operator=(other);
      radius = other.radius;
    }
  return *this;
}

G4bool BDSApertureCircle::Equals(const BDSAperture* other) const
{
  if (!other)
    {return false;}
  else if (other->apertureType != apertureType)
    {return false;}
  else if (other->tiltOffset != tiltOffset)
    {return false;}
  else
    {
      const BDSApertureCircle* oc = dynamic_cast<const BDSApertureCircle*>(other);
      return BDS::DoublesAreEqual(oc->radius, radius);
    }
}

void BDSApertureCircle::CheckInfoOK() const
{
  CheckRequiredParametersSet(radius, true);
  CheckParameterIsPositive(radius, "radius");
}

BDSExtent BDSApertureCircle::Extent() const
{
  BDSExtent simpleExtent(radius, radius, 0);
  return ExtentOffsetTilt(simpleExtent);
}

BDSApertureCircle BDSApertureCircle::operator+ (G4double number) const
{
  BDSApertureCircle result = BDSApertureCircle(*this);
  result += number;
  return result;
}

const BDSApertureCircle& BDSApertureCircle::operator+=(G4double number)
{
  radius += number;
  return *this;
}

BDSApertureCircle BDSApertureCircle::operator* (G4double number) const
{
  BDSApertureCircle result = BDSApertureCircle(*this);
  result *= number;
  return result;
}

const BDSApertureCircle& BDSApertureCircle::operator*=(G4double number)
{
  radius *= number;
  return *this;
}

BDSAperture* BDSApertureCircle::Plus(G4double number) const
{
  BDSApertureCircle result = (*this) + number;
  return new BDSApertureCircle(result);
}

BDSAperture* BDSApertureCircle::Times(G4double number) const
{
  BDSApertureCircle result = (*this) * number;
  return new BDSApertureCircle(result);
}

BDSAperture* BDSApertureCircle::Clone() const
{
  return new BDSApertureCircle(*this);
}

std::array<G4double,7> BDSApertureCircle::ApertureNumbers() const
{
  return {radius,0,0,0,tiltOffset.OffsetX(),tiltOffset.OffsetY(),tiltOffset.Tilt()};
}

BDSPolygon BDSApertureCircle::PolygonNPoints(unsigned int nPointsIn) const
{
  std::vector<G4TwoVector> r;
  r.reserve(nPointsIn);
  G4double dTheta = CLHEP::twopi / (G4double)nPointsIn;
  for (unsigned int i = 0; i < nPointsIn; i++)
    {r.emplace_back(radius * std::cos(i*dTheta), radius * std::sin(i*dTheta));}
  return BDSPolygon(r);
}
