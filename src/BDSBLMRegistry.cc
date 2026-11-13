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
#include "BDSBLM.hh"
#include "BDSBLMRegistry.hh"

#include "globals.hh"

#include <vector>

BDSBLMRegistry* BDSBLMRegistry::instance = nullptr;

BDSBLMRegistry* BDSBLMRegistry::Instance()
{
  if (!instance)
    {instance = new BDSBLMRegistry();}
  return instance;
}

BDSBLMRegistry::BDSBLMRegistry():
  numberOfEntries(0)
{;}

BDSBLMRegistry::~BDSBLMRegistry()
{
  for (auto b : blms)
    {delete b;}
  instance = nullptr;
}

G4int BDSBLMRegistry::RegisterBLM(G4String name,
				  BDSBLM*  blm,
				  G4double SIn)
{
  names.push_back(name);
  blms.insert(blm);
  blmsVector.push_back(blm);
  S.push_back(SIn);
  G4int index = numberOfEntries; // copy the number of entires / the index of this entry
  numberOfEntries++;
  return index;
}
