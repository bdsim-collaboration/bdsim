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
#ifndef BDSCCDCAMERA_H
#define BDSCCDCAMERA_H 

#include "globals.hh"
#include "G4LogicalVolume.hh"

#include "G4VisAttributes.hh"
#include "G4VSolid.hh"
#include "G4PVPlacement.hh"
#include "BDSCCDChip.hh"
#include "BDSLens.hh"

class BDSCCDCamera 
{
public:
  BDSCCDCamera();
  ~BDSCCDCamera();
  inline G4LogicalVolume* log(){return _log;}
  inline G4String name(){return _name;}
  inline G4ThreeVector size(){return _size;}
  inline void phys(G4PVPlacement* physIn){_phys=physIn;}
  inline G4PVPlacement* phys(){return _phys;}

private:
  void defaultDimensions();
  void build();
  void buildCavity();
  void buildMotherVolume();
  void buildCCDChip();
  void buildObjectLens();
  void buildImageLens();
  void visAtt();
  void placeComponents();
  void placeCavity();
  void placeCCDChip();
  void placeObjectLens();
  void placeImageLens();

  G4ThreeVector _size;
  G4String _name;
  G4String _logName;
  G4String _solidName;
  // Geometrical objects:
  G4LogicalVolume* _log;
  G4VSolid* _solid;
  BDSCCDChip* _ccdChip;
  BDSLens* _objectLens;
  BDSLens* _imageLens;
  G4PVPlacement* _phys = nullptr;
  G4double _cavityLength;
  G4LogicalVolume* _cavityLog;
};

#endif
