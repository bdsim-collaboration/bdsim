/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#ifndef BDSMAGNETOUTERINFO_H
#define BDSMAGNETOUTERINFO_H
#include "BDSMagnetGeometryType.hh"

#include "globals.hh"

#include <cmath>

class G4Colour;
class G4Material;

/**
 * @brief Holder struct of all information required to
 * create the outer geometry of a magnet. 
 * 
 * This reduces the number of argument to all magnet constructors 
 * plus aggregates common tasks in the component factory.
 * 
 * @author Laurie Nevay
 */

class BDSMagnetOuterInfo
{
public:
  /// default constructor
  BDSMagnetOuterInfo();

  /// extra constructor to assign all members at once
  BDSMagnetOuterInfo(const G4String&       nameIn,
		     BDSMagnetGeometryType geometryTypeIn,
		     G4double              horizontalWidthIn,
		     G4Material*           outerMaterialIn,
		     G4double              innerRadiusIn,
		     G4double              vhRatioIn             = 1,
		     G4double              angleInIn             = 0,
		     G4double              angleOutIn            = 0,
		     G4bool                yokeOnLeft            = false,
		     G4bool                hStyle                = false,
		     G4bool                buildEndPiecesIn      = true,
		     G4double              coilWidthFractionIn   = 0.65,
		     G4double              coilHeightFractionIn  = 0.8,
		     const G4String&       geometryTypeAndPathIn = "",
		     G4Colour*             colourIn              = nullptr,
		     G4bool                autoColour            = true);

  G4String              name;
  BDSMagnetGeometryType geometryType;
  G4double              horizontalWidth;
  G4Material*           outerMaterial;
  G4double              innerRadius;
  G4double              vhRatio;
  G4double              angleIn;
  G4double              angleOut;
  G4bool                yokeOnLeft;
  G4bool                hStyle;       ///< H Style for dipoles. If not, it's assumed C style.
  G4bool                buildEndPieces;
  G4double              coilWidthFraction;
  G4double              coilHeightFraction;
  G4String              geometryTypeAndPath;
  G4Colour*             colour;
  G4bool                autoColour;
  G4int                 detectSchema;

  inline G4double MinimumIntersectionRadiusRequired() const {return std::hypot(0.5*horizontalWidth, 0.5*horizontalWidth*vhRatio);}
};

#endif
