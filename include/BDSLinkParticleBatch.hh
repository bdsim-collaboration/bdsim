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

#ifndef BDSLINKPARTICLEBATCH_H
#define BDSLINKPARTICLEBATCH_H

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @brief Particle data exchanged with BDSLinkTrackerInterface.
 *
 * Stores one value per particle in structure-of-arrays form. All arrays
 * must have the same size. Coordinates and momenta use the units expected
 * by BDSLinkTrackerInterface::AddParticles().
 */
struct BDSLinkParticleBatch
{
  /// Horizontal and vertical positions in mm.
  std::vector<double> x;   // mm
  std::vector<double> y;   // mm

  /// Cartesian momentum components in MeV/c.
  std::vector<double> px;  // MeV/c
  std::vector<double> py;  // MeV/c
  std::vector<double> pz;  // MeV/c

  /// Time in ns and longitudinal position in mm.
  std::vector<double> t;   // ns
  std::vector<double> s;   // mm

  /// External particle identifiers and PDG identifiers.
  std::vector<int> particleID;
  std::vector<int> pdgID;

  /// Return the number of particles in the batch.
  std::size_t Size() const
  {
    return x.size();
  }

  /// Return whether the batch contains no particles.
  bool Empty() const
  {
    return x.empty();
  }

  /// Throw std::runtime_error if the particle arrays have different sizes.
  void Validate() const
  {
    const std::size_t n = x.size();

    const auto checkSize =
      [n](std::size_t actual,
          const std::string& name)
      {
        if (actual != n)
          {
            throw std::runtime_error(
              "BDSLinkParticleBatch: field \"" +
              name + "\" has size " +
              std::to_string(actual) +
              ", expected " +
              std::to_string(n)
            );
          }
      };

    checkSize(y.size(),          "y");
    checkSize(px.size(),         "px");
    checkSize(py.size(),         "py");
    checkSize(pz.size(),         "pz");
    checkSize(t.size(),          "t");
    checkSize(s.size(),          "s");
    checkSize(particleID.size(), "particleID");
    checkSize(pdgID.size(),      "pdgID");
  }
};

#endif
