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
#include "BDSSDFilterMaterial.hh"

#include "G4LogicalVolume.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4String.hh"
#include "G4Track.hh"
#include "G4Types.hh"
#include "G4VPhysicalVolume.hh"

#include <algorithm>
#include <vector>

BDSSDFilterMaterial::BDSSDFilterMaterial(const G4String& name,
                                         const std::vector<G4Material*>& materialsIn,
					 G4bool inclusiveIn):
  G4VSDFilter(name),
  materials(materialsIn),
  inclusive(inclusiveIn)
{;}

BDSSDFilterMaterial::~BDSSDFilterMaterial()
{;}

G4bool BDSSDFilterMaterial::Accept(const G4Step* aStep) const
{
  // get the step in the mass world
  const G4Step* realWorldStep = aStep->GetTrack()->GetStep();
  
  // get the material of the logical volume
  G4LogicalVolume* stepLV = realWorldStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume();
  G4Material* stepMaterial = stepLV->GetMaterial();

  G4bool found = std::find(materials.begin(), materials.end(), stepMaterial) != materials.end();
  return inclusive ? found : !found;
}
