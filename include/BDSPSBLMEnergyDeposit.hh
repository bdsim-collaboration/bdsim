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
#ifndef BDSPSBLMENERGYDEPOSIT_H
#define BDSPSBLMENERGYDEPOSIT_H
#include "G4PSEnergyDeposit.hh"
#include "G4String.hh"
#include "G4Types.hh"

class G4Step;

/**
 * @brief Primitive scorer for energy deposition in BLMs. 
 * 
 * The class inherits BDSPSBLMEnergyDeposition and ensures that the
 * copy number for the volume is obtained from the top-most BLM geometry 
 * volume. This enables hit mapping even for complex BLM geometries such as GDML models.
 *
 * @author Andrey Abramov
 */

class BDSPSBLMEnergyDeposit: public G4PSEnergyDeposit
{
 public:
  BDSPSBLMEnergyDeposit(const G4String& name,
			G4int           depth = 0);
  BDSPSBLMEnergyDeposit(const G4String& name,
			const G4String& unit,
			G4int           depth = 0);
  
  virtual ~BDSPSBLMEnergyDeposit();
  
  G4int GetIndex(G4Step* aStep) override;
};
#endif
