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
#ifdef USE_GDML

#ifndef BDSGEOMETRYGDML_H
#define BDSGEOMETRYGDML_H

#include "BDSGeometry.hh"

#include "globals.hh" // geant4 types / globals

#include <vector>

class G4LogicalVolume;

class BDSGeometryGDML: public BDSGeometry
{
public:
  BDSGeometryGDML(G4String GMDLfile);
  virtual ~BDSGeometryGDML();
  void Construct(G4LogicalVolume *marker);

  /// Access all logical volumes belonging to this component
  std::vector<G4LogicalVolume*> GetAllLogicalVolumes() const;  
  /// Access all sensitive volumes belonging to this component
  std::vector<G4LogicalVolume*> GetAllSensitiveVolumes() const;

private:
  G4String                      GDMLfile;
  G4LogicalVolume              *markerVol;
  G4LogicalVolume              *gdmlWorld;
  std::vector<G4LogicalVolume*> logicalVols; 
  std::vector<G4LogicalVolume*> sensitiveVols;
};
#endif

#endif
