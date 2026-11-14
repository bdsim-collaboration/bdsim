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
#include "BDSArray1DCoords.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSFieldValue.hh"
#include "BDSGlobalConstants.hh"
#include "BDSInterpolatorRoutines.hh"
#include "BDSInterpolatorType.hh"
#include "BDSPolygon.hh"
#include "BDSTiltOffset.hh"

#include "G4TwoVector.hh"
#include "G4Types.hh"

#include <algorithm>
#include <limits>
#include <list>
#include <vector>

BDSPolygon::BDSPolygon(const std::vector<G4TwoVector>& pointsIn):
  points(pointsIn),
  vertexNormals(nullptr),
  extent(nullptr),
  radiusToEncompass(0),
  recalculateRadiusToEncompass(true),
  radiusInterior(0),
  recalculateRadiusInterior(true),
  interpolation(BDSInterpolatorType::linear1d)
{
  if (points.size() < 3)
    {throw BDSException(__METHOD_NAME__, "polygon must have at least 3 x,y points to be valid.");}
  
  // determine the winding order and if wrong, flip it
  // we rely on this order, so we calculate the outward facing normals correctly
  // further down - the sign of the pi/2
  auto detV1 = points[1] - points[0];
  auto detV2 = points[2] - points[1];
  G4double det = Determinant(detV1, detV2);
  if (det < 0)
    {std::reverse(points.begin(), points.end());}
}

BDSPolygon::~BDSPolygon() noexcept
{
  delete vertexNormals;
  delete extent;
}

BDSPolygon::BDSPolygon(const BDSPolygon& other):
  points(other.points),
  vertexNormals(nullptr),
  extent(nullptr),
  radiusToEncompass(other.radiusToEncompass),
  recalculateRadiusToEncompass(other.recalculateRadiusToEncompass),
  radiusInterior(other.radiusInterior),
  recalculateRadiusInterior(other.recalculateRadiusInterior),
  interpolation(other.interpolation)
{
  if (other.vertexNormals)
    {vertexNormals = new std::vector<G4TwoVector>(*(other.vertexNormals));}
  if (other.extent)
    {extent = new BDSExtent(*other.extent);}
}

G4bool BDSPolygon::Inside(const G4TwoVector& point) const
{
  G4bool result = true;
  G4int n = (G4int)size();
  for (G4int i = 0; i < n; i++)
    {
      G4TwoVector dir  = points[(i+1) % n] - points[i];
      G4TwoVector norm(-dir.y(), dir.x()); // 90° CCW rotation: inward normal for CCW polygon
      result &= (point - points[i]).dot(norm) > 0;
    }
  return result;
}

BDSPolygon::PointIs BDSPolygon::InsideLabelled(const G4TwoVector& point) const
{
  return Inside(point) ? BDSPolygon::PointIs::in : BDSPolygon::PointIs::out;
}

G4bool BDSPolygon::Inside(const BDSPolygon& other) const
{
  // test all segments don't intersect -> doesn't overlap
  // test one point is inside
  G4bool result = true;
  for (G4int i = 0; i < (G4int)size() - 1; i++)
    {
      for (G4int j = 0; j < (G4int)other.size() - 1; j++)
        {result &= !SegmentsIntersect(points[i], points[i+1], other.points[j], other.points[j+1]);}
    }
  if (!result) // don't bother with inside point check
    {return result;}

  // test one point is inside.
  result &= other.Inside(points[0]);
  return result;
}

G4bool BDSPolygon::SelfIntersecting(G4int* const indexOfIntersectionA,
                                    G4int* const indexOfIntersectionB) const
{
  G4bool result = false;
  for (G4int i = 0; i < (G4int)size() - 1; i++)
    {
      for (G4int j = i + 2; j < (G4int)size() - 1; j++)
        {
          result |= SegmentsIntersect(points[i], points[i+1], points[j], points[j+1]);
          if (result)
            {// for optional feedback
              if (indexOfIntersectionA)
                {*indexOfIntersectionA = i;}
              if (indexOfIntersectionB)
                {*indexOfIntersectionB = j;}
              return result;
            }
        }
    }
  return result;
}

BDSPolygon BDSPolygon::ApplyTiltOffset(const BDSTiltOffset& to) const
{
  BDSPolygon r(*this);
  if (!to.HasTiltOrOffset())
    {return r;}
  G4double t = to.Tilt();
  G4TwoVector offset = to.GetOffset2D();
  for (auto& p : r)
    {
      p.rotate(t);
      p += offset;
    }
  return r;
}

BDSPolygon BDSPolygon::ExpandByValueUsingVertexNormals(G4double value) const
{
  if (!vertexNormals)
    {CalculateVertexNormals();}
  
  std::vector<G4TwoVector> newPoints;
  newPoints.reserve(points.size());
  for (G4int i = 0; i < (G4int)points.size(); i++)
    {newPoints.emplace_back(points[i] + (*vertexNormals)[i] * value);}
  return BDSPolygon(newPoints);
}

void BDSPolygon::ExpandByValueUsingVertexNormalsInPlace(G4double value)
{
  if (!vertexNormals)
    {CalculateVertexNormals();}

  for (G4int i = 0; i < (G4int)points.size(); i++)
    {
      G4TwoVector newValue = points[i] + (*vertexNormals)[i] * value;
      points[i] = newValue;
    }
  recalculateRadiusToEncompass = true;
  recalculateRadiusInterior = true;
}

BDSPolygon BDSPolygon::ScaleByValue(G4double scale) const
{
  std::vector<G4TwoVector> newPoints;
  newPoints.reserve(points.size());
  for (G4int i = 0; i < (G4int)points.size(); i++)
    {newPoints.emplace_back(points[i] * scale);}
  return BDSPolygon(newPoints);
}

void BDSPolygon::ScaleByValueInPlace(G4double scale)
{
  for (G4int i = 0; i < (G4int)points.size(); i++)
    {
      G4TwoVector newValue = points[i] * scale;
      points[i] = newValue;
    }
  recalculateRadiusToEncompass = true;
  recalculateRadiusInterior = true;
}

BDSExtent BDSPolygon::Extent() const
{
  if (extent)
    {return BDSExtent(*extent);}

  G4double extXNeg = 0;
  G4double extXPos = 0;
  G4double extYNeg = 0;
  G4double extYPos = 0;
  for (const auto& p : points)
    {
      extXNeg = std::min(extXNeg, p.x());
      extXPos = std::max(extXPos, p.x());
      extYNeg = std::min(extYNeg, p.y());
      extYPos = std::max(extYPos, p.y());
    }

  G4double lsl = BDSGlobalConstants::Instance()->LengthSafetyLarge();
  extent = new BDSExtent(extXNeg, extXPos,
                         extYNeg, extYPos,
                         -lsl,    lsl);
  return BDSExtent(*extent);
}

G4double BDSPolygon::RadiusToEncompass() const
{
  if (recalculateRadiusToEncompass)
    {
      auto max = std::max_element(points.begin(), points.end(), [](const G4TwoVector& a, const G4TwoVector& b) {return a.mag() < b.mag();});
      radiusToEncompass = max->mag();
    }
  recalculateRadiusToEncompass = false;
  return radiusToEncompass;
}

G4double BDSPolygon::MinimumInscribedCricleRadius() const
{

  if (recalculateRadiusInterior)
    {
      // first find the minimum radius of any point on the polygon
      auto min = std::min_element(points.begin(), points.end(), [](const G4TwoVector& a, const G4TwoVector& b) {return a.mag() < b.mag();});
      radiusInterior = min->mag();

      // then also take the minimum from the minimum approach to the 0,0 point
      // of any segment - see https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
      auto nPoints = (G4int)points.size();
      for (G4int i = 0; i < nPoints; i++)
        {
          G4TwoVector p0 = points[i];
          G4TwoVector p1 = points[(i+1)%nPoints];
          G4double nom = std::abs(p1.x()*p0.y() - p1.y()*p0.x());
          G4double dist = (p1-p0).mag();
          G4double minApproach = nom/dist;
          radiusInterior = std::min(radiusInterior, minApproach);
        }
      }

  recalculateRadiusInterior = false;
  return radiusInterior;
}

G4bool BDSPolygon::EncompassesExtentXY(const BDSExtent& ext) const
{
  G4bool result = true;
  auto pts = ext.AllBoundaryPointsXY();
  for (const auto& p : pts)
    {result |= Inside(p);}
  return result;
}

G4bool BDSPolygon::FitsInside(const BDSExtent& ext) const
{
  auto polyExtent = Extent();
  return polyExtent.TransverselyLessThan(ext);
}

G4int BDSPolygon::SegmentsIntersect(const G4TwoVector& p1,
                                    const G4TwoVector& p2,
                                    const G4TwoVector& q1,
                                    const G4TwoVector& q2,
                                    G4TwoVector* intersectionPoint)
{
  // Algorithm parametrises segments as P(s) = p1 + s*D0 and Q(t) = q1 + t*D1.
  // Use a relative error test to test for parallelism.  This effectively
  // is a threshold on the angle between D0 and D1.
  G4TwoVector D0       = p2 - p1;
  G4TwoVector D1       = q2 - q1;
  const G4double eps = std::numeric_limits<double>::epsilon() * std::abs(std::max(p1.x(), p1.y()));
  const G4double sqrEpsilon = eps*eps;
  G4TwoVector E        = q1 - p1;
  G4double    kross    = D0.x() * D1.y() - D0.y() * D1.x();
  G4double    sqrKross = kross * kross;
  G4double    sqrLen0  = D0.x() * D0.x() + D0.y() * D0.y();
  G4double    sqrLen1  = D1.x() * D1.x() + D1.y() * D1.y();
  if (sqrKross > sqrEpsilon * sqrLen0 * sqrLen1)
    {
      // lines are not parallel
      G4double s = (E.x() * D1.y() - E.y() * D1.x()) / kross;
      G4double t = (E.x() * D0.y() - E.y() * D0.x()) / kross;
      if (s < 0 || s > 1 || t < 0 || t > 1)
        {return 0;}
      if (intersectionPoint)
        {*intersectionPoint = p1 + s * D0;}
      return 1;
    }

  // lines are parallel
  G4double sqrLenE = E.x() * E.x() + E.y() * E.y();
  kross = E.x() * D0.y() - E.y() * D0.x();
  sqrKross = kross * kross;
  if (sqrKross > sqrEpsilon * sqrLen0 * sqrLenE)
    {return 0;}// lines are different
  
  // lines are the same
  return 2;
}

std::list<BDSPolygon::LabelledPoint> BDSPolygon::GenerateLabelled(const BDSPolygon& reference,
                                           const BDSPolygon& test,
                                           G4int* nInside)
{
  std::list<LabelledPoint> testLabelled;
  G4int nIn = 0;
  for (const auto& p : test)
    {
      auto inOut = reference.InsideLabelled(p);
      nIn += inOut == PointIs::in ? 1 : 0;
      testLabelled.emplace_back(LabelledPoint{&p, inOut});
    }
  if (nInside)
    {*nInside = nIn;}
  return testLabelled;
}

BDSPolygon BDSPolygon::Union(const BDSPolygon& other) const
{
  G4int nOtherInThis = 0;
  GenerateLabelled(*this, other, &nOtherInThis);
  G4int nThisInOther = 0;
  GenerateLabelled(other, *this, &nThisInOther);
  if (nOtherInThis == (G4int)other.size())
    {return BDSPolygon(*this);}
  if (nThisInOther == (G4int)size())
    {return BDSPolygon(other);}
  return BooleanTraversal(other, false);
}

BDSPolygon BDSPolygon::BooleanTraversal(const BDSPolygon& other, bool startInsideOther, bool reverseOther) const
{
  G4int nA = (G4int)size();
  G4int nB = (G4int)other.size();

  // Find all pairwise edge-edge intersections and their parameters along each edge.
  struct ISect
  {
    G4TwoVector pt;
    G4int iA, iB;
    G4double sA, sB;
  };
  std::vector<ISect> allISects;

  for (G4int i = 0; i < nA; i++)
    {
      G4TwoVector D0 = points[(i+1)%nA] - points[i];
      for (G4int j = 0; j < nB; j++)
        {
          G4TwoVector D1 = other.points[(j+1)%nB] - other.points[j];
          G4TwoVector E  = other.points[j] - points[i];
          G4double kross = D0.x()*D1.y() - D0.y()*D1.x();
          if (std::abs(kross) < std::numeric_limits<G4double>::epsilon() * D0.mag() * D1.mag())
            {continue;}
          G4double s = (E.x()*D1.y() - E.y()*D1.x()) / kross;
          G4double t = (E.x()*D0.y() - E.y()*D0.x()) / kross;
          if (s <= 0 || s >= 1 || t <= 0 || t >= 1)
            {continue;}
          allISects.push_back({points[i] + s*D0, i, j, s, t});
        }
    }

  if (allISects.empty())
    {throw BDSException(__METHOD_NAME__, "polygons do not overlap");}

  // Bucket intersections by edge, sorted by parameter along that edge.
  std::vector<std::vector<G4int>> byEdgeA(nA), byEdgeB(nB);
  for (G4int k = 0; k < (G4int)allISects.size(); k++)
    {
      byEdgeA[allISects[k].iA].push_back(k);
      byEdgeB[allISects[k].iB].push_back(k);
    }
  for (G4int i = 0; i < nA; i++)
    {std::sort(byEdgeA[i].begin(), byEdgeA[i].end(), [&](G4int a, G4int b){return allISects[a].sA < allISects[b].sA;});}
  for (G4int j = 0; j < nB; j++)
    {std::sort(byEdgeB[j].begin(), byEdgeB[j].end(), [&](G4int a, G4int b){return allISects[a].sB < allISects[b].sB;});}

  // Build augmented point lists: original vertices with intersection points inserted
  // in edge order. Intersection entries carry a cross-link to their counterpart in
  // the other polygon's list.
  enum class PtType {vertex, intersection};
  struct AugPt
  {
    G4TwoVector pt;
    PtType      type;
    G4int       crossIdx;
    bool        visited;
  };

  std::vector<AugPt> augA, augB;
  std::vector<G4int> idxInAugA(allISects.size()), idxInAugB(allISects.size());

  for (G4int i = 0; i < nA; i++)
    {
      augA.push_back({points[i], PtType::vertex, -1, false});
      for (G4int k : byEdgeA[i])
        {
          idxInAugA[k] = (G4int)augA.size();
          augA.push_back({allISects[k].pt, PtType::intersection, -1, false});
        }
    }
  for (G4int j = 0; j < nB; j++)
    {
      augB.push_back({other.points[j], PtType::vertex, -1, false});
      for (G4int k : byEdgeB[j])
        {
          idxInAugB[k] = (G4int)augB.size();
          augB.push_back({allISects[k].pt, PtType::intersection, -1, false});
        }
    }
  for (G4int k = 0; k < (G4int)allISects.size(); k++)
    {
      augA[idxInAugA[k]].crossIdx = idxInAugB[k];
      augB[idxInAugB[k]].crossIdx = idxInAugA[k];
    }

  // Find a starting vertex on A that is inside or outside other, depending on the
  // operation: outside for union (walks outer boundary), inside for intersection
  // (walks inner boundary).
  G4int startA = -1;
  for (G4int i = 0; i < (G4int)augA.size(); i++)
    {
      if (augA[i].type != PtType::vertex) {continue;}
      G4bool inside = other.Inside(augA[i].pt);
      if (inside == startInsideOther) {startA = i; break;}
    }
  if (startA < 0)
    {throw BDSException(__METHOD_NAME__, "no suitable starting vertex found for boolean traversal");}

  // Greiner-Hormann traversal: walk A forward; switch to B at every intersection;
  // switch back to A at the next intersection on B.  The visited flag on the
  // start point terminates the loop.
  std::vector<G4TwoVector> result;
  G4int curA = startA, curB = 0;
  bool onA = true;
  G4int maxSteps = 2 * ((G4int)augA.size() + (G4int)augB.size());

  for (G4int step = 0; step < maxSteps; step++)
    {
      if (onA)
        {
          AugPt& ap = augA[curA];
          if (ap.visited) {break;}
          result.push_back(ap.pt);
          ap.visited = true;
          if (ap.type == PtType::intersection)
            {
              G4int nBaug = (G4int)augB.size();
              curB = reverseOther ? (ap.crossIdx - 1 + nBaug) % nBaug
                                  : (ap.crossIdx + 1) % nBaug;
              onA  = false;
            }
          else
            {curA = (curA + 1) % (G4int)augA.size();}
        }
      else
        {
          AugPt& bp = augB[curB];
          if (bp.visited) {break;}
          result.push_back(bp.pt);
          bp.visited = true;
          if (bp.type == PtType::intersection)
            {
              curA = (bp.crossIdx + 1) % (G4int)augA.size();
              onA  = true;
            }
          else
            {
              G4int nBaug = (G4int)augB.size();
              curB = reverseOther ? (curB - 1 + nBaug) % nBaug
                                  : (curB + 1) % nBaug;
            }
        }
    }

  if ((G4int)result.size() < 3)
    {throw BDSException(__METHOD_NAME__, "boolean traversal produced a degenerate polygon");}

  return BDSPolygon(result);
}

std::vector<BDSPolygon*> BDSPolygon::Subtraction(const BDSPolygon& other) const
{
  G4int nOtherInThis = 0;
  GenerateLabelled(*this, other, &nOtherInThis);
  G4int nThisInOther = 0;
  GenerateLabelled(other, *this, &nThisInOther);
  if (nThisInOther == (G4int)size())
    {return {};} // A entirely inside B → empty result
  if (nOtherInThis == (G4int)other.size())
    {throw BDSException(__METHOD_NAME__, "subtraction of an enclosed polygon produces a hole, which is not supported");}
  if (nOtherInThis == 0 && nThisInOther == 0)
    {return {new BDSPolygon(*this)};} // disjoint → A unchanged
  return {new BDSPolygon(BooleanTraversal(other, false, true))};
}

BDSPolygon BDSPolygon::Intersection(const BDSPolygon& other) const
{
  G4int nOtherInThis = 0;
  GenerateLabelled(*this, other, &nOtherInThis);
  G4int nThisInOther = 0;
  GenerateLabelled(other, *this, &nThisInOther);
  if (nOtherInThis == (G4int)other.size())
    {return BDSPolygon(other);}
  if (nThisInOther == (G4int)size())
    {return BDSPolygon(*this);}
  return BooleanTraversal(other, true);
}

BDSPolygon BDSPolygon::InterpolateWithNPoints(unsigned int nPointsNew) const
{
  BDSArray1DCoords array = BDSArray1DCoords((G4int)points.size() + 2,
                                            -1.0,
                                            (G4double)points.size() + 1.0);
  auto Convert = [](const G4TwoVector& a) { return BDSFieldValue((FIELDTYPET)a.x(), (FIELDTYPET)a.y(), 0); };
  auto ConvertBack = [](const BDSFieldValue& a) { return G4TwoVector((FIELDTYPET)a.x(), (FIELDTYPET)a.y()); };
  array(0) = Convert(points.back());
  for (G4int i = 0; i < (G4int)points.size(); i++)
    {array(i+1) = Convert(points[i]);}
  array((G4int)points.size()+1) = Convert(points[0]);

  std::vector<G4TwoVector> result;
  result.reserve(nPointsNew);
  
  G4double ratio = (G4double)points.size() / (G4double)nPointsNew;
  switch (interpolation.underlying())
    {
    case BDSInterpolatorType::linear1d:
      {
        for (unsigned int i = 0; i < nPointsNew; i++)
          {
            G4double newIndex = (G4double)i * ratio;
            BDSFieldValue localData[2];
            G4double xFrac;
            array.ExtractSection2(newIndex, localData, xFrac);
            result.emplace_back(ConvertBack(BDS::Linear1D(localData, xFrac)));
          }
        break;
      }
    case BDSInterpolatorType::cubic1d:
      {
        for (unsigned int i = 0; i < nPointsNew; i++)
          {
            G4double newIndex = (G4double)i * ratio;
            BDSFieldValue localData[4];
            G4double xFrac;
            array.ExtractSection4(newIndex, localData, xFrac);
            result.emplace_back(ConvertBack(BDS::Cubic1D(localData, xFrac)));
          }
        break;
      }
    default:
      {throw BDSException(__METHOD_NAME__, "invalid interpolation type \"" + interpolation.ToString() + "\"");}
    }
  
  auto resultPolygon = BDSPolygon(result);
  return resultPolygon;
}

void BDSPolygon::CalculateVertexNormals() const
{
  delete vertexNormals;
  vertexNormals = new std::vector<G4TwoVector>();
  vertexNormals->reserve(points.size());

  // This relies on a winding order established in the constructor.
  
  // Loop round the polygon and calculate the normal of each segment
  // sum the normals of two segments and normalise to give an outwards
  // pointing unit vector at each vertex. We have a bit of if else for
  // the boundaries of the loop for back / front.
  G4TwoVector v1;
  G4TwoVector v2;
  G4TwoVector v3;
  for (G4int i = 0; i < (G4int)points.size(); i++)
    {
      if (i == 0)
        {
          v1 = points.back();
          v3 = points[i+1];
        }
      else if (i == (G4int)points.size() - 1)
        {
          v1 = points[i-1];
          v3 = points[0];
        }
      else
        {
          v1 = points[i-1];
          v3 = points[i+1];
        }
      v2 = points[i];
      
      auto normal1 = v2 - v1;
      auto normal2 = v3 - v2;
      normal1.rotate(-CLHEP::halfpi); // now it's a normal
      normal2.rotate(-CLHEP::halfpi);
      
      auto vertexNormal = (normal1 + normal2).unit();
      
      vertexNormals->emplace_back(vertexNormal); // outward pointing direction for expansion
    }
}

G4double BDSPolygon::Determinant(const G4TwoVector& v1,
                                 const G4TwoVector& v2) const
{
  G4double det = v1.x()*v2.y() - v2.x()*v1.y();
  return det;
}
