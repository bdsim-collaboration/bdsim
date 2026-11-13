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
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include <ostream>

BDSTiltOffset::BDSTiltOffset():
  dx(0.0), dy(0.0), tilt(0.0)
{;}

BDSTiltOffset::BDSTiltOffset(G4double xOffset,
			     G4double yOffset,
			     G4double tiltIn):
  dx(xOffset), dy(yOffset), tilt(tiltIn)
{;}

BDSTiltOffset::BDSTiltOffset(const BDSTiltOffset& other):
  dx(other.dx),
  dy(other.dy),
  tilt(other.tilt)
{;}

BDSTiltOffset& BDSTiltOffset::operator=(const BDSTiltOffset& other) noexcept
{
  if (this != &other)
    {
      dx = other.dx;
      dy = other.dy;
      tilt = other.tilt;
    }
  return *this;
}

std::ostream& operator<< (std::ostream& out, BDSTiltOffset const &to)
{
  out << to.tilt << " " << to.dx << " " << to.dy;
  return out;
}

G4Transform3D BDSTiltOffset::Transform3D() const
{
  G4ThreeVector off = GetOffset();
  G4RotationMatrix rm = G4RotationMatrix();
  rm.rotateZ(tilt);
  return G4Transform3D(rm, off);
}

G4bool BDSTiltOffset::HasFiniteOffset() const
{
  return BDS::IsFinite(dx) || BDS::IsFinite(dy);
}

G4bool BDSTiltOffset::HasFiniteTilt() const
{
  return BDS::IsFinite(tilt);
}

bool operator== (const BDSTiltOffset& lhs, const BDSTiltOffset& rhs)
{
  // remember, double == double is wrong
  bool sameTilt   = BDS::DoublesAreEqual(lhs.tilt, rhs.tilt);
  bool sameOffset = BDS::DoublesAreEqual(lhs.dx, rhs.dx) && BDS::DoublesAreEqual(lhs.dy, rhs.dy);
  return sameTilt && sameOffset;
}
