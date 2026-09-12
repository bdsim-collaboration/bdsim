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
#include "BDSDrift.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Material.hh"

BDSDrift::BDSDrift(G4String         nameIn, 
		   G4double         lengthIn,
		   BDSBeamPipeInfo* beamPipeInfoIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "drift", beamPipeInfoIn)
{;}

BDSDrift::~BDSDrift()
{;}

void BDSDrift::Build()
{
  BDSBeamPipeFactory* factory = BDSBeamPipeFactory::Instance();
  BDSBeamPipe* pipe = factory->CreateBeamPipe(name,
					      chordLength,
					      beamPipeInfo);

  RegisterDaughter(pipe);
  
  // make the beam pipe container, this object's container
  containerLogicalVolume = pipe->GetContainerLogicalVolume();
  containerSolid         = pipe->GetContainerSolid();

  // register vacuum volume (for biasing)
  SetAcceleratorVacuumLogicalVolume(pipe->GetVacuumLogicalVolume());

  // update extents
  InheritExtents(pipe);

  // update faces
  SetInputFaceNormal(pipe->InputFaceNormal());
  SetOutputFaceNormal(pipe->OutputFaceNormal());
}

G4String BDSDrift::Material() const
{
  auto bpMat = GetBeamPipeInfo()->beamPipeMaterial;
  if (bpMat)
    {return bpMat->GetName();}
  else
    {return BDSAcceleratorComponent::Material();} // none
}
