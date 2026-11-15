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
#include "BDSException.hh"
#include "BDSPolygon.hh"
#include "BDSTiltOffset.hh"

#include "G4TwoVector.hh"
#include "G4Types.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

static int nPassed = 0;
static int nFailed = 0;

void Check(bool condition, const std::string& name)
{
  if (condition)
    {std::cout << "PASS: " << name << "\n"; nPassed++;}
  else
    {std::cout << "FAIL: " << name << "\n"; nFailed++;}
}

bool Near(G4double a, G4double b, G4double tol = 1e-9)
{return std::abs(a - b) < tol;}

bool VecNear(const G4TwoVector& a, const G4TwoVector& b, G4double tol = 1e-9)
{return (a - b).mag() < tol;}

// Check that every point in 'expected' appears somewhere in 'poly' (order-insensitive).
bool PolyContainsAll(const BDSPolygon& poly,
                     const std::vector<G4TwoVector>& expected,
                     G4double tol = 1e-9)
{
  for (const auto& ep : expected)
    {
      bool found = false;
      for (const auto& pp : poly)
        {if ((pp - ep).mag() < tol) {found = true; break;}}
      if (!found) {return false;}
    }
  return true;
}

int main()
{
  // Canonical test polygons (unitless coordinates, CCW winding).
  //
  // A: square (0,0)-(4,4)
  // B: square (2,2)-(6,6) — partial overlap with A in (2,2)-(4,4)
  // small: square (1,1)-(3,3) — entirely inside A
  // big:   square (-5,-5)-(10,10) — entirely contains A
  // disjoint: square (10,10)-(14,14)

  const std::vector<G4TwoVector> ptsA      = {{0,0},{4,0},{4,4},{0,4}};
  const std::vector<G4TwoVector> ptsB      = {{2,2},{6,2},{6,6},{2,6}};
  const std::vector<G4TwoVector> ptsSmall  = {{1,1},{3,1},{3,3},{1,3}};
  const std::vector<G4TwoVector> ptsBig    = {{-5,-5},{10,-5},{10,10},{-5,10}};
  const std::vector<G4TwoVector> ptsDisjoint = {{10,10},{14,10},{14,14},{10,14}};

  try
    {
      // ---------------------------------------------------------------
      // Constructor
      // ---------------------------------------------------------------
      BDSPolygon polyA(ptsA);
      Check(polyA.size() == 4, "constructor: size");

      // Fewer than 3 points must throw.
      {
        bool threw = false;
        try {BDSPolygon bad({{0,0},{1,0}});} catch (const BDSException&) {threw = true;}
        Check(threw, "constructor: <3 points throws BDSException");
      }

      // CCW input (det > 0) is left unchanged.
      // ptsA: detV1=(4,0), detV2=(0,4), det=16>0 → stays.
      Check(VecNear(polyA.Points()[0], G4TwoVector(0,0)), "winding: CCW input preserved [0]");
      Check(VecNear(polyA.Points()[1], G4TwoVector(4,0)), "winding: CCW input preserved [1]");

      // CW input (det < 0) is reversed to CCW.
      // [(0,0),(0,4),(4,4),(4,0)] is CW: detV1=(0,4), detV2=(4,0), det=-16<0 → reversed.
      BDSPolygon cwPoly(std::vector<G4TwoVector>{{0,0},{0,4},{4,4},{4,0}});
      Check(VecNear(cwPoly.Points()[0], G4TwoVector(4,0)), "winding: CW input reversed [0]");
      Check(VecNear(cwPoly.Points()[1], G4TwoVector(4,4)), "winding: CW input reversed [1]");

      // ---------------------------------------------------------------
      // Copy constructor
      // ---------------------------------------------------------------
      BDSPolygon copyA(polyA);
      Check(copyA.size() == 4, "copy constructor: size");
      Check(VecNear(copyA.Points()[0], G4TwoVector(0,0)), "copy constructor: point");

      // ---------------------------------------------------------------
      // Inside(point)
      // ---------------------------------------------------------------
      Check( polyA.Inside(G4TwoVector(2,   2  )), "Inside(pt): centre");
      Check( polyA.Inside(G4TwoVector(0.1, 0.1)), "Inside(pt): near corner");
      Check(!polyA.Inside(G4TwoVector(5,   2  )), "Inside(pt): outside right");
      Check(!polyA.Inside(G4TwoVector(2,   5  )), "Inside(pt): outside top");
      Check(!polyA.Inside(G4TwoVector(-1,  2  )), "Inside(pt): outside left");
      Check(!polyA.Inside(G4TwoVector(2,  -1  )), "Inside(pt): outside bottom");
      // Closing edge (left side, from (0,4) back to (0,0)) must be checked.
      Check(!polyA.Inside(G4TwoVector(-0.5, 2)), "Inside(pt): closing edge tested");

      // ---------------------------------------------------------------
      // Inside(polygon)
      // The implementation tests whether *this* is inside *other*:
      // no segment intersections and a vertex of *this* is inside *other*.
      // ---------------------------------------------------------------
      BDSPolygon polySmall(ptsSmall);
      BDSPolygon polyB(ptsB);
      // polySmall.Inside(polyA): no intersections, (1,1) ∈ polyA → true.
      Check( polySmall.Inside(polyA),  "Inside(poly): small inside large");
      // polyA.Inside(polySmall): (0,0) ∉ polySmall → false.
      Check(!polyA.Inside(polySmall),  "Inside(poly): large not inside small");
      // Partial overlap: edges intersect → false.
      Check(!polyA.Inside(polyB),      "Inside(poly): overlapping polygons");

      // ---------------------------------------------------------------
      // SelfIntersecting
      // ---------------------------------------------------------------
      Check(!polyA.SelfIntersecting(), "SelfIntersecting: convex square");
      // Bowtie: [(0,0),(4,4),(0,4),(4,0)] — diagonals cross.
      // det of first two edges > 0, stays as-is.  Edge 0 and edge 2 cross at (2,2).
      BDSPolygon bowtie(std::vector<G4TwoVector>{{0,0},{4,4},{0,4},{4,0}});
      Check( bowtie.SelfIntersecting(), "SelfIntersecting: bowtie polygon");

      // ---------------------------------------------------------------
      // ScaleByValue / ScaleByValueInPlace
      // ---------------------------------------------------------------
      BDSPolygon scaled = polyA.ScaleByValue(2.0);
      Check(scaled.size() == 4,                            "ScaleByValue: size");
      Check(VecNear(scaled.Points()[0], G4TwoVector(0,0)), "ScaleByValue: (0,0)*2");
      Check(VecNear(scaled.Points()[1], G4TwoVector(8,0)), "ScaleByValue: (4,0)*2");
      Check(VecNear(scaled.Points()[2], G4TwoVector(8,8)), "ScaleByValue: (4,4)*2");

      BDSPolygon scaledIP(ptsA);
      scaledIP.ScaleByValueInPlace(0.5);
      Check(VecNear(scaledIP.Points()[1], G4TwoVector(2,0)), "ScaleByValueInPlace: (4,0)*0.5");
      Check(VecNear(scaledIP.Points()[2], G4TwoVector(2,2)), "ScaleByValueInPlace: (4,4)*0.5");

      // ---------------------------------------------------------------
      // RadiusToEncompass — max |point|, here the farthest vertex is (4,4).
      // ---------------------------------------------------------------
      G4double r = polyA.RadiusToEncompass();
      Check(Near(r, std::sqrt(32.0)), "RadiusToEncompass: diagonal distance");
      // After scaling the cached value is invalidated.
      BDSPolygon polyR(ptsA);
      polyR.ScaleByValueInPlace(2.0);
      Check(Near(polyR.RadiusToEncompass(), std::sqrt(128.0)), "RadiusToEncompass: recalculated after scale");

      // ---------------------------------------------------------------
      // MinimumInscribedCricleRadius — minimum distance from origin to the boundary
      // ---------------------------------------------------------------

      // Unit square with sides at x=±1, y=±1: all four sides are at distance 1.
      // Vertices are at distance √2, so the segment formula drives the result.
      {
        BDSPolygon sq(std::vector<G4TwoVector>{{1,-1},{1,1},{-1,1},{-1,-1}});
        Check(Near(sq.MinimumInscribedCricleRadius(), 1.0), "MinimumInscribedCricleRadius: unit square");
      }

      // Rectangle 2×1: y=±1 sides (distance 1) are closer than x=±2 sides (distance 2).
      {
        BDSPolygon rect(std::vector<G4TwoVector>{{2,-1},{2,1},{-2,1},{-2,-1}});
        Check(Near(rect.MinimumInscribedCricleRadius(), 1.0), "MinimumInscribedCricleRadius: 2x1 rectangle → short dimension");
      }

      // Regular hexagon with circumradius 1: apothem = cos(30°) = √3/2 ≈ 0.866.
      {
        const G4double c = 0.5, s = std::sqrt(3.0)/2;
        BDSPolygon hex(std::vector<G4TwoVector>{{1,0},{c,s},{-c,s},{-1,0},{-c,-s},{c,-s}});
        Check(Near(hex.MinimumInscribedCricleRadius(), std::sqrt(3.0)/2, 1e-9),
              "MinimumInscribedCricleRadius: unit hexagon → apothem");
      }

      // Equilateral triangle with circumradius 2: inradius = R/2 = 1.
      // Vertices at (2,0), (-1,√3), (-1,-√3).
      {
        const G4double s3 = std::sqrt(3.0);
        BDSPolygon tri(std::vector<G4TwoVector>{{2,0},{-1,s3},{-1,-s3}});
        Check(Near(tri.MinimumInscribedCricleRadius(), 1.0, 1e-9),
              "MinimumInscribedCricleRadius: equilateral triangle circumradius=2");
      }

      // Cache must be invalidated after ScaleByValueInPlace.
      // Prime the cache on the unit square (MinimumInscribedCricleRadius=1), then scale by 3 (expect 3).
      {
        BDSPolygon scPoly(std::vector<G4TwoVector>{{1,-1},{1,1},{-1,1},{-1,-1}});
        (void)scPoly.MinimumInscribedCricleRadius(); // prime cache
        scPoly.ScaleByValueInPlace(3.0);
        Check(Near(scPoly.MinimumInscribedCricleRadius(), 3.0),
              "MinimumInscribedCricleRadius: recalculated after ScaleByValueInPlace");
      }

      // Cache must be invalidated after ExpandByValueUsingVertexNormalsInPlace.
      // Unit square expanded by 1 → half-width 2 (approx; exact depends on corner normals).
      {
        BDSPolygon expPoly(std::vector<G4TwoVector>{{1,-1},{1,1},{-1,1},{-1,-1}});
        (void)expPoly.MinimumInscribedCricleRadius(); // prime cache
        expPoly.ExpandByValueUsingVertexNormalsInPlace(1.0);
        // Each vertex normal for a square corner is (±1,±1)/√2, so the new sides are still
        // at distance 1 + 1/√2·√2 = 2 from the origin (the full side shifts by 1).
        // Regardless of the exact value, it must differ from 1.0.
        Check(!Near(expPoly.MinimumInscribedCricleRadius(), 1.0),
              "MinimumInscribedCricleRadius: recalculated after ExpandByValueUsingVertexNormalsInPlace");
      }

      // ScaleByValue (non-in-place) returns a new polygon; its MinimumInscribedCricleRadius must be correct.
      {
        BDSPolygon base2(std::vector<G4TwoVector>{{1,-1},{1,1},{-1,1},{-1,-1}});
        BDSPolygon scaled2 = base2.ScaleByValue(4.0);
        Check(Near(scaled2.MinimumInscribedCricleRadius(), 4.0),
              "MinimumInscribedCricleRadius: correct on polygon returned by ScaleByValue");
      }

      // ---------------------------------------------------------------
      // ExpandByValueUsingVertexNormals / InPlace
      // ---------------------------------------------------------------
      BDSPolygon expanded = polyA.ExpandByValueUsingVertexNormals(1.0);
      Check(expanded.size() == 4, "ExpandByValue: size unchanged");
      // Each vertex of a unit square is pushed outward along (+/-1,+/-1)/√2.
      // Vertex (4,0) (index 1) gets (+1,-1)/√2 ≈ (+0.707,-0.707).
      const G4TwoVector ep1 = expanded.Points()[1];
      Check(ep1.x() > 4.0 && ep1.y() < 0.0, "ExpandByValue: vertex pushed outward");

      BDSPolygon expandedIP(ptsA);
      expandedIP.ExpandByValueUsingVertexNormalsInPlace(1.0);
      Check(expandedIP.size() == 4, "ExpandByValueInPlace: size unchanged");
      const G4TwoVector ep1ip = expandedIP.Points()[1];
      Check(ep1ip.x() > 4.0 && ep1ip.y() < 0.0, "ExpandByValueInPlace: vertex pushed outward");

      // ---------------------------------------------------------------
      // ApplyTiltOffset
      // ---------------------------------------------------------------
      // Pure x-offset of 1.
      BDSTiltOffset toX(1.0, 0.0, 0.0);
      BDSPolygon shiftedX = polyA.ApplyTiltOffset(toX);
      Check(VecNear(shiftedX.Points()[0], G4TwoVector(1,0)), "ApplyTiltOffset: x offset [0]");
      Check(VecNear(shiftedX.Points()[1], G4TwoVector(5,0)), "ApplyTiltOffset: x offset [1]");

      // Pure y-offset of 2.
      BDSTiltOffset toY(0.0, 2.0, 0.0);
      BDSPolygon shiftedY = polyA.ApplyTiltOffset(toY);
      Check(VecNear(shiftedY.Points()[0], G4TwoVector(0,2)), "ApplyTiltOffset: y offset [0]");

      // 90-degree (π/2) tilt: (4,0) rotates CCW to (0,4).
      BDSTiltOffset toTilt(0.0, 0.0, CLHEP::halfpi);
      BDSPolygon tilted = polyA.ApplyTiltOffset(toTilt);
      Check(VecNear(tilted.Points()[1], G4TwoVector(0,4), 1e-9), "ApplyTiltOffset: 90deg tilt");

      // Zero tilt and zero offset → no change.
      BDSTiltOffset toNone(0.0, 0.0, 0.0);
      BDSPolygon unchanged = polyA.ApplyTiltOffset(toNone);
      Check(VecNear(unchanged.Points()[0], G4TwoVector(0,0)), "ApplyTiltOffset: no-op");

      // ---------------------------------------------------------------
      // SegmentsIntersect (static)
      // ---------------------------------------------------------------
      // Two crossing segments: horizontal (0,2)-(4,2) and vertical (2,0)-(2,4).
      {
        G4TwoVector ip;
        G4int r1 = BDSPolygon::SegmentsIntersect(G4TwoVector(0,2), G4TwoVector(4,2),
                                                  G4TwoVector(2,0), G4TwoVector(2,4), &ip);
        Check(r1 == 1, "SegmentsIntersect: crossing segments → 1");
        Check(VecNear(ip, G4TwoVector(2,2), 1e-9), "SegmentsIntersect: intersection point");
      }
      // Parallel segments → 0.
      {
        G4int r2 = BDSPolygon::SegmentsIntersect(G4TwoVector(0,0), G4TwoVector(2,0),
                                                  G4TwoVector(0,1), G4TwoVector(2,1));
        Check(r2 == 0, "SegmentsIntersect: parallel segments → 0");
      }
      // Collinear overlapping segments → 2.
      {
        G4int r3 = BDSPolygon::SegmentsIntersect(G4TwoVector(0,0), G4TwoVector(4,0),
                                                  G4TwoVector(2,0), G4TwoVector(6,0));
        Check(r3 == 2, "SegmentsIntersect: collinear segments → 2");
      }

      // ---------------------------------------------------------------
      // Union
      // ---------------------------------------------------------------
      // B entirely inside A → result == A.
      BDSPolygon unionContained = polyA.Union(polySmall);
      Check(unionContained.size() == 4,                              "Union: B⊂A returns A (size)");
      Check(VecNear(unionContained.Points()[0], polyA.Points()[0]), "Union: B⊂A returns A (point)");

      // A entirely inside big → result == big.
      BDSPolygon polyBig(ptsBig);
      BDSPolygon unionInsideBig = polyA.Union(polyBig);
      Check(unionInsideBig.size() == 4, "Union: A⊂B returns B (size)");

      // Partial overlap of A and B: the result has 8 vertices (4 from A, 4 from B,
      // minus 2 hidden corners, plus 2 intersection points).
      BDSPolygon unionAB = polyA.Union(polyB);
      Check(unionAB.size() == 8, "Union: partial overlap vertex count");
      // Key intersection points and outer corners must all be present.
      Check(PolyContainsAll(unionAB, {{0,0},{4,0},{4,2},{6,2},{6,6},{2,6},{2,4},{0,4}}),
            "Union: partial overlap contains expected vertices");

      // Disjoint polygons must throw.
      {
        BDSPolygon polyDisjoint(ptsDisjoint);
        bool threw = false;
        try {polyA.Union(polyDisjoint);} catch (const BDSException&) {threw = true;}
        Check(threw, "Union: disjoint polygons throw BDSException");
      }

      // ---------------------------------------------------------------
      // Intersection
      // ---------------------------------------------------------------
      // B entirely inside A → result == B.
      BDSPolygon isectContained = polyA.Intersection(polySmall);
      Check(isectContained.size() == 4, "Intersection: B⊂A returns B (size)");
      Check(PolyContainsAll(isectContained, ptsSmall), "Intersection: B⊂A returns B (points)");

      // A entirely inside big → result == A.
      BDSPolygon isectInsideBig = polyA.Intersection(polyBig);
      Check(isectInsideBig.size() == 4, "Intersection: A⊂B returns A (size)");

      // Partial overlap: intersection is the square (2,2)-(4,4) — 4 vertices.
      BDSPolygon isectAB = polyA.Intersection(polyB);
      Check(isectAB.size() == 4, "Intersection: partial overlap vertex count");
      Check(PolyContainsAll(isectAB, {{4,4},{2,4},{2,2},{4,2}}),
            "Intersection: partial overlap contains expected vertices");
      // The intersection polygon is convex, so Inside(point) is reliable.
      Check( isectAB.Inside(G4TwoVector(3,3)), "Intersection: overlap region inside result");
      Check(!isectAB.Inside(G4TwoVector(1,1)), "Intersection: A-only region outside result");
      Check(!isectAB.Inside(G4TwoVector(5,5)), "Intersection: B-only region outside result");

      // Non-overlapping polygons must throw.
      {
        BDSPolygon polyDisjoint(ptsDisjoint);
        bool threw = false;
        try {polyA.Intersection(polyDisjoint);} catch (const BDSException&) {threw = true;}
        Check(threw, "Intersection: disjoint polygons throw BDSException");
      }

      // ---------------------------------------------------------------
      // Subtraction
      // ---------------------------------------------------------------
      // A entirely inside big → empty result.
      {
        auto sub = polyA.Subtraction(polyBig);
        Check(sub.empty(), "Subtraction: A⊂B → empty result");
      }

      // Disjoint → result == A.
      {
        BDSPolygon polyDisjoint(ptsDisjoint);
        auto sub = polyA.Subtraction(polyDisjoint);
        Check(sub.size() == 1,         "Subtraction: disjoint → one polygon");
        Check(sub[0]->size() == 4,     "Subtraction: disjoint result == A (size)");
        Check(PolyContainsAll(*sub[0], ptsA), "Subtraction: disjoint result == A (points)");
        delete sub[0];
      }

      // B enclosed in A → not supported, must throw.
      {
        bool threw = false;
        try {polyA.Subtraction(polySmall);} catch (const BDSException&) {threw = true;}
        Check(threw, "Subtraction: B⊂A (hole) throws BDSException");
      }

      // Partial overlap: A-B is the L-shaped hexagon with 6 vertices.
      {
        auto sub = polyA.Subtraction(polyB);
        Check(sub.size() == 1,     "Subtraction: partial overlap → one polygon");
        Check(sub[0]->size() == 6, "Subtraction: partial overlap vertex count");
        Check(PolyContainsAll(*sub[0], {{0,0},{4,0},{4,2},{2,2},{2,4},{0,4}}),
              "Subtraction: partial overlap contains expected vertices");
        delete sub[0];
      }

      // ---------------------------------------------------------------
      // InterpolateWithNPoints
      // ---------------------------------------------------------------
      BDSPolygon interp = polyA.InterpolateWithNPoints(8);
      Check(interp.size() == 8, "InterpolateWithNPoints: output size");

      // ---------------------------------------------------------------
      // Iterator / size / empty
      // ---------------------------------------------------------------
      G4int count = 0;
      for (const auto& p : polyA) {count++; (void)p;}
      Check(count == 4,     "iterator: range-for visits all points");
      Check(!polyA.empty(), "empty(): non-empty polygon");

      BDSPolygon polyFront(ptsA);
      Check(VecNear(polyFront.front(), G4TwoVector(0,0)), "front()");
      Check(VecNear(polyFront.back(),  G4TwoVector(0,4)), "back()");
    }
  catch (const BDSException& e)
    {
      std::cout << "UNEXPECTED EXCEPTION: " << e.what() << "\n";
      nFailed++;
    }
  catch (const std::exception& e)
    {
      std::cout << "UNEXPECTED STD EXCEPTION: " << e.what() << "\n";
      nFailed++;
    }

  std::cout << "\n" << nPassed << " passed, " << nFailed << " failed\n";
  return nFailed > 0 ? 1 : 0;
}
