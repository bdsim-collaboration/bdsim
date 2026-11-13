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
#include "BDSWarning.hh"

#include "globals.hh"

#include <chrono>
#include <thread>

void BDS::Warning(const G4String& message)
{
  G4cout << G4endl << G4endl << "WARNING: " << message << G4endl << G4endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));
}

void BDS::Warning(const G4String& methodName,
		  const G4String& message)
{
  G4cout << G4endl << G4endl << "WARNING: " << methodName << " " << message << G4endl << G4endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));
}
