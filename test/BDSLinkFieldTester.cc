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

#include "BDSHitSamplerLink.hh"
#include "BDSIMLink.hh"
#include "BDSLinkBunch.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSParticleDefinition.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include "G4Proton.hh"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
  if (argc != 5)
    {
      std::cerr << "Usage: BDSLinkFieldTester <gmad-file> <element-name> "
                << "<map-reference-x-m> <map-reference-xp>" << std::endl;
      return 1;
    }

  const std::string inputFile   = argv[1];
  const std::string elementName = argv[2];

  BDSLinkBunch bunch;
  BDSIMLink link(&bunch);
  std::vector<std::string> arguments = {
    "bdsim",
    "--file=" + inputFile,
    "--output=None",
    "--batch",
    "--seed=1234"
  };
  std::vector<char*> argumentPointers;
  for (auto& argument : arguments)
    {argumentPointers.push_back(&argument[0]);}

  link.Initialise((G4int)argumentPointers.size(), argumentPointers.data(), false);
  if (!link.Initialised())
    {
      std::cerr << "BDSLink failed to initialise" << std::endl;
      return 1;
    }

  const G4double kineticEnergy = 100 * CLHEP::GeV;
  auto* particle = new BDSParticleDefinition(G4Proton::ProtonDefinition(),
                                             0,
                                             kineticEnergy,
                                             0,
                                             1);
  BDSParticleCoordsFull coordinates(10 * CLHEP::mm,
                                    2 * CLHEP::mm,
                                    0,
                                    0,
                                    0,
                                    1,
                                    0,
                                    0,
                                    particle->TotalEnergy(),
                                    1);
  bunch.AddParticle(particle, coordinates, 17, 17);

  link.SelectLinkElement(elementName);
  link.BeamOn(1);
  bunch.ClearParticles(); // sampler hits contain their own coordinate values

  const BDSHitsCollectionSamplerLink* hits = link.SamplerHits();
  if (!hits || hits->entries() != 1)
    {
      std::cerr << "Expected one link sampler hit" << std::endl;
      return 1;
    }

  const BDSHitSamplerLink* hit = (*hits)[0];
  const G4double xp = hit->coords.xp;
  const G4double yp = hit->coords.yp;
  if (!(std::isfinite(hit->coords.x) && std::isfinite(hit->coords.y) &&
        std::isfinite(xp) && std::isfinite(yp)))
    {
      std::cerr << "Non-finite transverse sampler coordinates" << std::endl;
      return 1;
    }
  // Reference values from standalone BDSIM's float sampler for this map.
  const G4double expectedX = std::stod(argv[3]) * CLHEP::m;
  const G4double expectedXP = std::stod(argv[4]);
  if (std::abs(hit->coords.x - expectedX) > 10 * CLHEP::nm ||
      std::abs(xp - expectedXP) > 1e-9)
    {
      std::cerr << "Field-map tracking differs from standalone BDSIM: x="
                << hit->coords.x / CLHEP::m << " m, xp=" << xp << std::endl;
      return 1;
    }

  return 0;
}
