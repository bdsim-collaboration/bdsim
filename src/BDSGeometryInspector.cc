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
#include "BDSGeometryInspector.hh"
#include "BDSGlobalConstants.hh"

#include "globals.hh"
#include "G4AffineTransform.hh"
#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4DisplacedSolid.hh"
#include "G4EllipticalTube.hh"
#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4Version.hh"
#include "G4VSolid.hh"

#include <utility>

// for Geant4.10.2 and below we have a different algorithm as some accessors are not available
#if G4VERSION_NUMBER < 1030
#include "G4VisExtent.hh"
#include <algorithm>
#include <cmath>
#endif

std::pair<BDSExtent, BDSExtent> BDS::DetermineExtents(const G4VSolid* solid)
{
  G4String className = solid->GetEntityType();
  if (className == "G4DisplacedSolid")
    {return BDS::InspectDisplacedSolid(solid);}
  else if (className == "G4SubtractionSolid")
    {return BDS::InspectSubtractionSolid(solid);}
  else if (className == "G4IntersectionSolid")
    {return BDS::InspectIntersectionSolid(solid);}
  else if (className == "G4UnionSolid")
    {return BDS::InspectUnionSolid(solid);}
  else if (className == "G4Box")
    {return BDS::InspectBox(solid);}
  else if (className == "G4Tubs")
    {return BDS::InspectTubs(solid);}
  else if (className == "G4CutTubs")
    {return BDS::InspectCutTubs(solid);}
  else if (className == "G4EllipticalTube")
    {return BDS::InspectEllipticalTube(solid);}
  else
    {
      G4ThreeVector low, high;
      solid->BoundingLimits(low, high);
      // take abs of low and high
      BDSExtent outer = BDSExtent(std::max(std::abs(low.x()), std::abs(high.x())),
                                  std::max(std::abs(low.y()), std::abs(high.y())),
                                  std::max(std::abs(low.z()), std::abs(high.z())));
      BDSExtent inner = BDSExtent();
      return std::make_pair(outer, inner);
    }
}

std::pair<BDSExtent, BDSExtent> BDS::InspectDisplacedSolid(const G4VSolid* solidIn)
{
  const G4DisplacedSolid* solid = dynamic_cast<const G4DisplacedSolid*>(solidIn);
  if (!solid)
    {return std::make_pair(BDSExtent(), BDSExtent());}

  const G4VSolid* constituentSolid = solid->GetConstituentMovedSolid();

  auto ext = BDS::DetermineExtents(constituentSolid);
  G4AffineTransform transform = solid->GetTransform();

  // only going to shift x,y without any rotation.
  G4ThreeVector translation = transform.NetTranslation();
  BDSExtent outer = ext.first.Translate(translation.x(), translation.y(), 0);
  BDSExtent inner = ext.second.Translate(translation.x(), translation.y(), 0);
  
  return std::make_pair(outer, inner);
}
  
std::pair<BDSExtent, BDSExtent> BDS::InspectSubtractionSolid(const G4VSolid* solidIn)
{
  const G4SubtractionSolid* solid = dynamic_cast<const G4SubtractionSolid*>(solidIn);
  if (!solid)
    {return std::make_pair(BDSExtent(), BDSExtent());}

  const G4VSolid* a = solid->GetConstituentSolid(0);
  const G4VSolid* b = solid->GetConstituentSolid(1);

  auto extA = BDS::DetermineExtents(a);
  auto extB = BDS::DetermineExtents(b);

  BDSExtent outer = extA.first.TransverselyLessThan(extB.first) ? extB.first : extA.first;

  // Here we make an ASSUMPTION that the b solid is the interior one - convention.
  // Also it's the outer extent of that solid.
  BDSExtent inner = extB.first;
  
  return std::make_pair(outer, inner);
}

std::pair<BDSExtent, BDSExtent> BDS::InspectIntersectionSolid(const G4VSolid* solidIn)
{
  const G4IntersectionSolid* solid = dynamic_cast<const G4IntersectionSolid*>(solidIn);
  if (!solid)
    {return std::make_pair(BDSExtent(), BDSExtent());}

  const G4VSolid* a = solid->GetConstituentSolid(0);
  const G4VSolid* b = solid->GetConstituentSolid(1);
  
  auto extA = BDS::DetermineExtents(a);
  auto extB = BDS::DetermineExtents(b);

  BDSExtent outer = extA.first.TransverselyLessThan(extB.first) ? extB.first : extA.first;
  BDSExtent inner = extA.first.TransverselyLessThan(extB.first) ? extA.second : extB.second;
  
  return std::make_pair(outer, inner);
}

std::pair<BDSExtent, BDSExtent> BDS::InspectUnionSolid(const G4VSolid* solidIn)
{
  const G4UnionSolid* solid = dynamic_cast<const G4UnionSolid*>(solidIn);
  if (!solid)
    {return std::make_pair(BDSExtent(), BDSExtent());}

  const G4VSolid* a = solid->GetConstituentSolid(0);
  const G4VSolid* b = solid->GetConstituentSolid(1);
  
  auto extA = BDS::DetermineExtents(a);
  auto extB = BDS::DetermineExtents(b);

  BDSExtent outer = extA.first.TransverselyLessThan(extB.first) ? extB.first : extA.first;
  BDSExtent inner = extA.first.TransverselyLessThan(extB.first) ? extA.second : extB.second;
  
  return std::make_pair(outer, inner);
}

std::pair<BDSExtent, BDSExtent> BDS::InspectBox(const G4VSolid* solidIn)
{
  const G4Box* solid = dynamic_cast<const G4Box*>(solidIn);
  if (!solid)
    {return std::make_pair(BDSExtent(), BDSExtent());}

  G4double dx = solid->GetXHalfLength();
  G4double dy = solid->GetYHalfLength();
  G4double dz = solid->GetZHalfLength();

  // accurate along z, but margin in x,y
  G4double lsl = BDSGlobalConstants::Instance()->LengthSafetyLarge();
  BDSExtent outer(dx + lsl, dy + lsl, dz); // symmetric +/-
  BDSExtent inner = BDSExtent();
  return std::make_pair(outer, inner);
}

std::pair<BDSExtent, BDSExtent> BDS::InspectTubs(const G4VSolid* solidIn)
{
  const G4Tubs* solid = dynamic_cast<const G4Tubs*>(solidIn);
  if (!solid)
    {return std::make_pair(BDSExtent(), BDSExtent());}

  G4double innerR = solid->GetInnerRadius();
  G4double outerR = solid->GetOuterRadius();
  G4double zHalfL = solid->GetZHalfLength();

  G4double lsl = BDSGlobalConstants::Instance()->LengthSafetyLarge();
  BDSExtent outer(outerR + lsl, outerR + lsl, zHalfL);
  BDSExtent inner(innerR, innerR, zHalfL);
  return std::make_pair(outer, inner);
}

std::pair<BDSExtent, BDSExtent> BDS::InspectCutTubs(const G4VSolid* solidIn)
{
  G4ThreeVector zminV = G4ThreeVector();
  G4ThreeVector zmaxV = G4ThreeVector();
#if G4VERSION_NUMBER > 1039
  solidIn->BoundingLimits(zminV, zmaxV);
#elif G4VERSION_NUMBER > 1029
  solidIn->Extent(zminV, zmaxV);
#else
  G4VisExtent v = solidIn->GetExtent();
  zmaxV = G4ThreeVector(std::max(std::abs(v.GetXmin()),std::abs(v.GetXmax())),
			std::max(std::abs(v.GetYmin()),std::abs(v.GetYmax())),
			std::max(std::abs(v.GetZmin()),std::abs(v.GetZmax())));			
#endif
  
  const G4CutTubs* solid = dynamic_cast<const G4CutTubs*>(solidIn);
  if (!solid)
    {return std::make_pair(BDSExtent(zmaxV), BDSExtent());}
  
  G4double innerR = solid->GetInnerRadius();
  G4double outerR = solid->GetOuterRadius();
  G4double lsl = BDSGlobalConstants::Instance()->LengthSafetyLarge();
  outerR += lsl;
  
  BDSExtent outer(-outerR, outerR,
		  -outerR, outerR,
		  -zmaxV.z(), zmaxV.z());
  BDSExtent inner(-innerR, innerR,
		  -innerR, innerR,
                  -zmaxV.z(), zmaxV.z());
  return std::make_pair(outer, inner);
}

std::pair<BDSExtent, BDSExtent> BDS::InspectEllipticalTube(const G4VSolid* solidIn)
{
  const G4EllipticalTube* solid = dynamic_cast<const G4EllipticalTube*>(solidIn);
  if (!solid)
    {return std::make_pair(BDSExtent(), BDSExtent());}
  
  G4double dX = solid->GetDx();
  G4double dY = solid->GetDy();
  G4double dZ = solid->GetDz();
  
  G4double lsl = BDSGlobalConstants::Instance()->LengthSafetyLarge();
  BDSExtent outer(dX + lsl, dY + lsl, dZ);
  BDSExtent inner(0, 0, dZ);
  return std::make_pair(outer, inner);
}
