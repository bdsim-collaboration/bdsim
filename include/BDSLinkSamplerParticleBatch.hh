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

#ifndef BDSLINKSAMPLERPARTICLEBATCH_H
#define BDSLINKSAMPLERPARTICLEBATCH_H

#include "BDSLinkParticleBatch.hh"

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @brief Particle data extracted from a BDSIM link sampler.
 *
 * Combines the common particle phase-space arrays with the ancestry,
 * Geant4 tracking and statistical-weight information associated with each
 * sampler hit. All arrays must contain one entry per sampler hit.
 */
struct BDSLinkSamplerParticleBatch
{
  /// Common particle phase-space data and external identifiers.
  BDSLinkParticleBatch particles;

  /// Parent, Geant4 track and event identifiers for each sampler hit.
  std::vector<int> parentID;
  std::vector<int> trackID;
  std::vector<int> eventID;

  /// Statistical weight for each sampler hit.
  std::vector<double> weight;

  /// Throw std::runtime_error if the sampler arrays have different sizes.
  void Validate() const
  {
    // Validate the common particle arrays first.
    particles.Validate();

    const std::size_t n =
      particles.Size();

    const auto checkSize =
      [n](std::size_t actual,
          const std::string& name)
      {
        if (actual != n)
          {
            throw std::runtime_error(
              "BDSLinkSamplerParticleBatch: field \"" +
              name + "\" has size " +
              std::to_string(actual) +
              ", expected " +
              std::to_string(n)
            );
          }
      };

    // Validate only the sampler-specific arrays here.
    checkSize(parentID.size(), "parentID");
    checkSize(trackID.size(),  "trackID");
    checkSize(eventID.size(),  "eventID");
    checkSize(weight.size(),   "weight");
  }
};

#endif
