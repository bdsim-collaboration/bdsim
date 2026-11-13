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
#ifndef BDSCCDCHIP_H
#define BDSCCDCHIP_H 

#include "globals.hh"
#include "BDSCCDPixel.hh"
#include "G4LogicalVolume.hh"

#include "G4VSolid.hh"
#include <vector>
#include "G4TwoVector.hh"
#include "G4ThreeVector.hh"

class BDSCCDChip
{
public:
  BDSCCDChip(G4String aName, G4ThreeVector pixelSize, G4TwoVector nPixels);
  ~BDSCCDChip();
  /// @{ Assignment and copy constructor not implemented nor used
  BDSCCDChip& operator=(const BDSCCDChip&) = delete;
  BDSCCDChip(BDSCCDChip&) = delete;
  /// @}

  inline G4LogicalVolume* log(){return _log;}
  inline G4String name(){return _name;}
  inline G4ThreeVector size(){return _size;}

protected:

private:
  void computeDimensions();
  void build();
  void buildMotherVolume();
  void buildPixels();
  
  // Geometrical objects:
  //  G4VPhysicalVolume* _phys;
  G4LogicalVolume* _log;
  G4VSolid* _solid;
  BDSCCDPixel* _pixel;
  
private:
  const G4TwoVector _nPixels;
  G4ThreeVector _size;
  G4String _name;
};

#endif
