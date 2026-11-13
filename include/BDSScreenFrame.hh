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
#ifndef BDSSCREENFRAME_H
#define BDSSCREENFRAME_H

#include "globals.hh"
#include "G4TwoVector.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"

class G4LogicalVolume;
class G4Material;
class G4VisAttributes;

/**
 * @brief A frame for the vacuum window in e.g. BDSMultilayerScreen.
 *
 * This is a virtual class and the derived class must implement Build()
 * that ultimately sets the member logVol.
 * 
 * @author Lawrence Deacon.
 */

class BDSScreenFrame
{
public:
  BDSScreenFrame(G4String      name,
		 G4ThreeVector size,
		 G4TwoVector   windowSize,
		 G4TwoVector   windowOffset,
		 G4Material*   material);

  virtual ~BDSScreenFrame();

  /// Build method to construct geometry.
  virtual void Build() = 0;

  /// Accessor.
  G4LogicalVolume* LogVol() const {return logVol;}

protected:
  /// Set the visual attributes to member logVol.
  virtual void SetVisAtts();

  /// Construct default visualisation attribute for light transparent grey.
  void SetDefaultVisAtts();

  /// Attach visualisation attributes to logical volume if they exist and if not
  /// use the default.
  void SetVisAtts(G4LogicalVolume* logVolIn,
		  G4VisAttributes* visAttsIn = nullptr);
  
  G4String         name;
  G4ThreeVector    size;
  G4TwoVector      windowSize;
  G4TwoVector      windowOffset;
  G4Material*      material;
  G4LogicalVolume* logVol;
  G4bool           checkOverlaps; ///< Cache of checking overlaps from global constants.
  G4VisAttributes* visAtt;

private:
  BDSScreenFrame() = delete;
};

#endif
