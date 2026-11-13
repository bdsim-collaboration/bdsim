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
#include "BDSAperture.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSLaserWire.hh"
#include "BDSMaterials.hh"
#include "BDSSDType.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"

BDSLaserWire::BDSLaserWire(const G4String&      nameIn,
                           G4double             lengthIn,
                           G4double             wavelengthIn,
                           const G4ThreeVector& directionIn,
                           BDSBeamPipeInfo*     beamPipeInfoIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "laserwire", beamPipeInfoIn),
  itsLaserDirection(directionIn),
  itsLaserWavelength(wavelengthIn)
{;}

BDSLaserWire::~BDSLaserWire()
{;}

void BDSLaserWire::BuildContainerLogicalVolume()
{
  G4double beamPipeRadius = beamPipeInfo->IndicativeRadius();
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
