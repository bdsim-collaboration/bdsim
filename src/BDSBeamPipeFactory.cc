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
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipeFactoryGeneral.hh"
#include "BDSBeamPipeFactoryLHCDetailed.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBeamPipeType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"

#include "G4String.hh"
#include "G4Types.hh"

BDSBeamPipeFactory::BDSBeamPipeFactory()
{
  general     = new BDSBeamPipeFactoryGeneral();
  lhcdetailed = new BDSBeamPipeFactoryLHCDetailed();
}

BDSBeamPipeFactory::~BDSBeamPipeFactory()
{
  delete general;
  delete lhcdetailed;
}

BDSBeamPipeFactoryBase* BDSBeamPipeFactory::GetAppropriateFactory(BDSBeamPipeType type)
{
  BDSBeamPipeFactoryBase* result;
  switch(type.underlying())
    {
      case BDSBeamPipeType::circular:
      case BDSBeamPipeType::elliptical:
      case BDSBeamPipeType::rectangular:
      case BDSBeamPipeType::lhc:
      case BDSBeamPipeType::rectellipse:
      case BDSBeamPipeType::racetrack:
      case BDSBeamPipeType::octagonal:
      case BDSBeamPipeType::circularvacuum:
      case BDSBeamPipeType::clicpcl:
      case BDSBeamPipeType::pointsfile:
      case BDSBeamPipeType::rhombus:
        {result = general; break;}
      case BDSBeamPipeType::lhcdetailed:
        {result = lhcdetailed; break;}
      default:
        {
          throw BDSException(__METHOD_NAME__, "unimplemented beam pipe type.");
          break;
        }
    }
  return result;
}

BDSBeamPipe* BDSBeamPipeFactory::CreateBeamPipe(const G4String&  name,
                                                G4double         length,
                                                BDSBeamPipeInfo*  bpi)
{
  BDSBeamPipeFactoryBase* factory = GetAppropriateFactory(bpi->beamPipeType);
  return factory->CreateBeamPipe(name, length, bpi);
}