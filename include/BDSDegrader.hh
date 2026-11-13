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
#ifndef BDSDEGRADER_H
#define BDSDEGRADER_H

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

class G4Colour;
class G4Material;

/**
 * @brief Degrader based on wedge design used in the PSI medical accelerator.
 * 
 * @author Will Shields
 */

class BDSDegrader: public BDSAcceleratorComponent
{
public:
  BDSDegrader(const G4String& nameIn, 
	      G4double lengthIn,
	      G4double horizontalWidthIn,
	      G4int    numberWedgesIn,
	      G4double wedgeLengthIn,
	      G4double degraderHeightIn,
	      G4double degraderOffsetIn,
	      G4double baseWidthIn,
	      G4Material* degraderMaterialIn,
	      G4Material* vacuumMaterialIn,
	      G4Colour*   colourIn);
  virtual ~BDSDegrader();
  
protected:
  virtual void Build();
  
  virtual void BuildContainerLogicalVolume();
  
  G4double    horizontalWidth;
  G4int       numberWedges;
  G4double    wedgeLength;
  G4double    degraderHeight;
  G4double    degraderOffset;
  G4double    baseWidth;
  G4Material* degraderMaterial;
  G4Material* vacuumMaterial;
  G4Colour*   colour;
  
  inline G4bool IsOdd(G4int integer) const {return integer % 2 != 0;}
  inline G4bool IsEven(G4int integer)const {return integer % 2 == 0;}
  
private:
  /// Private default constructor to force the use of the supplied one.
  BDSDegrader() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSDegrader& operator=(const BDSDegrader&) = delete;
  BDSDegrader(BDSDegrader&) = delete;
  ///@}

  /// Register physical placement of the degrader wedges. zOffset is the wedge offset from the start
  /// of the element, i.e from -l/2.
  void PlaceWedge(G4bool            placeRight,
                  G4double          zOffset,
                  const G4String&   placementName,
                  G4LogicalVolume*  lv,
                  G4RotationMatrix* rotation);
};

#endif
