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
#ifndef ANALYSISSPECTRAPARTICLES_H
#define ANALYSISSPECTRAPARTICLES_H

#include <map>
#include <string>

/**
 * @brief Enum for which particles to make a spectra of.
 */

namespace RBDS
{
  enum class SpectraParticles {all, primary, secondary};

  static const std::map<std::string, SpectraParticles> spectraParticlesKeys =
    {{"p", SpectraParticles::primary},
     {"s", SpectraParticles::secondary}};
     
}

typedef std::pair<long long int, RBDS::SpectraParticles> ParticleSpec;

#endif
