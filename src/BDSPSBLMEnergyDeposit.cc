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
#include "BDSPSBLMEnergyDeposit.hh"

#include "G4StepPoint.hh"
#include "G4TouchableHistory.hh"

BDSPSBLMEnergyDeposit::BDSPSBLMEnergyDeposit(const G4String& name,
					     G4int           depth):
  G4PSEnergyDeposit(name, depth)
{;}

BDSPSBLMEnergyDeposit::BDSPSBLMEnergyDeposit(const G4String& name,
					     const G4String& unit,
                                             G4int           depth):
  G4PSEnergyDeposit(name, unit, depth)
{;}

BDSPSBLMEnergyDeposit::~BDSPSBLMEnergyDeposit()
{;}

G4int BDSPSBLMEnergyDeposit::GetIndex(G4Step* aStep)
{
  G4StepPoint* preStep = aStep->GetPreStepPoint();
  G4TouchableHistory* th = (G4TouchableHistory*)(preStep->GetTouchable());
  /// Get the copy number from the outermost volume in the geometry
  /// Works for both single-solid BLMs and complex GDML models
  return th->GetReplicaNumber(th->GetHistoryDepth() - 1);
}



