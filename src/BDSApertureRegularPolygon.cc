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
#include "BDSAperture.hh"
#include "BDSApertureRegularPolygon.hh"
#include "BDSApertureType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
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

BDSApertureRegularPolygon::BDSApertureRegularPolygon(G4int nSidesIn,
                                                     G4double radiusIn,
                                                     G4double polygonTiltIn):
  BDSAperture(BDSApertureType::regularpolygon),
  nSides(nSidesIn),
  radius(radiusIn),
  polygonTilt(polygonTiltIn)
{;}

BDSApertureRegularPolygon::BDSApertureRegularPolygon(const BDSApertureRegularPolygon& other):
  BDSAperture(other),
  nSides(other.nSides),
  radius(other.radius),
  polygonTilt(other.polygonTilt)
{;}

BDSApertureRegularPolygon& BDSApertureRegularPolygon::operator=(const BDSApertureRegularPolygon& other) noexcept
{
  if (this != &other)
    {
      BDSAperture::operator=(other);
      nSides = other.nSides;
      radius = other.radius;
      polygonTilt = other.polygonTilt;
    }
  return *this;
}

G4bool BDSApertureRegularPolygon::Equals(const BDSAperture* other) const
{
  if (!other)
    {return false;}
  else if (other->apertureType != apertureType)
    {return false;}
  else if (other->tiltOffset != tiltOffset)
    {return false;}
  else
    {
      const BDSApertureRegularPolygon* oc = dynamic_cast<const BDSApertureRegularPolygon*>(other);
      G4bool r1 = oc->nSides == nSides;
      G4bool r2 = BDS::DoublesAreEqual(oc->radius, radius);
      G4bool r3 = BDS::DoublesAreEqual(oc->polygonTilt, polygonTilt);
      return r1 && r2 && r3;
    }
}

void BDSApertureRegularPolygon::CheckInfoOK() const
{
  CheckRequiredParametersSet(nSides, true);
  if (nSides <= 2)
    {throw BDSException(__METHOD_NAME__, "parameter nSides (aper1) is not >= 2");}
  CheckRequiredParametersSet(radius, true);
  CheckParameterIsPositive(radius, "radius");
  if (polygonTilt > CLHEP::twopi/(G4double)nSides)
    {throw BDSException(__METHOD_NAME__, "excessive tilt (aper3) greater than 2pi / "+std::to_string(nSides));}
}

BDSExtent BDSApertureRegularPolygon::Extent() const
{
  BDSPolygon p = PolygonNPoints(nSides);
  BDSExtent ext = p.Extent();
  return ext;
}

BDSApertureRegularPolygon BDSApertureRegularPolygon::operator+ (G4double number) const
{
  BDSApertureRegularPolygon result = BDSApertureRegularPolygon(*this);
  result += number;
  return result;
}

const BDSApertureRegularPolygon& BDSApertureRegularPolygon::operator+=(G4double number)
{
  radius += number;
  return *this;
}

BDSApertureRegularPolygon BDSApertureRegularPolygon::operator* (G4double number) const
{
  BDSApertureRegularPolygon result = BDSApertureRegularPolygon(*this);
  result *= number;
  return result;
}

const BDSApertureRegularPolygon& BDSApertureRegularPolygon::operator*=(G4double number)
{
  radius *= number;
  return *this;
}

BDSAperture* BDSApertureRegularPolygon::Plus(G4double number) const
{
  BDSApertureRegularPolygon result = (*this) + number;
  return new BDSApertureRegularPolygon(result);
}

BDSAperture* BDSApertureRegularPolygon::Times(G4double number) const
{
  BDSApertureRegularPolygon result = (*this) * number;
  return new BDSApertureRegularPolygon(result);
}

BDSAperture* BDSApertureRegularPolygon::Clone() const
{
  return new BDSApertureRegularPolygon(*this);
}

std::array<G4double,7> BDSApertureRegularPolygon::ApertureNumbers() const
{
  return {radius,(G4double)nSides,polygonTilt,0,tiltOffset.OffsetX(),tiltOffset.OffsetY(),tiltOffset.Tilt()};
}

BDSPolygon BDSApertureRegularPolygon::PolygonNPoints(unsigned int nPointsIn) const
{
  std::vector<G4TwoVector> r;
  G4double dTheta = CLHEP::twopi / (G4double)nSides;
  for (G4int i = 0; i < nSides; i++)
    {r.emplace_back(radius * std::cos(i*dTheta + polygonTilt), radius * std::sin(i*dTheta + polygonTilt));}

  if (nPointsIn == (unsigned int)nSides)
    {return BDSPolygon(r);}
  else
    {
      nPointsIn = BDS::NextMultiple(nPointsIn, nSides);
      unsigned int nPointsPerEdge = nPointsIn / nSides;
      std::vector<G4TwoVector> result;
      for (G4int i=0; i < 8; i++)
        {AddPointsOnLineUpTo(result, r[i], r[(i+1)%nSides], nPointsPerEdge);}
      return BDSPolygon(result);
    }
}
