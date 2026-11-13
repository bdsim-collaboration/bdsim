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
#include "BDSSDType.hh"

#include <map>
#include <string>

// dictionary for BDSSDType
template<>
std::map<BDSSDType, std::string>* BDSSDType::dictionary =
  new std::map<BDSSDType, std::string> ({
					 {BDSSDType::samplerplane,           "samplerplane"},
					 {BDSSDType::samplercylinder,        "samplercylinder"},
					 {BDSSDType::samplerlink,            "samplerlink"},
					 {BDSSDType::terminator,             "terminator"},
					 {BDSSDType::energydep,              "energydep"},
					 {BDSSDType::energydepvacuum,        "energydepvacuum"},
					 {BDSSDType::energydeptunnel,        "energydeptunnel"},
					 {BDSSDType::energydepworld,         "energydepworld"},
					 {BDSSDType::energydepworldcontents, "energydepworldcontents"},
					 {BDSSDType::worldexit,              "worldexit"},
					 {BDSSDType::worldcomplete,          "worldcomplete"},
					 {BDSSDType::collimator,             "collimator"},
					 {BDSSDType::collimatorcomplete,     "collimatorcomplete"},
					 {BDSSDType::apertureimpacts,        "aperture"},
					 {BDSSDType::aperturecomplete,       "aperturecomplete"},
					 {BDSSDType::thinthing,              "thinthing"},
					 {BDSSDType::wirecomplete,           "wirecomplete"}
    });
