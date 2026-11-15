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
#include "BDSApertureEllipse.hh"
#include "BDSApertureRectangle.hh"
#include "BDSApertureRectEllipse.hh"
#include "BDSApertureType.hh"
#include "BDSExtent.hh"
#include "BDSPolygon.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "G4Types.hh"

#include <algorithm>
#include <array>

BDSApertureRectEllipse::BDSApertureRectEllipse(G4double rectangleAIn,
                                               G4double rectangleBIn,
                                               G4double ellipseAIn,
                                               G4double ellipseBIn,
                                               unsigned int nPointsIn):
  BDSApertureCurved(BDSApertureType::rectellipse, nPointsIn),
  rectangleA(rectangleAIn),
  rectangleB(rectangleBIn),
  ellipseA(ellipseAIn),
  ellipseB(ellipseBIn)
{;}

G4bool BDSApertureRectEllipse::Equals(const BDSAperture* other) const
{
  if (!other)
    {return false;}
  else if (other->apertureType != apertureType)
    {return false;}
  else if (other->tiltOffset != tiltOffset)
    {return false;}
  else
    {
      const BDSApertureRectEllipse* oc = dynamic_cast<const BDSApertureRectEllipse*>(other);
      return BDS::DoublesAreEqual(oc->rectangleA, rectangleA) &&
	         BDS::DoublesAreEqual(oc->rectangleB, rectangleB) &&
	         BDS::DoublesAreEqual(oc->ellipseA, ellipseA) &&
      	     BDS::DoublesAreEqual(oc->ellipseB, ellipseB);
    }
}

void BDSApertureRectEllipse::CheckInfoOK() const
{
  CheckRequiredParametersSet(rectangleA, true, rectangleB, true, ellipseA, true, ellipseB, true);
  CheckParameterIsPositive(rectangleA, "rectangleA");
  CheckParameterIsPositive(rectangleB, "rectangleB");
  CheckParameterIsPositive(ellipseA,   "ellipseA");
  CheckParameterIsPositive(ellipseB,   "ellipseB");
}

BDSExtent BDSApertureRectEllipse::Extent() const
{
  BDSExtent simpleExtent(std::min(rectangleA, ellipseA), std::min(rectangleB, ellipseB), 0);
  return ExtentOffsetTilt(simpleExtent);
}

G4double BDSApertureRectEllipse::MaximumInscribedCircleRadius() const
{
  return std::min({rectangleA, rectangleB, ellipseA, ellipseB});
}

BDSApertureRectEllipse BDSApertureRectEllipse::operator+ (G4double number) const
{
  BDSApertureRectEllipse result = BDSApertureRectEllipse(*this);
  result += number;
  return result;
}

const BDSApertureRectEllipse& BDSApertureRectEllipse::operator+=(G4double number)
{
  rectangleA += number;
  rectangleB += number;
  ellipseA   += number;
  ellipseB   += number;
  return *this;
}

BDSApertureRectEllipse BDSApertureRectEllipse::operator* (G4double number) const
{
  BDSApertureRectEllipse result = BDSApertureRectEllipse(*this);
  result *= number;
  return result;
}

const BDSApertureRectEllipse& BDSApertureRectEllipse::operator*=(G4double number)
{
  rectangleA *= number;
  rectangleB *= number;
  ellipseA   *= number;
  ellipseB   *= number;
  return *this;
}

BDSAperture* BDSApertureRectEllipse::Plus(G4double number) const
{
  BDSApertureRectEllipse result = (*this) + number;
  return new BDSApertureRectEllipse(result);
}

BDSAperture* BDSApertureRectEllipse::Times(G4double number) const
{
  BDSApertureRectEllipse result = (*this) * number;
  return new BDSApertureRectEllipse(result);
}

BDSAperture* BDSApertureRectEllipse::Clone() const
{
  return new BDSApertureRectEllipse(*this);
}

std::array<G4double,7> BDSApertureRectEllipse::ApertureNumbers() const
{
  return {rectangleA,rectangleB,ellipseA,ellipseB,tiltOffset.OffsetX(),tiltOffset.OffsetY(),tiltOffset.Tilt()};
}

BDSPolygon BDSApertureRectEllipse::PolygonNPoints(unsigned int nPointsIn) const
{
  BDSApertureEllipse   ae = BDSApertureEllipse(ellipseA, ellipseB, nPointsIn);
  BDSApertureRectangle ar = BDSApertureRectangle(rectangleA, rectangleB);
  BDSPolygon pEllipse   = ae.PolygonNPoints(nPointsIn);
  BDSPolygon pRectangle = ar.PolygonNPoints(nPointsIn);
  BDSPolygon result = pEllipse.Intersection(pRectangle);
  return result;
}
