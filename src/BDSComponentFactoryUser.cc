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
#include "BDSComponentConstructor.hh"
#include "BDSComponentFactoryUser.hh"

#include "G4Types.hh"

#include <map>

BDSComponentFactoryUser::BDSComponentFactoryUser()
{;}

BDSComponentFactoryUser::~BDSComponentFactoryUser()
{
  for (auto& constructor : userFunctions)
    {delete constructor.second;}
}

void BDSComponentFactoryUser::RegisterComponent(const G4String& componentTypeName,
						BDSComponentConstructor* componentConstructor)
{
  userFunctions[componentTypeName] = componentConstructor;
}

G4bool BDSComponentFactoryUser::CanConstructComponentByName(const G4String& componentTypeName) const
{
  return userFunctions.find(componentTypeName) != userFunctions.end();
}

BDSAcceleratorComponent* BDSComponentFactoryUser::ConstructComponent(const G4String& componentTypeName,
								     GMAD::Element const* elementIn,
								     GMAD::Element const* prevElementIn,
								     GMAD::Element const* nextElementIn,
                     const BDSBeamlineIntegral& integral)
{
  return userFunctions[componentTypeName]->Construct(elementIn,
						     prevElementIn,
						     nextElementIn,
                 integral);
}
