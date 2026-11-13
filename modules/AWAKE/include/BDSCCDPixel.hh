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
#ifndef BDSCCDPIXEL_H
#define BDSCCDPIXEL_H

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"
#include "BDSCCDPixelSD.hh"

class BDSCCDPixel 
{
public:
  BDSCCDPixel(G4ThreeVector size, G4String name);
  ~BDSCCDPixel();
  /// @{ Assignment and copy constructor not implemented nor used
  BDSCCDPixel& operator=(const BDSCCDPixel&) = delete;
  BDSCCDPixel(BDSCCDPixel&) = delete;
  /// @}
  G4LogicalVolume* log();
  G4String name();
  G4ThreeVector size();

private:
  void build();
  void visAtt();

  G4ThreeVector _size;
  G4String _name;
  G4String _logName;
  G4String _solidName;
  // Geometrical objects:
  G4LogicalVolume* _log;
  G4VSolid* _solid;
  G4VisAttributes* _visAtt;
  BDSCCDPixelSD* _sensDet;
};

#endif
