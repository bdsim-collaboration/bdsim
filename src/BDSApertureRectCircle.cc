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
#include "BDSApertureRectangle.hh"
#include "BDSApertureRectCircle.hh"
#include "BDSApertureType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSPolygon.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "G4Types.hh"

#include <algorithm>
#include <array>
#include <cmath>

BDSApertureRectCircle::BDSApertureRectCircle(G4double aIn,
                                             G4double bIn,
                                             G4double radiusIn,
                                             unsigned int nPointsIn):
  BDSApertureCurved(BDSApertureType::rectcircle, nPointsIn),
  a(aIn),
  b(bIn),
  radius(radiusIn)
{;}

G4bool BDSApertureRectCircle::Equals(const BDSAperture* other) const
{
  if (!other)
    {return false;}
  else if (other->apertureType != apertureType)
    {return false;}
  else if (other->tiltOffset != tiltOffset)
    {return false;}
  else
    {
      const BDSApertureRectCircle* oc = dynamic_cast<const BDSApertureRectCircle*>(other);
      return BDS::DoublesAreEqual(oc->a, a) &&
	         BDS::DoublesAreEqual(oc->b, b) &&
	         BDS::DoublesAreEqual(oc->radius, radius);
    }
}

void BDSApertureRectCircle::CheckInfoOK() const
{
  CheckRequiredParametersSet(a, true, b, true, radius, true);
  CheckParameterIsPositive(a, "a");
  CheckParameterIsPositive(b, "b");
  CheckParameterIsPositive(radius, "radius");
  
  if (radius < std::min(a, b)) // error if just a circle
    {throw BDSException(__METHOD_NAME__, "\"aper3\" is smaller than \"aper1\" and \"aper2\" - the result is only a circle");}
  
  if (std::hypot(a, b) <= radius) // error if rectangle only
    {throw BDSException(__METHOD_NAME__, "\"aper3\" is bigger than both \"aper1\" and \"aper2\" - result is a rectangle");}
}

BDSExtent BDSApertureRectCircle::Extent() const
{
  BDSExtent simpleExtent(std::min(a, radius), std::min(b, radius), 0);
  return ExtentOffsetTilt(simpleExtent);
}

BDSApertureRectCircle BDSApertureRectCircle::operator+ (G4double number) const
{
  BDSApertureRectCircle result = BDSApertureRectCircle(*this);
  result += number;
  return result;
}

const BDSApertureRectCircle& BDSApertureRectCircle::operator+=(G4double number)
{
  a += number;
  b += number;
  radius += number;
  return *this;
}

BDSApertureRectCircle BDSApertureRectCircle::operator* (G4double number) const
{
  BDSApertureRectCircle result = BDSApertureRectCircle(*this);
  result *= number;
  return result;
}

const BDSApertureRectCircle& BDSApertureRectCircle::operator*=(G4double number)
{
  a *= number;
  b *= number;
  radius *= number;
  return *this;
}

BDSAperture* BDSApertureRectCircle::Plus(G4double number) const
{
  BDSApertureRectCircle result = (*this) + number;
  return new BDSApertureRectCircle(result);
}

BDSAperture* BDSApertureRectCircle::Times(G4double number) const
{
  BDSApertureRectCircle result = (*this) * number;
  return new BDSApertureRectCircle(result);
}

BDSAperture* BDSApertureRectCircle::Clone() const
{
  return new BDSApertureRectCircle(*this);
}

std::array<G4double,7> BDSApertureRectCircle::ApertureNumbers() const
{
  return {a,b,radius,0,tiltOffset.OffsetX(),tiltOffset.OffsetY(),tiltOffset.Tilt()};
}

BDSPolygon BDSApertureRectCircle::PolygonNPoints(unsigned int nPointsIn) const
{
  BDSApertureCircle    ac = BDSApertureCircle(radius, nPointsIn);
  BDSApertureRectangle ar = BDSApertureRectangle(a, b);
  BDSPolygon pCircle    = ac.PolygonNPoints(nPointsIn);
  BDSPolygon pRectangle = ar.PolygonNPoints(nPointsIn);
  BDSPolygon result = pCircle.Intersection(pRectangle);
  return result;
}
