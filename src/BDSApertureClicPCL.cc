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
#include "BDSApertureClicPCL.hh"
#include "BDSApertureCurved.hh"
#include "BDSApertureType.hh"
#include "BDSExtent.hh"
#include "BDSPolygon.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "G4TwoVector.hh"
#include "G4Types.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <array>
#include <vector>

BDSApertureClicPCL::BDSApertureClicPCL(G4double xHalfWidthIn,
                                       G4double yTopIn,
                                       G4double yBottomIn,
                                       G4double ySepIn,
                                       unsigned int nPointsIn):
  BDSApertureCurved(BDSApertureType::clicpcl, nPointsIn),
  x(xHalfWidthIn),
  yTop(yTopIn),
  yBottom(yBottomIn),
  ySep(ySepIn)
{;}

G4bool BDSApertureClicPCL::Equals(const BDSAperture* other) const
{
  if (!other)
    {return false;}
  else if (other->apertureType != apertureType)
    {return false;}
  else if (other->tiltOffset != tiltOffset)
    {return false;}
  else
    {
      const BDSApertureClicPCL* oc = dynamic_cast<const BDSApertureClicPCL*>(other);
      return BDS::DoublesAreEqual(oc->x, x) &&
             BDS::DoublesAreEqual(oc->yTop, yTop) &&
             BDS::DoublesAreEqual(oc->yBottom, yBottom) &&
	           BDS::DoublesAreEqual(oc->ySep, ySep);
    }
}

void BDSApertureClicPCL::CheckInfoOK() const
{
  CheckRequiredParametersSet(x, true, yTop, true, yBottom, true, ySep, true);
  CheckParameterIsPositive(x, "x");
  CheckParameterIsPositive(yTop, "yTop");
  CheckParameterIsPositive(yBottom, "yBottom");
  CheckParameterIsPositive(ySep, "ySep");
}

BDSExtent BDSApertureClicPCL::Extent() const
{
  BDSExtent simpleExtent(-x, x,
                         -yBottom, ySep + yTop,
                          0, 0);
  return ExtentOffsetTilt(simpleExtent);
}

G4double BDSApertureClicPCL::MinimumInscribedCircleRadius() const
{
  return std::min({x, ySep+yTop, yBottom});
}

BDSApertureClicPCL BDSApertureClicPCL::operator+ (G4double number) const
{
  BDSApertureClicPCL result = BDSApertureClicPCL(*this);
  result += number;
  return result;
}

const BDSApertureClicPCL& BDSApertureClicPCL::operator+=(G4double number)
{
  x += number;
  yTop += number;
  yBottom += number;
  return *this;
}

BDSApertureClicPCL BDSApertureClicPCL::operator* (G4double number) const
{
  BDSApertureClicPCL result = BDSApertureClicPCL(*this);
  result *= number;
  return result;
}

const BDSApertureClicPCL& BDSApertureClicPCL::operator*=(G4double number)
{
  x *= number;
  yTop *= number;
  yBottom *= number;
  ySep *= number;
  return *this;
}

BDSAperture* BDSApertureClicPCL::Plus(G4double number) const
{
  BDSApertureClicPCL result = (*this) + number;
  return new BDSApertureClicPCL(result);
}

BDSAperture* BDSApertureClicPCL::Times(G4double number) const
{
  BDSApertureClicPCL result = (*this) * number;
  return new BDSApertureClicPCL(result);
}

BDSAperture* BDSApertureClicPCL::Clone() const
{
  return new BDSApertureClicPCL(*this);
}

std::array<G4double,7> BDSApertureClicPCL::ApertureNumbers() const
{
  return {x,yTop,yBottom,ySep,tiltOffset.OffsetX(),tiltOffset.OffsetY(),tiltOffset.Tilt()};
}

BDSPolygon BDSApertureClicPCL::PolygonNPoints(unsigned int nPointsIn) const
{
  std::vector<G4TwoVector> r;
  nPointsIn = BDS::NextMultiple(nPointsIn, 2); // ensure multiple of 2
  G4int nPoints = nPointsIn / 2;
  AppendAngleEllipse(r, -CLHEP::halfpi, CLHEP::halfpi, x, yTop, nPoints, 0, ySep);
  AppendAngleEllipse(r, CLHEP::halfpi, CLHEP::halfpi + CLHEP::pi, x, yBottom, nPoints, 0, 0);
  return BDSPolygon(r);
}
