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
#ifndef BDSTRAJECTORIESTOSTORE_H
#define BDSTRAJECTORIESTOSTORE_H

#include "BDSTrajectoryFilter.hh"

#include <bitset>
#include <map>
#include <utility>

class BDSTrajectory;

/**
 * @brief Double map of trajectories to bitset of which filters matched whether to store them.
 * 
 * @author Laurie Nevay
 */

class BDSTrajectoriesToStore
{
public:
  BDSTrajectoriesToStore() = delete;
  BDSTrajectoriesToStore(std::map<BDSTrajectory*, bool> trajectoriesIn,
			 std::map<BDSTrajectory*, std::bitset<BDS::NTrajectoryFilters> > filtersMatchedIn):
    trajectories(std::move(trajectoriesIn)),
    filtersMatched(std::move(filtersMatchedIn))
  {;}
  ~BDSTrajectoriesToStore(){;}
  
  std::map<BDSTrajectory*, bool> trajectories;
  std::map<BDSTrajectory*, std::bitset<BDS::NTrajectoryFilters> > filtersMatched;
};

#endif
