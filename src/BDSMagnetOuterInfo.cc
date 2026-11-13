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
#include "BDSColours.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSMagnetOuterInfo.hh"

#include "globals.hh"
#include "G4Colour.hh"
#include "G4Material.hh"

BDSMagnetOuterInfo::BDSMagnetOuterInfo():
  name("not_specified"),
  geometryType(BDSMagnetGeometryType::cylindrical),
  horizontalWidth(0),
  outerMaterial(nullptr),
  innerRadius(1.0),
  vhRatio(1.0),
  angleIn(0),
  angleOut(0),
  yokeOnLeft(false),
  hStyle(false),
  buildEndPieces(true),
  coilWidthFraction(0.65),
  coilHeightFraction(0.8),
  geometryTypeAndPath(""),
  autoColour(true)
{
  colour = BDSColours::Instance()->GetColour("default");
}

BDSMagnetOuterInfo::BDSMagnetOuterInfo(const G4String&         nameIn,
				       BDSMagnetGeometryType geometryTypeIn,
				       G4double              horizontalWidthIn,
				       G4Material*           outerMaterialIn,
				       G4double              innerRadiusIn,
				       G4double              vhRatioIn,
				       G4double              angleInIn,
				       G4double              angleOutIn,
				       G4bool                yokeOnLeftIn,
				       G4bool                hStyleIn,
				       G4bool                buildEndPiecesIn,
				       G4double              coilWidthFractionIn,
				       G4double              coilHeightFractionIn,
				       const G4String&       geometryTypeAndPathIn,
				       G4Colour*             colourIn,
				       G4bool                autoColourIn):
  name(nameIn),
  geometryType(geometryTypeIn),
  horizontalWidth(horizontalWidthIn),
  outerMaterial(outerMaterialIn),
  innerRadius(innerRadiusIn),
  vhRatio(vhRatioIn),
  angleIn(angleInIn),
  angleOut(angleOutIn),
  yokeOnLeft(yokeOnLeftIn),
  hStyle(hStyleIn),
  buildEndPieces(buildEndPiecesIn),
  coilWidthFraction(coilWidthFractionIn),
  coilHeightFraction(coilHeightFractionIn),
  geometryTypeAndPath(geometryTypeAndPathIn),
  colour(colourIn),
  autoColour(autoColourIn)
{;}
