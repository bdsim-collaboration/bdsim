/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSGeometryComponent.hh"
#include "BDSSimpleComponent.hh"

#include "globals.hh" // geant4 types / globals

BDSSimpleComponent::BDSSimpleComponent(const G4String&       nameIn,
                                       BDSGeometryComponent* component,
                                       G4double              arcLengthIn,
                                       G4double              angleIn,
                                       const G4ThreeVector&  inputFaceNormalIn,
                                       const G4ThreeVector&  outputFaceNormalIn,
                                       BDSBeamPipeInfo*      beamPipeInfoIn,
                                       const G4String&       typeIn):
  BDSAcceleratorComponent(nameIn, arcLengthIn, angleIn, typeIn, beamPipeInfoIn,
                          inputFaceNormalIn, outputFaceNormalIn)
{
  RegisterDaughter(component);
  InheritExtents(component);
  containerSolid = component->GetContainerSolid();
  containerLogicalVolume = component->GetContainerLogicalVolume();
  containerAssembly = component->GetContainerAssemblyVolume();
  containerIsAssembly = component->ContainerIsAssembly();
}

BDSSimpleComponent::BDSSimpleComponent(const G4String&       nameIn,
                                       G4double              arcLengthIn,
                                       G4double              angleIn,
                                       G4VSolid*             containerSolidIn,
                                       G4LogicalVolume*      containerLogicalVolumeIn,
                                       const BDSExtent&      extentIn,
                                       const G4ThreeVector&  inputFaceNormalIn,
                                       const G4ThreeVector&  outputFaceNormalIn,
                                       BDSBeamPipeInfo*      beamPipeInfoIn,
                                       const G4String&       typeIn):
  BDSAcceleratorComponent(nameIn, arcLengthIn, angleIn, typeIn, beamPipeInfoIn,
                          inputFaceNormalIn, outputFaceNormalIn)
{
  SetExtent(extentIn);
  containerSolid         = containerSolidIn;
  containerLogicalVolume = containerLogicalVolumeIn;
}

void BDSSimpleComponent::Build()
{
  BuildContainerLogicalVolume();
  // in each case this is used, user limits are attached already,
  // so it is not needed to overwrite them here.
}
