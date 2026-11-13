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
#include "BDSCavityFactory.hh"
#include "BDSCavityFactoryBase.hh"
#include "BDSCavityFactoryElliptical.hh"
#include "BDSCavityFactoryPillBox.hh"
#include "BDSCavityFactoryRectangular.hh"
#include "BDSCavityInfo.hh"
#include "BDSCavityType.hh"
#include "BDSDebug.hh"

#include "globals.hh"

BDSCavityFactory* BDSCavityFactory::instance = nullptr;

BDSCavityFactory* BDSCavityFactory::Instance()
{
  if (!instance)
    {instance = new BDSCavityFactory();}
  return instance;
}

BDSCavityFactory::BDSCavityFactory():
  elliptical(new BDSCavityFactoryElliptical()),
  rectangular(new BDSCavityFactoryRectangular()),
  pillBox(new BDSCavityFactoryPillBox())
{;}

BDSCavityFactory::~BDSCavityFactory()
{
  delete elliptical;
  delete rectangular;
  delete pillBox;
  instance = nullptr;
}

BDSCavityFactoryBase* BDSCavityFactory::GetAppropriateFactory(const BDSCavityType cavityType)
{
  BDSCavityFactoryBase* result = pillBox;
  switch (cavityType.underlying())
    {
    case BDSCavityType::elliptical:
      {result = elliptical; break;}
    case BDSCavityType::rectangular:
      {result = rectangular; break;}
    case BDSCavityType::pillbox:
      {result = pillBox; break;}
    default:
      {break;}
    }
  return result;
}

BDSCavity* BDSCavityFactory::CreateCavity(const G4String&      name,
					  G4double             totalChordLength,
					  const BDSCavityInfo* info,
					  G4Material*          vacuumMaterial)
{
  BDSCavityFactoryBase* factory = GetAppropriateFactory(info->cavityType);

  return factory->CreateCavity(name, totalChordLength, info, vacuumMaterial);
}
