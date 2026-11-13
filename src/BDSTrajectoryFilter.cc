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
#include "BDSTrajectoryFilter.hh"

#include <map>
#include <string>

// dictionary for BDSTrajectoryFilter
template<>
std::map<BDSTrajectoryFilter, std::string>* BDSTrajectoryFilter::dictionary =
  new std::map<BDSTrajectoryFilter, std::string> ({
   {BDSTrajectoryFilter::primary,         "primary"},
   {BDSTrajectoryFilter::depth,           "depth"},
   {BDSTrajectoryFilter::particle,        "particle"},
   {BDSTrajectoryFilter::energyThreshold, "energyThreshold"},
   {BDSTrajectoryFilter::sampler,         "sampler"},
   {BDSTrajectoryFilter::elossSRange,     "elossSRange"},
   {BDSTrajectoryFilter::minimumZ,        "minimumZ"},
   {BDSTrajectoryFilter::maximumR,        "maximumR"},
   {BDSTrajectoryFilter::connect,         "connect"},
   {BDSTrajectoryFilter::secondary,       "secondary"}
});

namespace BDS
{
  BDSTrajectoryFilter BDSTrajectoryFilterEnumOfIndex(int i)
    {return static_cast<BDSTrajectoryFilter>(i);}
}
