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
#include "BDSAperturePoints.hh"
#include "BDSAperturePointsLoader.hh"
#include "BDSApertureType.hh"
#include "BDSExtent.hh"
#include "BDSPolygon.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "G4Types.hh"

#include <array>
#include <vector>

BDSAperturePoints::BDSAperturePoints(const G4String& pointsFileNameIn,
                                     const G4String& unitsStrIn):
  BDSAperture(BDSApertureType::points),
  pointsFileName(pointsFileNameIn),
  unitsStr(unitsStrIn),
  points(nullptr)
{;}

BDSAperturePoints::BDSAperturePoints(const BDSAperturePoints& other):
  BDSAperture(other.apertureType),
  pointsFileName(other.pointsFileName),
  unitsStr(other.unitsStr),
  points(nullptr)
{
  if (other.points)
    {points = new BDSPolygon(*other.points);}
}

BDSAperturePoints::~BDSAperturePoints() noexcept
{
  delete points;
}

void BDSAperturePoints::LoadData()
{
  std::vector<G4TwoVector>* rawPoints = BDS::LoadAperturePoints(pointsFileName, unitsStr);
  points = new BDSPolygon(*rawPoints);
}

G4bool BDSAperturePoints::Equals(const BDSAperture* other) const
{
  if (!other)
    {return false;}
  else if (other->apertureType != apertureType)
    {return false;}
  else if (other->tiltOffset != tiltOffset)
    {return false;}
  else
    {
      const BDSAperturePoints* oc = dynamic_cast<const BDSAperturePoints*>(other);
      return oc->pointsFileName == pointsFileName && oc->unitsStr == unitsStr;
    }
}

void BDSAperturePoints::CheckInfoOK() const
{
  if (pointsFileName.empty())
    {throw BDSException("","no points file specified");}
}

G4double  BDSAperturePoints::RadiusToEncompass() const
{
  return points->RadiusToEncompass();
}

BDSExtent BDSAperturePoints::Extent() const
{
  return points->Extent();
}

G4double  BDSAperturePoints::MaximumInscribedCircleRadius() const
{
  return points->MaximumInscribedCricleRadius();
}

unsigned int BDSAperturePoints::MinimumNumberOfPoints() const
{
  return points->size();
}

BDSAperturePoints BDSAperturePoints::operator+ (G4double number) const
{
  BDSAperturePoints result = BDSAperturePoints(*this);
  result += number;
  return result;
}

const BDSAperturePoints& BDSAperturePoints::operator+=(G4double number)
{
  points->ExpandByValueUsingVertexNormalsInPlace(number);
  return *this;
}

BDSAperturePoints BDSAperturePoints::operator* (G4double number) const
{
  BDSAperturePoints result = BDSAperturePoints(*this);
  result *= number;
  return result;
}

const BDSAperturePoints& BDSAperturePoints::operator*=(G4double number)
{
  points->ScaleByValueInPlace(number);
  return *this;
}

BDSAperture* BDSAperturePoints::Plus(G4double number) const
{
  BDSAperturePoints result = (*this) + number;
  return new BDSAperturePoints(result);
}

BDSAperture* BDSAperturePoints::Times(G4double number) const
{
  BDSAperturePoints result = (*this) * number;
  return new BDSAperturePoints(result);
}

BDSAperture* BDSAperturePoints::Clone() const
{
  return new BDSAperturePoints(*this);
}

std::array<G4double,7> BDSAperturePoints::ApertureNumbers() const
{
  return {0,0,0,0,tiltOffset.OffsetX(),tiltOffset.OffsetY(),tiltOffset.Tilt()};
}

BDSPolygon BDSAperturePoints::PolygonNPoints(unsigned int nPointsIn) const
{
  if (nPointsIn == points->size())
    {return BDSPolygon(*points);}
  else
  {
    nPointsIn = BDS::NextMultiple(nPointsIn, points->size());
    return points->InterpolateWithNPoints(nPointsIn);
  }
}
