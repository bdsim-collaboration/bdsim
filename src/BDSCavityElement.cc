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
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBeamPipeType.hh"
#include "BDSCavity.hh"
#include "BDSCavityElement.hh"
#include "BDSCavityFactory.hh"
#include "BDSCavityInfo.hh"
#include "BDSExtent.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldInfo.hh"

#include "globals.hh" // geant4 globals / types
#include "G4LogicalVolume.hh"


BDSCavityElement::BDSCavityElement(const G4String&       nameIn,
				   G4double             chordLengthIn,
				   G4Material*          vacuumMaterialIn,
				   const BDSFieldInfo*  vacuumFieldInfoIn,
				   const BDSCavityInfo* cavityInfoIn):
  BDSAcceleratorComponent(nameIn, chordLengthIn, 0,
			  "cavity_"+cavityInfoIn->cavityType.ToString()),
  vacuumFieldInfo(vacuumFieldInfoIn),
  cavityInfo(cavityInfoIn),
  vacuumMaterial(vacuumMaterialIn)
{
  beamPipeInfo = new BDSBeamPipeInfo(BDSBeamPipeType::circular,
				     cavityInfo->irisRadius, 0, 0, 0,
				     vacuumMaterialIn,
				     cavityInfo->thickness,
				     cavityInfo->material);
}

BDSCavityElement::~BDSCavityElement()
{
  delete vacuumFieldInfo;
  delete cavityInfo;
}

void BDSCavityElement::BuildContainerLogicalVolume()
{;}

void BDSCavityElement::Build()
{
  BDSCavity* cavity = BDSCavityFactory::Instance()->CreateCavity(name,
								 chordLength,
								 cavityInfo,
								 vacuumMaterial);
  RegisterDaughter(cavity);

  // make the beam pipe container, this object's container
  containerLogicalVolume = cavity->GetContainerLogicalVolume();
  containerSolid         = cavity->GetContainerSolid();

  // register vacuum volume (for biasing)
  SetAcceleratorVacuumLogicalVolume(cavity->GetVacuumLogicalVolume());

  // update extents
  InheritExtents(cavity);

  // field
  BuildField();

  BuildUserLimits();
  AttachUserLimits();
}

void BDSCavityElement::BuildField()
{
  BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumFieldInfo,
                                                            GetAcceleratorVacuumLogicalVolumes(),
                                                            true);
}
