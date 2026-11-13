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
#include "BDSExtentGlobal.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4TwoVector.hh"

#include <algorithm>
#include <ostream>
#include <utility>
#include <vector>

#include "CLHEP/Geometry/Point3D.h"


BDSExtentGlobal::BDSExtentGlobal():
  transform(G4Transform3D()),
  extXNegG(0.0),
  extXPosG(0.0),
  extYNegG(0.0),
  extYPosG(0.0),
  extZNegG(0.0),
  extZPosG(0.0)
{;}

BDSExtentGlobal::BDSExtentGlobal(const BDSExtent&     extentIn,
				 const G4Transform3D& transformIn):
  BDSExtent(extentIn),
  transform(G4Transform3D(transformIn))
{
  const auto bps = extentIn.AllBoundaryPoints(); // local frame

  // transform them to new frame
  std::vector<G4ThreeVector> tBPS; // transformed boundary points
  for (const auto& point : bps)
    {tBPS.emplace_back(transform * (HepGeom::Point3D<G4double>)point);}

  // must initialise to a point that definitely lies inside the extent
  // before iterating to find extents - use one point.
  extXNegG = tBPS[0].x();
  extXPosG = tBPS[0].x();
  extYNegG = tBPS[0].y();
  extYPosG = tBPS[0].y();
  extZNegG = tBPS[0].z();
  extZPosG = tBPS[0].z();

  // calculate new extents
  for (const auto& point : tBPS)
    {
      extXNegG = std::min(extXNegG, point.x());
      extXPosG = std::max(extXPosG, point.x());
      extYNegG = std::min(extYNegG, point.y());
      extYPosG = std::max(extYPosG, point.y());
      extZNegG = std::min(extZNegG, point.z());
      extZPosG = std::max(extZPosG, point.z());
    }

  if (extXNegG > extXPosG)
    {std::swap(extXNegG, extXPosG);}
  if (extYNegG > extYPosG)
    {std::swap(extYNegG, extYPosG);}
  if (extZNegG > extZPosG)
    {std::swap(extZNegG, extZPosG);}
}

BDSExtentGlobal::~BDSExtentGlobal()
{;}

std::vector<G4ThreeVector> BDSExtentGlobal::AllBoundaryPointsGlobal() const
{
  std::vector<G4ThreeVector> result;
  result.emplace_back(extXNegG, extYNegG, extZNegG);
  result.emplace_back(extXPosG, extYNegG, extZNegG);
  result.emplace_back(extXPosG, extYPosG, extZNegG);
  result.emplace_back(extXNegG, extYPosG, extZNegG);
  result.emplace_back(extXNegG, extYNegG, extZPosG);
  result.emplace_back(extXPosG, extYNegG, extZPosG);
  result.emplace_back(extXPosG, extYPosG, extZPosG);
  result.emplace_back(extXNegG, extYPosG, extZPosG);
  return result;
}

G4bool BDSExtentGlobal::Overlaps(const BDSExtentGlobal& /*other*/) const
{
  return false;
}

BDSExtentGlobal BDSExtentGlobal::TranslateGlobal(G4double dx, G4double dy, G4double dz) const
{
  BDSExtentGlobal cp = BDSExtentGlobal(*this);
  cp.extXNegG += dx;
  cp.extXPosG += dx;
  cp.extYNegG += dy;
  cp.extYPosG += dy;
  cp.extZNegG += dz;
  cp.extZPosG += dz;

  return cp;
}

BDSExtentGlobal BDSExtentGlobal::ExpandToEncompass(const BDSExtentGlobal& other) const
{
  BDSExtentGlobal result = BDSExtentGlobal(*this);
  result.extXNegG = std::min(extXNegG, other.extXNegG);
  result.extXPosG = std::max(extXPosG, other.extXPosG);
  result.extYNegG = std::min(extYNegG, other.extYNegG);
  result.extYPosG = std::max(extYPosG, other.extYPosG);
  result.extZNegG = std::min(extZNegG, other.extZNegG);
  result.extZPosG = std::max(extZPosG, other.extZPosG);
  return result;
}

std::ostream& operator<< (std::ostream& out, BDSExtentGlobal const& ext)
{
  out << static_cast<const BDSExtent&>(ext);
  out << G4endl;
  out << "Transform: " << ext.transform.getRotation()
      << ext.transform.getTranslation() << G4endl;
  out << "In Global frame: " << G4endl;
  out << ext.extXNegG << " " << ext.extXPosG << " ";
  out << ext.extYNegG << " " << ext.extYPosG << " ";
  out << ext.extZNegG << " " << ext.extZPosG << " mm";
  return out;
}

G4double BDSExtentGlobal::MaximumAbsGlobal() const
{
  std::vector<G4double> exts = {std::abs(extXNegG), extXPosG,
				std::abs(extYNegG), extYPosG,
				std::abs(extZNegG), extZPosG};
  return *std::max_element(exts.begin(), exts.end());
}

G4bool BDSExtentGlobal::EncompassesGlobal(G4double x,
					  G4double y,
					  G4double z) const
{
  G4ThreeVector point = G4ThreeVector(x,y,z);
  
  auto p = AllBoundaryPointsGlobal();
  
  // construct vectors for each side
  G4ThreeVector edge01 = p[1] - p[0];
  G4ThreeVector edge03 = p[3] - p[0];
  G4ThreeVector edge04 = p[4] - p[0];

  // axes vectors
  G4ThreeVector axX = edge04.cross(edge03);  
  G4ThreeVector axY = edge04.cross(edge01);
  G4ThreeVector axZ = edge01.cross(edge03);

  // projection of the point onto an axis of the rectangle must lie
  // inside the projection of the projection of the two (origin,vertex)
  // projections onto the same axis for the point to be inside an arbitrary
  // 3d cuboid.
  G4double pxp = axX.dot(point);
  G4double pyp = axY.dot(point);
  G4double pzp = axZ.dot(point);
  G4bool insideX = (pxp > axX.dot(p[0])) && (pxp < axX.dot(p[1]));
  G4bool insideY = (pyp > axY.dot(p[0])) && (pyp < axY.dot(p[3]));
  G4bool insideZ = (pzp > axZ.dot(p[0])) && (pzp < axZ.dot(p[4]));

  return insideX && insideY && insideZ;  
}

G4bool BDSExtentGlobal::Encompasses(const BDSExtentGlobal& otherExtent)
{
  G4bool insideX = (std::abs(otherExtent.ExtentXGlobal().first) < std::abs(extXNegG)) &&
          (otherExtent.ExtentXGlobal().second < extXPosG);
  G4bool insideY = (std::abs(otherExtent.ExtentYGlobal().first) < std::abs(extYNegG)) &&
          (otherExtent.ExtentYGlobal().second < extYPosG);
  G4bool insideZ = (std::abs(otherExtent.ExtentZGlobal().first) < std::abs(extZNegG)) &&
          (otherExtent.ExtentZGlobal().second < extZPosG);
  return insideX && insideY && insideZ;
}

G4bool BDSExtentGlobal::Encompasses(const std::vector<BDSExtentGlobal>& otherExtents)
{
  G4bool encompassesAllBeamlines = true;

  // loop over all extents from all beam lines
  for (const auto& ext : otherExtents)
    {
      if (!Encompasses(ext))
        {encompassesAllBeamlines = false;}
    }
  return encompassesAllBeamlines;
}

G4ThreeVector BDSExtentGlobal::GetMaximumExtentAbsolute() const
{
  G4ThreeVector mEA;
  for (int i = 0; i < 3; i++)
    {mEA[i] = std::max(std::abs(ExtentPositiveGlobal()[i]), std::abs(ExtentNegativeGlobal()[i]));}
  return mEA;
}
