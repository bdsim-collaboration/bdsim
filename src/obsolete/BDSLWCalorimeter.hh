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
#ifndef BDSLWCALORIMETER_H
#define BDSLWCALORIMETER_H

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

#include <vector>

class BDSBeamPipe;
class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;

class BDSLWCalorimeter: public BDSAcceleratorComponent
{
public:
  BDSLWCalorimeter(G4String         name,
		   G4double         length,
		   BDSBeamPipeInfo* beamPipeInfo,
		   G4int            precisionRegion = 0);
  virtual ~BDSLWCalorimeter();

  /// Access all sensitive volumes belonging to this component including
  /// those belonging to the beam pipe
  virtual std::vector<G4LogicalVolume*> GetAllSensitiveVolumes() const;
  
protected:
  virtual void Build();

  G4LogicalVolume* lwCalLogicalVolume;

private:
  virtual void SetVisAttributes();
  virtual void BuildMarkerLogicalVolume();
  void BuildBeampipe();
  void BuildCal(G4double aLength);

  G4Box*  lwCal;
  G4VPhysicalVolume* physiLWCal;

  BDSBeamPipe* beampipe;
};

#endif
