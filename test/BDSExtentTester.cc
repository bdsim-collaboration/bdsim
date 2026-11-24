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
#include "BDSExtent.hh"
#include "BDSParticleCoords.hh"

#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"
#include "G4Types.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>
#include <iostream>
#include <string>
#include <utility>
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

bool TVNear(const G4ThreeVector& a, const G4ThreeVector& b, G4double tol = 1e-9)
{return (a - b).mag() < tol;}

int main()
{
  // -----------------------------------------------------------------------
  // Constructors
  // -----------------------------------------------------------------------

  // Default — all zero
  {
    BDSExtent e;
    Check(e.XNeg() == 0 && e.XPos() == 0 &&
          e.YNeg() == 0 && e.YPos() == 0 &&
          e.ZNeg() == 0 && e.ZPos() == 0,
          "constructor: default → all zero");
  }

  // Symmetric (x,y,z) — stores ±|value|
  {
    BDSExtent e(3, 4, 5);
    Check(Near(e.XNeg(), -3) && Near(e.XPos(), 3), "constructor: symmetric X");
    Check(Near(e.YNeg(), -4) && Near(e.YPos(), 4), "constructor: symmetric Y");
    Check(Near(e.ZNeg(), -5) && Near(e.ZPos(), 5), "constructor: symmetric Z");
  }

  // Symmetric with negative inputs — abs is taken
  {
    BDSExtent e(-3, -4, -5);
    Check(Near(e.XNeg(), -3) && Near(e.XPos(), 3), "constructor: symmetric negative input X");
  }

  // Asymmetric (6 values)
  {
    BDSExtent e(-1, 2, -3, 4, -5, 6);
    Check(Near(e.XNeg(), -1) && Near(e.XPos(), 2), "constructor: asymmetric X");
    Check(Near(e.YNeg(), -3) && Near(e.YPos(), 4), "constructor: asymmetric Y");
    Check(Near(e.ZNeg(), -5) && Near(e.ZPos(), 6), "constructor: asymmetric Z");
  }

  // Pair constructor
  {
    BDSExtent e(std::make_pair(-1.0, 2.0),
                std::make_pair(-3.0, 4.0),
                std::make_pair(-5.0, 6.0));
    Check(Near(e.XNeg(), -1) && Near(e.XPos(), 2), "constructor: pair X");
    Check(Near(e.YNeg(), -3) && Near(e.YPos(), 4), "constructor: pair Y");
    Check(Near(e.ZNeg(), -5) && Near(e.ZPos(), 6), "constructor: pair Z");
  }

  // ThreeVector symmetric
  {
    BDSExtent e(G4ThreeVector(2, 3, 4));
    Check(Near(e.XNeg(), -2) && Near(e.XPos(), 2), "constructor: ThreeVector symmetric X");
    Check(Near(e.YNeg(), -3) && Near(e.YPos(), 3), "constructor: ThreeVector symmetric Y");
    Check(Near(e.ZNeg(), -4) && Near(e.ZPos(), 4), "constructor: ThreeVector symmetric Z");
  }

  // ThreeVector neg + pos
  {
    BDSExtent e(G4ThreeVector(-1, -2, -3), G4ThreeVector(4, 5, 6));
    Check(Near(e.XNeg(), -1) && Near(e.XPos(), 4), "constructor: ThreeVector neg+pos X");
    Check(Near(e.YNeg(), -2) && Near(e.YPos(), 5), "constructor: ThreeVector neg+pos Y");
    Check(Near(e.ZNeg(), -3) && Near(e.ZPos(), 6), "constructor: ThreeVector neg+pos Z");
  }

  // -----------------------------------------------------------------------
  // Pair accessors
  // -----------------------------------------------------------------------
  {
    BDSExtent e(-1, 2, -3, 4, -5, 6);
    auto px = e.ExtentX();
    Check(Near(px.first, -1) && Near(px.second, 2), "ExtentX() pair");
    auto py = e.ExtentY();
    Check(Near(py.first, -3) && Near(py.second, 4), "ExtentY() pair");
    auto pz = e.ExtentZ();
    Check(Near(pz.first, -5) && Near(pz.second, 6), "ExtentZ() pair");
  }

  // ThreeVector accessors
  {
    BDSExtent e(-1, 2, -3, 4, -5, 6);
    Check(TVNear(e.ExtentPositive(), G4ThreeVector(2, 4, 6)),    "ExtentPositive()");
    Check(TVNear(e.ExtentNegative(), G4ThreeVector(-1, -3, -5)), "ExtentNegative()");
  }

  // -----------------------------------------------------------------------
  // DX / DY / DZ
  // -----------------------------------------------------------------------
  {
    BDSExtent e(-1, 3, -2, 4, -3, 5);
    Check(Near(e.DX(), 4), "DX()");
    Check(Near(e.DY(), 6), "DY()");
    Check(Near(e.DZ(), 8), "DZ()");
  }

  // -----------------------------------------------------------------------
  // AllBoundaryPoints — 8 corners
  // -----------------------------------------------------------------------
  {
    BDSExtent e(1, 2, 3); // x:[-1,1], y:[-2,2], z:[-3,3]
    auto pts = e.AllBoundaryPoints();
    Check(pts.size() == 8, "AllBoundaryPoints: size 8");

    bool allFound = true;
    for (int sx : {-1, 1}) for (int sy : {-1, 1}) for (int sz : {-1, 1})
      {
        G4ThreeVector corner(sx * 1.0, sy * 2.0, sz * 3.0);
        bool found = false;
        for (const auto& p : pts)
          {if (TVNear(p, corner)) {found = true; break;}}
        if (!found)
          {allFound = false;}
      }
    Check(allFound, "AllBoundaryPoints: all 8 corners present");
  }

  // -----------------------------------------------------------------------
  // AllBoundaryPointsXY — 4 XY corners
  // -----------------------------------------------------------------------
  {
    BDSExtent e(1, 2, 3);
    auto pts = e.AllBoundaryPointsXY();
    Check(pts.size() == 4, "AllBoundaryPointsXY: size 4");

    bool allFound = true;
    for (int sx : {-1, 1}) for (int sy : {-1, 1})
      {
        G4TwoVector corner(sx * 1.0, sy * 2.0);
        bool found = false;
        for (const auto& p : pts)
          {if ((p - corner).mag() < 1e-9) {found = true; break;}}
        if (!found)
          {allFound = false;}
      }
    Check(allFound, "AllBoundaryPointsXY: all 4 corners present");
  }

  // -----------------------------------------------------------------------
  // Equality
  // -----------------------------------------------------------------------
  {
    BDSExtent a(-1, 2, -3, 4, -5, 6);
    BDSExtent b(-1, 2, -3, 4, -5, 6);
    BDSExtent c(-1, 2, -3, 4, -5, 7);
    Check(  a == b,  "operator==: equal extents");
    Check(!(a == c), "operator==: unequal extents");
    Check(!(a != b), "operator!=: equal extents");
    Check(  a != c,  "operator!=: unequal extents");
  }

  // -----------------------------------------------------------------------
  // 3D comparison operators (strict containment in all dimensions)
  // -----------------------------------------------------------------------
  {
    BDSExtent small(-1, 1, -1, 1, -1, 1);
    BDSExtent big(  -2, 2, -2, 2, -2, 2);
    BDSExtent same(-1, 1, -1, 1, -1, 1);

    Check(  small < big,   "operator<: small strictly inside big");
    Check(!(big   < small),"operator<: big not inside small");
    Check(!(small < same), "operator<: equal extents not strictly less");
    Check(  big   > small, "operator>: big > small");
    Check(!(small > big),  "operator>: small not > big");
    Check(  small <= big,  "operator<=: small <= big");
    Check(  small <= same, "operator<=: equal satisfies <=");
    Check(  big   >= small,"operator>=: big >= small");
    Check(  small >= same, "operator>=: equal satisfies >=");
  }

  // -----------------------------------------------------------------------
  // Transverse comparison (XY only, Z ignored)
  // -----------------------------------------------------------------------
  {
    BDSExtent small(-1, 1, -1, 1, -5, 5); // transversely small, long in Z
    BDSExtent big(  -2, 2, -2, 2, -5, 5);
    BDSExtent bigZ( -1, 1, -1, 1, -9, 9); // same XY as small, larger Z

    Check(  small.TransverselyLessThan(big),        "TransverselyLessThan: small < big");
    Check(!(big.TransverselyLessThan(small)),        "TransverselyLessThan: big not < small");
    // Same XY but different Z — Z is ignored, so not transversely less
    Check(!bigZ.TransverselyLessThan(small),         "TransverselyLessThan: equal XY → false");
    Check( big.TransverselyGreaterThan(small),       "TransverselyGreaterThan: big > small");
    Check( small.TransverselyLessEquals(big),        "TransverselyLessEquals: small <= big");
    Check( small.TransverselyLessEquals(bigZ),       "TransverselyLessEquals: equal XY → true");
    Check( big.TransverselyGreaterEquals(small),     "TransverselyGreaterEquals: big >= small");
    Check( small.TransverselyGreaterEquals(bigZ),    "TransverselyGreaterEquals: equal XY → true");
  }

  // -----------------------------------------------------------------------
  // Encompasses(point)
  // -----------------------------------------------------------------------
  {
    BDSExtent e(2, 3, 4); // x:[-2,2], y:[-3,3], z:[-4,4]
    Check( e.Encompasses(G4ThreeVector(0, 0, 0)),   "Encompasses(pt): centre");
    Check( e.Encompasses(G4ThreeVector(1, 1, 1)),   "Encompasses(pt): interior");
    // Boundary is inclusive
    Check( e.Encompasses(G4ThreeVector(2, 0, 0)),   "Encompasses(pt): on x+ boundary");
    Check( e.Encompasses(G4ThreeVector(-2, 0, 0)),  "Encompasses(pt): on x- boundary");
    Check( e.Encompasses(G4ThreeVector(0, 3, 0)),   "Encompasses(pt): on y+ boundary");
    Check( e.Encompasses(G4ThreeVector(0, 0, -4)),  "Encompasses(pt): on z- boundary");
    // Outside in each axis
    Check(!e.Encompasses(G4ThreeVector(3, 0, 0)),   "Encompasses(pt): beyond x+");
    Check(!e.Encompasses(G4ThreeVector(0, 4, 0)),   "Encompasses(pt): beyond y+");
    Check(!e.Encompasses(G4ThreeVector(0, 0, 5)),   "Encompasses(pt): beyond z+");
    Check(!e.Encompasses(G4ThreeVector(-3, 0, 0)),  "Encompasses(pt): beyond x-");
  }

  // Encompasses(x,y,z) convenience overload
  {
    BDSExtent e(2, 3, 4);
    Check( e.Encompasses(1, 1, 1),  "Encompasses(x,y,z): interior");
    Check(!e.Encompasses(3, 0, 0),  "Encompasses(x,y,z): outside");
  }

  // Encompasses(BDSParticleCoords)
  {
    BDSExtent e(2, 3, 4);
    BDSParticleCoords inside(1, 1, 1,  0, 0, 0, 0);
    BDSParticleCoords outside(5, 0, 0, 0, 0, 0, 0);
    Check( e.Encompasses(inside),  "Encompasses(coords): inside");
    Check(!e.Encompasses(outside), "Encompasses(coords): outside");
  }

  // -----------------------------------------------------------------------
  // Encompasses(extent)
  // -----------------------------------------------------------------------
  {
    BDSExtent outer(-4, 4, -4, 4, -4, 4);
    BDSExtent inner(-1, 1, -1, 1, -1, 1);
    BDSExtent overlapping(-2, 6, -2, 2, -2, 2); // extends beyond outer in x+

    Check( outer.Encompasses(inner),       "Encompasses(extent): outer contains inner");
    Check(!inner.Encompasses(outer),       "Encompasses(extent): inner does not contain outer");
    Check(!outer.Encompasses(overlapping), "Encompasses(extent): partial overlap → false");
    // Self-containment
    Check( outer.Encompasses(outer),       "Encompasses(extent): self-containment");
  }

  // -----------------------------------------------------------------------
  // Translate
  // -----------------------------------------------------------------------
  {
    BDSExtent e(1, 2, 3); // x:[-1,1], y:[-2,2], z:[-3,3]

    BDSExtent shifted = e.Translate(1, 2, 3);
    Check(Near(shifted.XNeg(),  0) && Near(shifted.XPos(), 2), "Translate: X");
    Check(Near(shifted.YNeg(),  0) && Near(shifted.YPos(), 4), "Translate: Y");
    Check(Near(shifted.ZNeg(),  0) && Near(shifted.ZPos(), 6), "Translate: Z");

    // Original is unmodified
    Check(Near(e.XNeg(), -1), "Translate: original unchanged");

    // ThreeVector overload
    BDSExtent shifted2 = e.Translate(G4ThreeVector(-1, -2, -3));
    Check(Near(shifted2.XNeg(), -2) && Near(shifted2.XPos(), 0), "Translate(ThreeVector): X");
    Check(Near(shifted2.YNeg(), -4) && Near(shifted2.YPos(), 0), "Translate(ThreeVector): Y");
  }

  // -----------------------------------------------------------------------
  // Tilted
  // -----------------------------------------------------------------------
  {
    BDSExtent e(3, 4, 5); // x:[-3,3], y:[-4,4], z:[-5,5]

    // Zero angle → unchanged copy
    BDSExtent t0 = e.Tilted(0);
    Check(t0 == e, "Tilted: angle=0 → unchanged");

    // 90° CCW rotation of (±3, ±4) bounding box:
    //   ( 3, 4) → (-4,  3)
    //   ( 3,-4) → ( 4,  3)
    //   (-3,-4) → ( 4, -3)
    //   (-3, 4) → (-4, -3)
    // → x: [-4, 4], y: [-3, 3], z: unchanged
    BDSExtent t90 = e.Tilted(CLHEP::halfpi);
    Check(Near(t90.XNeg(), -4, 1e-9), "Tilted: 90deg XNeg");
    Check(Near(t90.XPos(),  4, 1e-9), "Tilted: 90deg XPos");
    Check(Near(t90.YNeg(), -3, 1e-9), "Tilted: 90deg YNeg");
    Check(Near(t90.YPos(),  3, 1e-9), "Tilted: 90deg YPos");
    Check(Near(t90.ZNeg(), -5, 1e-9), "Tilted: 90deg ZNeg unchanged");
    Check(Near(t90.ZPos(),  5, 1e-9), "Tilted: 90deg ZPos unchanged");

    // 180° rotation of symmetric extent → identical to original
    BDSExtent t180 = e.Tilted(CLHEP::pi);
    Check(Near(t180.XNeg(), -3, 1e-9) && Near(t180.XPos(), 3, 1e-9), "Tilted: 180deg X");
    Check(Near(t180.YNeg(), -4, 1e-9) && Near(t180.YPos(), 4, 1e-9), "Tilted: 180deg Y");

    // 45° rotation of unit square (1,1,0):
    //   corners at (±1,±1) → all at distance √2 → bounding box ±√2
    BDSExtent sq(1, 1, 0);
    BDSExtent t45 = sq.Tilted(CLHEP::pi / 4);
    Check(Near(t45.XNeg(), -std::sqrt(2.0), 1e-9), "Tilted: 45deg square XNeg");
    Check(Near(t45.XPos(),  std::sqrt(2.0), 1e-9), "Tilted: 45deg square XPos");
    Check(Near(t45.YNeg(), -std::sqrt(2.0), 1e-9), "Tilted: 45deg square YNeg");
    Check(Near(t45.YPos(),  std::sqrt(2.0), 1e-9), "Tilted: 45deg square YPos");
  }

  // -----------------------------------------------------------------------
  // MaximumAbs / MinimumAbs
  // -----------------------------------------------------------------------
  {
    BDSExtent e(-1, 2, -3, 4, -5, 6);
    Check(Near(e.MaximumAbs(), 6), "MaximumAbs: largest abs is ZPos=6");
    Check(Near(e.MinimumAbs(), 1), "MinimumAbs: smallest abs is |XNeg|=1");
  }
  {
    // Asymmetric — negative side dominates
    BDSExtent e(-7, 2, -3, 4, -5, 6);
    Check(Near(e.MaximumAbs(), 7), "MaximumAbs: negative side dominates");
  }

  // -----------------------------------------------------------------------
  // MaximumX / MaximumY / MaximumZ
  // -----------------------------------------------------------------------
  {
    BDSExtent e(-3, 2, -4, 1, -5, 6);
    Check(Near(e.MaximumX(), 3), "MaximumX: max(|-3|, 2)=3");
    Check(Near(e.MaximumY(), 4), "MaximumY: max(|-4|, 1)=4");
    Check(Near(e.MaximumZ(), 6), "MaximumZ: max(|-5|, 6)=6");
  }

  // -----------------------------------------------------------------------
  // MaximumAbsTransverse / MinimumAbsTransverse
  // -----------------------------------------------------------------------
  {
    BDSExtent e(-3, 2, -4, 1, -10, 10); // large Z should be ignored
    Check(Near(e.MaximumAbsTransverse(), 4), "MaximumAbsTransverse: largest XY abs");
    Check(Near(e.MinimumAbsTransverse(), 1), "MinimumAbsTransverse: smallest XY abs");
  }

  // -----------------------------------------------------------------------
  // TransverseBoundingRadius
  // -----------------------------------------------------------------------
  {
    // Symmetric (3,4,5): max|x|=3, max|y|=4 → hypot(3,4)=5
    BDSExtent e(3, 4, 5);
    Check(Near(e.TransverseBoundingRadius(), 5.0), "TransverseBoundingRadius: 3-4-5 right triangle");

    // Asymmetric: x:[-5,1], y:[-1,2] → max|x|=5, max|y|=2 → hypot(5,2)=√29
    BDSExtent e2(-5, 1, -1, 2, 0, 0);
    Check(Near(e2.TransverseBoundingRadius(), std::sqrt(29.0)), "TransverseBoundingRadius: asymmetric");
  }

  // -----------------------------------------------------------------------
  // ExpandBy
  // -----------------------------------------------------------------------
  {
    BDSExtent e(1, 2, 3); // x:[-1,1], y:[-2,2], z:[-3,3]
    BDSExtent exp = e.ExpandBy(0.5);
    Check(Near(exp.XNeg(), -1.5) && Near(exp.XPos(), 1.5), "ExpandBy: X expanded");
    Check(Near(exp.YNeg(), -2.5) && Near(exp.YPos(), 2.5), "ExpandBy: Y expanded");
    Check(Near(exp.ZNeg(), -3.5) && Near(exp.ZPos(), 3.5), "ExpandBy: Z expanded");
    // Original unmodified
    Check(Near(e.XNeg(), -1), "ExpandBy: original unchanged");
    // Negative margin is treated as abs
    BDSExtent expNeg = e.ExpandBy(-0.5);
    Check(Near(expNeg.XNeg(), -1.5), "ExpandBy: negative margin → abs taken");
  }

  // -----------------------------------------------------------------------
  // ExpandTransverselyBy
  // -----------------------------------------------------------------------
  {
    BDSExtent e(1, 2, 3);
    BDSExtent exp = e.ExpandTransverselyBy(1.0);
    Check(Near(exp.XNeg(), -2) && Near(exp.XPos(), 2), "ExpandTransverselyBy: X expanded");
    Check(Near(exp.YNeg(), -3) && Near(exp.YPos(), 3), "ExpandTransverselyBy: Y expanded");
    Check(Near(exp.ZNeg(), -3) && Near(exp.ZPos(), 3), "ExpandTransverselyBy: Z unchanged");
  }

  // -----------------------------------------------------------------------
  // ExpandToEncompass — modifies *this in-place
  // Note: the current implementation modifies a local copy and does NOT
  // update *this; these tests will FAIL and reveal that bug.
  // -----------------------------------------------------------------------
  {
    BDSExtent e(-1, 1, -2, 2, -3, 3);
    BDSExtent other(-3, 0, -1, 4, -2, 5);
    e.ExpandToEncompass(other);
    // Expected: take the most extreme bound in each direction
    Check(Near(e.XNeg(), -3), "ExpandToEncompass: XNeg = min(-1,-3) = -3");
    Check(Near(e.XPos(),  1), "ExpandToEncompass: XPos = max( 1, 0) =  1");
    Check(Near(e.YNeg(), -2), "ExpandToEncompass: YNeg = min(-2,-1) = -2");
    Check(Near(e.YPos(),  4), "ExpandToEncompass: YPos = max( 2, 4) =  4");
    Check(Near(e.ZNeg(), -3), "ExpandToEncompass: ZNeg = min(-3,-2) = -3");
    Check(Near(e.ZPos(),  5), "ExpandToEncompass: ZPos = max( 3, 5) =  5");
    // After encompassing, *this should contain the other extent
    Check(e.Encompasses(other), "ExpandToEncompass: result encompasses other");
  }

  // -----------------------------------------------------------------------
  // BDS::MaximumCombinedExtent — free function in BDS namespace
  // -----------------------------------------------------------------------
  {
    BDSExtent a(-1, 3, -2, 1, -4, 2);
    BDSExtent b(-3, 1, -1, 4, -2, 5);
    BDSExtent combined = BDS::MaximumCombinedExtent(a, b);
    Check(Near(combined.XNeg(), -3), "MaximumCombinedExtent: XNeg = min(-1,-3)");
    Check(Near(combined.XPos(),  3), "MaximumCombinedExtent: XPos = max( 3, 1)");
    Check(Near(combined.YNeg(), -2), "MaximumCombinedExtent: YNeg = min(-2,-1)");
    Check(Near(combined.YPos(),  4), "MaximumCombinedExtent: YPos = max( 1, 4)");
    Check(Near(combined.ZNeg(), -4), "MaximumCombinedExtent: ZNeg = min(-4,-2)");
    Check(Near(combined.ZPos(),  5), "MaximumCombinedExtent: ZPos = max( 2, 5)");
    // Combined should encompass both inputs
    Check(combined.Encompasses(a), "MaximumCombinedExtent: encompasses first");
    Check(combined.Encompasses(b), "MaximumCombinedExtent: encompasses second");
  }

  std::cout << "\n" << nPassed << " passed, " << nFailed << " failed\n";
  return nFailed > 0 ? 1 : 0;
}
