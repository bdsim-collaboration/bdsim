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
#include "BDSOutputROOTEventInfo.hh"

#include <ctime>

ClassImp(BDSOutputROOTEventInfo)

BDSOutputROOTEventInfo::BDSOutputROOTEventInfo():
  startTime(time_t()),
  stopTime(time_t()),
  durationWall(0),
  durationCPU(0),
  index(-1),
  aborted(false),
  primaryHitMachine(false),
  primaryAbsorbedInCollimator(false),
  memoryUsageMb(0),
  energyDeposited(0),
  energyDepositedVacuum(0),
  energyDepositedWorld(0),
  energyDepositedWorldContents(0),
  energyDepositedTunnel(0),
  energyWorldExit(0),
  energyWorldExitKinetic(0),
  energyImpactingAperture(0),
  energyImpactingApertureKinetic(0),
  energyKilled(0),
  energyTotal(0),
  nCollimatorsInteracted(0),
  nTracks(0),
  bunchIndex(0)
{;}

BDSOutputROOTEventInfo::~BDSOutputROOTEventInfo()
{;}

void BDSOutputROOTEventInfo::Flush()
{
  startTime         = time_t();
  stopTime          = time_t();
  durationWall      = 0;
  durationCPU       = 0;
  seedStateAtStart  = "";
  index             = -1;
  aborted           = false;
  primaryHitMachine = false;
  primaryAbsorbedInCollimator = false;
  memoryUsageMb         = 0;
  energyDeposited       = 0;
  energyDepositedVacuum = 0;
  energyDepositedWorld  = 0;
  energyDepositedWorldContents = 0;
  energyDepositedTunnel = 0;
  energyWorldExit       = 0;
  energyWorldExitKinetic = 0;
  energyImpactingAperture = 0;
  energyImpactingApertureKinetic = 0;
  energyKilled          = 0;
  energyTotal           = 0;
  nCollimatorsInteracted = 0;
  nTracks                = 0;
  bunchIndex             = 0;
}

void BDSOutputROOTEventInfo::Fill(const BDSOutputROOTEventInfo* other)
{
  if (!other)
    {return;}

  startTime               = other->startTime;
  stopTime                = other->stopTime;
  durationWall            = other->durationWall;
  durationCPU             = other->durationCPU;
  seedStateAtStart        = other->seedStateAtStart;
  index                   = other->index;
  aborted                 = other->aborted;
  primaryHitMachine       = other->primaryHitMachine;
  primaryAbsorbedInCollimator = other->primaryAbsorbedInCollimator;
  memoryUsageMb           = other->memoryUsageMb;
  energyDeposited         = other->energyDeposited;
  energyDepositedVacuum   = other->energyDepositedVacuum;
  energyDepositedWorld    = other->energyDepositedWorld;
  energyDepositedWorldContents = other->energyDepositedWorldContents;
  energyDepositedTunnel   = other->energyDepositedTunnel;
  energyWorldExit         = other->energyWorldExit;
  energyWorldExitKinetic  = other->energyWorldExitKinetic;
  energyImpactingAperture = other->energyImpactingAperture;
  energyImpactingApertureKinetic = other->energyImpactingApertureKinetic;
  energyKilled            = other->energyKilled;
  energyTotal             = other->energyTotal;
  nCollimatorsInteracted  = other->nCollimatorsInteracted;
  nTracks                 = other->nTracks;
  bunchIndex              = other->bunchIndex;
}
