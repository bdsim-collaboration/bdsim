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
#include "BDSSDFilterLogicalVolume.hh"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"

BDSSDFilterLogicalVolume::BDSSDFilterLogicalVolume(const G4String& name,
						   G4LogicalVolume* referenceLVIn):
  G4VSDFilter(name),
  referenceLV(referenceLVIn)
{;}

BDSSDFilterLogicalVolume::~BDSSDFilterLogicalVolume()
{;}

G4bool BDSSDFilterLogicalVolume::Accept(const G4Step* aStep) const
{
  // get the step in the mass world
  const G4Step* realWorldStep = aStep->GetTrack()->GetStep();

  // get the logical volume
  G4LogicalVolume* stepLV = realWorldStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume();
  
  return stepLV == referenceLV;
}
