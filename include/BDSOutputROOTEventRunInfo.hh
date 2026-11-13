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
#ifndef BDSOUTPUTROOTEVENTRUN_H
#define BDSOUTPUTROOTEVENTRUN_H

#include "Rtypes.h"
#include "TObject.h"

#include <ctime>
#include <string>

class BDSOutputROOTEventInfo;

/**
 * @brief Information pertaining to a run.
 *
 * @author Stewart Boogert
 */

class BDSOutputROOTEventRunInfo: public TObject
{
public:
  BDSOutputROOTEventRunInfo();
  explicit BDSOutputROOTEventRunInfo(const BDSOutputROOTEventInfo* info);
  virtual ~BDSOutputROOTEventRunInfo();
  void Flush();

  time_t startTime;
  time_t stopTime;
  double durationWall;
  double durationCPU;
  std::string seedStateAtStart; ///< Seed state at the start of the event.
  
  ClassDef(BDSOutputROOTEventRunInfo,3);
};

#endif
