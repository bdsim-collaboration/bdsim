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
#ifndef BDSSCREENFRAMERECTANGULAR_H
#define BDSSCREENFRAMERECTANGULAR_H

#include "BDSScreenFrame.hh"

#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"

class G4LogicalVolume;
class G4Material;

/**
 * @brief Rectangular screen frame.
 *
 * Rectangular hollow frame / tube. Length is determined by size.z().
 *
 * @author Lawrence Deacon
 */

class BDSScreenFrameRectangular: public BDSScreenFrame
{
public:
  BDSScreenFrameRectangular(G4String      name,
			    G4ThreeVector size,///< Full width in each dimension of inner surface.
			    G4TwoVector   windowSize,   ///< Full width of h,v of outer surface.
			    G4TwoVector   windowOffset, ///< Offset of intertior to exterior.
			    G4Material*   material);
  
  virtual ~BDSScreenFrameRectangular();
  /// @{ Assignment and copy constructor not implemented nor used
  BDSScreenFrameRectangular& operator=(const BDSScreenFrameRectangular&) = delete;
  BDSScreenFrameRectangular(BDSScreenFrameRectangular&) = delete;
  /// @}

  /// Construct the geometry.
  virtual void Build();

  /// Place member logVol (from base class) into argument motherVol.
  void Place(G4RotationMatrix* rot,
	     G4ThreeVector     pos,
	     G4LogicalVolume*  motherVol);


protected:
  virtual void SetVisAtts();
  
private:
  /// Remove default constructor to force use of supplied one.
  BDSScreenFrameRectangular() = delete;
  G4ThreeVector     cavityPos;
  G4RotationMatrix* zeroRot;
};

#endif
