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
#ifndef BDSCPARTICLES_H
#define BDSCPARTICLES_H

#include <map>
#include <string>

/// BDSC = BDSIM Convert namespace
namespace BDSC
{
  /// Simple article information holder.
  struct ParticleInfo
  {
    int    pdgID;
    double mass; // in GeV
  };

  /// Simple map of some particle properties
  const std::map<std::string, ParticleInfo> particles =
  {
    {"proton", {2212, 0.938272}},
    {"e-",     {11,   0.000510999}},
    {"e+",     {11,   0.000510999}}
  };

  /// Access the simple particle information.
  ParticleInfo GetParticleInfo(const std::string& particleName); 
}

#endif
