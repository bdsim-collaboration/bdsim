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
#include "BDSEventInfo.hh"
#include "BDSOutputROOTEventInfo.hh"

#include "globals.hh"

BDSEventInfo::BDSEventInfo():
  info(new BDSOutputROOTEventInfo())
{;}

BDSEventInfo::~BDSEventInfo()
{
  delete info;
}

void BDSEventInfo::Print() const
{
  G4cout << "Start / stop time (ms): " << info->startTime << " " << info->stopTime << G4endl;
  G4cout << "Duration Wall (ms)    : " << info->durationWall << G4endl;
  G4cout << "Duration CPU  (ms)    : " << info->durationCPU  << G4endl;
}
