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
#include "BDSApertureEllipse.hh"
#include "BDSApertureType.hh"
#include "BDSExtent.hh"
#include "BDSPolygon.hh"
#include "BDSUtilities.hh"

#include "G4TwoVector.hh"
#include "G4Types.hh"

#include <array>
#include <cmath>
#include <vector>

BDSApertureEllipse::BDSApertureEllipse(G4double aIn,
                                       G4double bIn,
                                       unsigned int nPointsIn):
  BDSApertureCurved(BDSApertureType::ellipse, nPointsIn),
  a(aIn),
  b(bIn)
{;}

G4bool BDSApertureEllipse::Equals(const BDSAperture* other) const
{
  if (!other)
    {return false;}
  else if (other->apertureType != apertureType)
    {return false;}
  else if (other->tiltOffset != tiltOffset)
    {return false;}
  else
    {
      const BDSApertureEllipse* oc = dynamic_cast<const BDSApertureEllipse*>(other);
      return BDS::DoublesAreEqual(oc->a, a) && BDS::DoublesAreEqual(oc->b, b);
    }
}

void BDSApertureEllipse::CheckInfoOK() const
{
  CheckRequiredParametersSet(a, true, b, true);
  CheckParameterIsPositive(a, "a");
  CheckParameterIsPositive(b, "b");
}

BDSExtent BDSApertureEllipse::Extent() const
{
  BDSExtent simpleExtent(a, b, 0);
  return ExtentOffsetTilt(simpleExtent);
}

BDSApertureEllipse BDSApertureEllipse::operator+ (G4double number) const
{
  BDSApertureEllipse result = BDSApertureEllipse(*this);
  result += number;
  return result;
}

const BDSApertureEllipse& BDSApertureEllipse::operator+=(G4double number)
{
  a += number;
  b += number;
  return *this;
}

BDSApertureEllipse BDSApertureEllipse::operator* (G4double number) const
{
  BDSApertureEllipse result = BDSApertureEllipse(*this);
  result *= number;
  return result;
}

const BDSApertureEllipse& BDSApertureEllipse::operator*=(G4double number)
{
  a *= number;
  b *= number;
  return *this;
}

BDSAperture* BDSApertureEllipse::Plus(G4double number) const
{
  BDSApertureEllipse result = (*this) + number;
  return new BDSApertureEllipse(result);
}

BDSAperture* BDSApertureEllipse::Times(G4double number) const
{
  BDSApertureEllipse result = (*this) * number;
  return new BDSApertureEllipse(result);
}

BDSAperture* BDSApertureEllipse::Clone() const
{
  return new BDSApertureEllipse(*this);
}

std::array<G4double,7> BDSApertureEllipse::ApertureNumbers() const
{
  return {a,b,0,0,tiltOffset.OffsetX(),tiltOffset.OffsetY(),tiltOffset.Tilt()};
}

BDSPolygon BDSApertureEllipse::PolygonNPoints(unsigned int nPointsIn) const
{
  std::vector<G4TwoVector> r;
  r.reserve(nPointsIn);
  G4double dTheta = CLHEP::twopi / (G4double)nPointsIn;
  for (unsigned int i = 0; i < nPointsIn; i++)
    {r.emplace_back(a * std::cos(i*dTheta), b * std::sin(i*dTheta));}
  return BDSPolygon(r);
}
