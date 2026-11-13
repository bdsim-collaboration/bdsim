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
#ifndef BDSGeometryDriver_h
#define BDSGeometryDriver_h 1

#include "globals.hh"
#include "BDSMaterials.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4UserLimits.hh"
#include "BDSMySQLTable.hh"
#include "G4VPhysicalVolume.hh"
//#include "BDSSamplerSD.hh"
#include <fstream>
#include <vector>
//#include "BDSMagFieldSQL.hh"
#include "G4Region.hh"

class BDSGeometryDriver
{
public:
  BDSGeometryDriver(G4String geomFile, G4LogicalVolume* markerVol, G4double markerlength=0);
  ~BDSGeometryDriver();

  virtual void Construct();

  std::vector<G4LogicalVolume*> GetSensitiveComponents() const;
  std::vector<G4VPhysicalVolume*> GetMultiplePhysicalVolumes() const;
  std::vector<G4LogicalVolume*> GetGFlashComponents() const;

  G4VPhysicalVolume* align_in_volume() const;
  G4VPhysicalVolume* align_out_volume() const;
  
protected:
  G4LogicalVolume* itsMarkerVol;
  G4double itsMarkerLength;
  G4String itsGeomFile;
  G4VPhysicalVolume* _align_in_volume;
  G4VPhysicalVolume* _align_out_volume;
  std::vector<G4LogicalVolume*> itsSensitiveComponents;
  std::vector<G4LogicalVolume*> itsGFlashComponents;
  std::vector<G4VPhysicalVolume*> itsMultiplePhysicalVolumes;
  
private:
  BDSGeometryDriver();

};

inline  std::vector<G4LogicalVolume*> BDSGeometryDriver::GetSensitiveComponents() const
{return itsSensitiveComponents;}

inline  std::vector<G4VPhysicalVolume*> BDSGeometryDriver::GetMultiplePhysicalVolumes() const
{return itsMultiplePhysicalVolumes;}

inline  std::vector<G4LogicalVolume*> BDSGeometryDriver::GetGFlashComponents() const
{return itsGFlashComponents;}

inline G4VPhysicalVolume* BDSGeometryDriver::align_in_volume() const
{return _align_in_volume;}

inline G4VPhysicalVolume* BDSGeometryDriver::align_out_volume() const
{return _align_out_volume;}

#endif
