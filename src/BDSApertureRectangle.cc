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
#include "BDSApertureRectangle.hh"
#include "BDSApertureType.hh"
#include "BDSExtent.hh"
#include "BDSPolygon.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "G4Types.hh"

#include <array>
#include <vector>

BDSApertureRectangle::BDSApertureRectangle(G4double aIn,
                                           G4double bIn):
  BDSAperture(BDSApertureType::rectangle),
  a(aIn),
  b(bIn)
{;}

G4bool BDSApertureRectangle::Equals(const BDSAperture* other) const
{
  if (!other)
    {return false;}
  else if (other->apertureType != apertureType)
    {return false;}
  else if (other->tiltOffset != tiltOffset)
    {return false;}
  else
    {
      const BDSApertureRectangle* oc = dynamic_cast<const BDSApertureRectangle*>(other);
      return BDS::DoublesAreEqual(oc->a, a) && BDS::DoublesAreEqual(oc->b, b);
    }
}

void BDSApertureRectangle::CheckInfoOK() const
{
  CheckRequiredParametersSet(a, true, b, true);
  CheckParameterIsPositive(a, "a");
  CheckParameterIsPositive(b, "b");
}

BDSExtent BDSApertureRectangle::Extent() const
{
  BDSExtent simpleExtent(a, b, 0);
  return ExtentOffsetTilt(simpleExtent);
}

BDSApertureRectangle BDSApertureRectangle::operator+ (G4double number) const
{
  BDSApertureRectangle result = BDSApertureRectangle(*this);
  result += number;
  return result;
}

const BDSApertureRectangle& BDSApertureRectangle::operator+=(G4double number)
{
  a += number;
  b += number;
  return *this;
}

BDSApertureRectangle BDSApertureRectangle::operator* (G4double number) const
{
  BDSApertureRectangle result = BDSApertureRectangle(*this);
  result *= number;
  return result;
}

const BDSApertureRectangle& BDSApertureRectangle::operator*=(G4double number)
{
  a *= number;
  b *= number;
  return *this;
}

BDSAperture* BDSApertureRectangle::Plus(G4double number) const
{
  BDSApertureRectangle result = (*this) + number;
  return new BDSApertureRectangle(result);
}

BDSAperture* BDSApertureRectangle::Times(G4double number) const
{
  BDSApertureRectangle result = (*this) * number;
  return new BDSApertureRectangle(result);
}

BDSAperture* BDSApertureRectangle::Clone() const
{
  return new BDSApertureRectangle(*this);
}

std::array<G4double,7> BDSApertureRectangle::ApertureNumbers() const
{
  return {a,b,0,0,tiltOffset.OffsetX(),tiltOffset.OffsetY(),tiltOffset.Tilt()};
}

BDSPolygon BDSApertureRectangle::PolygonNPoints(unsigned int nPointsIn) const
{
  std::vector<G4TwoVector> r;
  r.emplace_back( a,  b);
  r.emplace_back(-a,  b);
  r.emplace_back(-a, -b);
  r.emplace_back( a, -b);

  if (nPointsIn == 4)
    {return BDSPolygon(r);}
  else
    {
      nPointsIn = BDS::NextMultiple(nPointsIn, 4); // ensure multiple of 8
      unsigned int nPointsPerEdge = nPointsIn / 4;
      std::vector<G4TwoVector> result;
      for (G4int i=0; i < 8; i++)
        {AddPointsOnLineUpTo(result, r[i], r[(i+1)%4], nPointsPerEdge);}
      return BDSPolygon(result);
    }
}
