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
  if (argc != 3 && argc != 4)
    {
      std::cerr << "Usage: BDSLinkStraightElementTester <gmad-file> <element-name> [missing]"
                << std::endl;
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

  if (argc == 4)
    {
      if (std::string(argv[3]) != "missing")
        {
          std::cerr << "Unknown check: " << argv[3] << std::endl;
          return 1;
        }
      if (link.GetLinkIndex(elementName) != -1)
        {
          std::cerr << "Element was registered although BDSIM did not construct it: "
                    << elementName << std::endl;
          return 1;
        }
      return 0;
    }

  const G4double kineticEnergy = 100 * CLHEP::GeV;
  auto* particle = new BDSParticleDefinition(G4Proton::ProtonDefinition(),
                                             0,
                                             kineticEnergy,
                                             0,
                                             1);
  const G4double x0  = 1 * CLHEP::mm;
  const G4double y0  = -2 * CLHEP::mm;
  const G4double xp0 = 1e-3;
  const G4double yp0 = -2e-3;
  const G4double zp0 = std::sqrt(1 - xp0*xp0 - yp0*yp0);
  BDSParticleCoordsFull coordinates(x0,
                                    y0,
                                    0,
                                    xp0,
                                    yp0,
                                    zp0,
                                    0,
                                    0,
                                    particle->TotalEnergy(),
                                    1);
  bunch.AddParticle(particle, coordinates, 17, 17);

  const G4int expectedSamplerID = link.GetLinkIndex(elementName);
  link.SelectLinkElement(elementName);
  link.BeamOn(1);
  bunch.ClearParticles();

  const BDSHitsCollectionSamplerLink* hits = link.SamplerHits();
  if (!hits || hits->entries() != 1)
    {
      std::cerr << "Expected one link sampler hit for " << elementName << std::endl;
      return 1;
    }
  const BDSHitSamplerLink* hit = (*hits)[0];
  if (hit->samplerID != expectedSamplerID)
    {
      std::cerr << "Hit belongs to sampler " << hit->samplerID
                << ", expected " << expectedSamplerID << std::endl;
      return 1;
    }

  const G4double expectedX = x0 + 0.5 * CLHEP::m * xp0 / zp0;
  const G4double expectedY = y0 + 0.5 * CLHEP::m * yp0 / zp0;
  if (std::abs(hit->coords.x - expectedX) > 10 * CLHEP::nm ||
      std::abs(hit->coords.y - expectedY) > 10 * CLHEP::nm ||
      std::abs(hit->coords.xp - xp0) > 1e-12 ||
      std::abs(hit->coords.yp - yp0) > 1e-12)
    {
      std::cerr << "Unexpected transport through " << elementName
                << ": x=" << hit->coords.x / CLHEP::mm << " mm"
                << ", y=" << hit->coords.y / CLHEP::mm << " mm"
                << ", xp=" << hit->coords.xp
                << ", yp=" << hit->coords.yp << std::endl;
      return 1;
    }

  return 0;
}
