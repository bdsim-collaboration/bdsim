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
#include "BDSApertureOctagon.hh"
#include "BDSApertureType.hh"
#include "BDSExtent.hh"
#include "BDSPolygon.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "G4Types.hh"

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

BDSApertureOctagon::BDSApertureOctagon(G4double xIn,
                                       G4double yIn,
                                       G4double xEdgeIn,
                                       G4double yEdgeIn) :
  BDSAperture(BDSApertureType::octagon),
  x(xIn),
  y(yIn),
  xEdge(xEdgeIn),
  yEdge(yEdgeIn)
{;}

G4bool BDSApertureOctagon::Equals(const BDSAperture* other) const
{
  if (!other)
    {return false;}
  else if (other->apertureType != apertureType)
    {return false;}
  else if (other->tiltOffset != tiltOffset)
    {return false;}
  else
    {
      const BDSApertureOctagon* oc = dynamic_cast<const BDSApertureOctagon*>(other);
      return BDS::DoublesAreEqual(oc->x, x) &&
             BDS::DoublesAreEqual(oc->y, y) &&
             BDS::DoublesAreEqual(oc->xEdge, xEdge) &&
             BDS::DoublesAreEqual(oc->yEdge, yEdge);
    }
}

void BDSApertureOctagon::CheckInfoOK() const
{
  CheckRequiredParametersSet(x, true, y, true, xEdge, true, yEdge, true);
  CheckParameterIsPositive(x, "x");
  CheckParameterIsPositive(y, "y");
  CheckParameterIsPositive(xEdge, "xEdge");
  CheckParameterIsPositive(yEdge, "yEdge");
}

G4double BDSApertureOctagon::RadiusToEncompass() const
{
  return std::max(std::hypot(x,yEdge), std::hypot(xEdge,y)) + tiltOffset.Radius();
}

BDSExtent BDSApertureOctagon::Extent() const
{
  BDSExtent simpleExtent(x, y, 0);
  return ExtentOffsetTilt(simpleExtent);
}

G4double BDSApertureOctagon::MaximumInscribedCircleRadius() const
{
  auto p = Polygon();
  return p.MaximumInscribedCricleRadius();
}

BDSApertureOctagon BDSApertureOctagon::operator+ (G4double number) const
{
  BDSApertureOctagon result = BDSApertureOctagon(*this);
  result += number;
  return result;
}

const BDSApertureOctagon& BDSApertureOctagon::operator+=(G4double number)
{
  x += number;
  y += number;
  xEdge += number;
  yEdge += number;
  return *this;
}

BDSApertureOctagon BDSApertureOctagon::operator* (G4double number) const
{
  BDSApertureOctagon result = BDSApertureOctagon(*this);
  result *= number;
  return result;
}

const BDSApertureOctagon& BDSApertureOctagon::operator*=(G4double number)
{
  x *= number;
  y *= number;
  xEdge *= number;
  yEdge *= number;
  return *this;
}

BDSAperture* BDSApertureOctagon::Plus(G4double number) const
{
  BDSApertureOctagon result = (*this) + number;
  return new BDSApertureOctagon(result);
}

BDSAperture* BDSApertureOctagon::Times(G4double number) const
{
  BDSApertureOctagon result = (*this) * number;
  return new BDSApertureOctagon(result);
}

BDSAperture* BDSApertureOctagon::Clone() const
{
  return new BDSApertureOctagon(*this);
}

std::array<G4double,7> BDSApertureOctagon::ApertureNumbers() const
{
  return {x,y,xEdge,yEdge,tiltOffset.OffsetX(),tiltOffset.OffsetY(),tiltOffset.Tilt()};
}

BDSPolygon BDSApertureOctagon::PolygonNPoints(unsigned int nPointsIn) const
{
  std::vector<G4TwoVector> r;
  r.emplace_back( x,      yEdge);
  r.emplace_back( x,     -yEdge);
  r.emplace_back( xEdge, -y);
  r.emplace_back(-xEdge, -y);
  r.emplace_back(-x,     -yEdge);
  r.emplace_back(-x,      yEdge);
  r.emplace_back(-xEdge,  y);
  r.emplace_back( xEdge,  y);
  if (nPointsIn == 8)
    {return BDSPolygon(r);}
  else
    {
      nPointsIn = BDS::NextMultiple(nPointsIn, 8); // ensure multiple of 8
      unsigned int nPointsPerEdge = nPointsIn / 8;
      std::vector<G4TwoVector> result;
      for (G4int i=0; i < 8; i++)
        {AddPointsOnLineUpTo(result, r[i], r[(i+1)%8], nPointsPerEdge);}
      return BDSPolygon(result);
    }
}
