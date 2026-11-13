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
#include "BDSBeamPipe.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSSDManager.hh"
#include "BDSSampler.hh"
#include "BDSSDSampler.hh"

#include "G4String.hh"
#include "G4LogicalVolume.hh"

BDSSampler::BDSSampler(const G4String& nameIn,
                       G4int filterSetIDIn):
  BDSGeometryComponent(nullptr, nullptr),
  name(nameIn),
  filterSetID(filterSetIDIn)
{;}

void BDSSampler::CommonConstruction()
{
  containerLogicalVolume = new G4LogicalVolume(containerSolid, nullptr, GetName() + "_lv");
  containerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->VisibleDebugVisAttr());
  SetSensitivity();
}

void BDSSampler::SetSensitivity()
{
  auto sdMan = BDSSDManager::Instance();
  BDSSDSampler* sd = filterSetID > -1 ? sdMan->SamplerPlaneWithFilter(filterSetID) : sdMan->SamplerPlane();
  containerLogicalVolume->SetSensitiveDetector(sd);
}

void BDSSampler::MakeMaterialValidForUseInMassWorld()
{
  if (containerLogicalVolume)
    {containerLogicalVolume->SetMaterial(BDSMaterials::Instance()->GetMaterial("G4_Galactic"));}
}