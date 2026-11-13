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
#ifndef BDS_ROUND_TUNNEL_HH
#define BDS_ROUND_TUNNEL_HH
#include <stdexcept>
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "BDSTunnel.hh"
#include "../parser/element.h"

class BDSRoundTunnel:public BDSTunnel{
public:
  BDSRoundTunnel(Element val, G4double length, G4double angle);
  ~BDSRoundTunnel();



protected:
  virtual void CalculateDimensions();

private:
  BDSRoundTunnel();
  
  virtual void BuildSolidVolumes();
  virtual void BuildStraightSolids();
  virtual void BuildAngleSolids();
  virtual void SetVisAttributes();

  G4VSolid* _tunnelSizedBlock;
  G4LogicalVolume* _floorLogicalVolume;
  G4VSolid* _largerInnerSolid; 
  G4VSolid * _largerCavity;
  G4ThreeVector _floorOffset;
  G4double _blockSize;
  
  
};



#endif
