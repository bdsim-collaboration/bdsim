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
#include "BDSOutputROOTEventRunInfo.hh"

#include <ctime>

ClassImp(BDSOutputROOTEventRunInfo)

BDSOutputROOTEventRunInfo::BDSOutputROOTEventRunInfo():
  startTime(time_t()),
  stopTime(time_t()),
  durationWall(0),
  durationCPU(0)
{;}

BDSOutputROOTEventRunInfo::BDSOutputROOTEventRunInfo(const BDSOutputROOTEventInfo* info):
  startTime(info->startTime),
  stopTime(info->stopTime),
  durationWall(info->durationWall),
  durationCPU(info->durationCPU),
  seedStateAtStart(info->seedStateAtStart)
{;}

BDSOutputROOTEventRunInfo::~BDSOutputROOTEventRunInfo()
{;}

void BDSOutputROOTEventRunInfo::Flush()
{
  startTime        = time_t();
  stopTime         = time_t();
  durationWall     = 0;
  durationCPU      = 0;
  seedStateAtStart = "";
}
