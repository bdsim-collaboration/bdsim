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
#include "BDSBeamPipeInfo.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSLaserWire.hh"
#include "BDSMaterials.hh"
#include "BDSSDType.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"

BDSLaserWire::BDSLaserWire(G4String      nameIn,
			   G4double      lengthIn,
			   G4double      wavelengthIn,
			   G4ThreeVector directionIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "laserwire"),
  itsLaserDirection(directionIn),
  itsLaserWavelength(wavelengthIn)
{;}

BDSLaserWire::~BDSLaserWire()
{;}

void BDSLaserWire::BuildContainerLogicalVolume()
{
  G4double beamPipeRadius = BDSGlobalConstants::Instance()->DefaultBeamPipeModel()->aper1;
  containerSolid = new G4Box(name +"_container_solid",
			     beamPipeRadius,
			     beamPipeRadius,
			     chordLength*0.5);
  containerLogicalVolume=new G4LogicalVolume(containerSolid,
					     BDSMaterials::Instance()->GetMaterial("LaserVac"),
					     name + "_container_lv");
  if (sensitiveVacuum)
    {RegisterSensitiveVolume(containerLogicalVolume, BDSSDType::energydepvacuum);}
  BDSGlobalConstants::Instance()->SetLaserwireWavelength(containerLogicalVolume->GetName(),itsLaserWavelength);
  BDSGlobalConstants::Instance()->SetLaserwireDir(containerLogicalVolume->GetName(),itsLaserDirection);

  SetAcceleratorVacuumLogicalVolume(containerLogicalVolume);
}
