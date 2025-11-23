/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSMagnetType.hh"

#include <map>

// dictionary for BDSMagnetType
template<>
std::map<BDSMagnetType, std::string>* BDSMagnetType::dictionary =
  new std::map<BDSMagnetType, std::string> ({
  {BDSMagnetType::decapole,        "decapole"},
  {BDSMagnetType::hkicker,         "hkicker"},
  {BDSMagnetType::vkicker,         "vkicker"},
  {BDSMagnetType::muonspoiler,     "muonspoiler"},
  {BDSMagnetType::octupole,        "octupole"},
  {BDSMagnetType::quadrupole,      "quadrupole"},
  {BDSMagnetType::sectorbend,      "sbend"},
  {BDSMagnetType::rectangularbend, "rbend"},
  {BDSMagnetType::rfcavity,        "rfcavity"},
  {BDSMagnetType::sextupole,       "sextupole"},
  {BDSMagnetType::undulator,       "undulator"},
  {BDSMagnetType::solenoid,        "solenoid"},
  {BDSMagnetType::multipole,       "multipole"},
  {BDSMagnetType::thinmultipole,   "thinmultipole"},
  {BDSMagnetType::dipolefringe,    "dipolefringe"},
  {BDSMagnetType::rmatrix,         "rmatrix"},
  {BDSMagnetType::paralleltransporter, "paralleltransporter"}
});
