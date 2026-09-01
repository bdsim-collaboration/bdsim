/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2024.

This file is part of BDSIM.
*/

#include "BDSBunchSixTrackLink.hh"
#include "BDSHitSamplerLink.hh"
#include "BDSIMLink.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSParticleDefinition.hh"
#include "BDSSamplerRegistry.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include "G4Proton.hh"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace
{
  bool HasSamplerNamed(const std::vector<G4String>& names, const G4String& name)
  {
    return std::find(names.begin(), names.end(), name) != names.end();
  }
}

int main(int argc, char** argv)
{
  if (argc != 4)
    {
      std::cerr << "Usage: BDSLinkSamplerTester <gmad-file> <parser|legacy> <element-name>" << std::endl;
      return 1;
    }

  const std::string inputFile  = argv[1];
  const std::string mode       = argv[2];
  const std::string elementName = argv[3];
  if (mode != "parser" && mode != "legacy")
    {
      std::cerr << "Unknown construction mode: " << mode << std::endl;
      return 1;
    }

  BDSBunchSixTrackLink bunch;
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
    {argumentPointers.push_back(argument.data());}

  link.Initialise((G4int)argumentPointers.size(), argumentPointers.data(), false);
  if (!link.Initialised())
    {
      std::cerr << "BDSLink failed to initialise" << std::endl;
      return 1;
    }

  if (mode == "legacy")
    {
      link.AddLinkCollimatorJaw(elementName,
                                "G4_Fe",
                                1 * CLHEP::m,
                                5 * CLHEP::cm,
                                5 * CLHEP::cm,
                                0,
                                0,
                                0);

      // Keep the established dynamic construction behaviour covered: it creates
      // both the link output sampler and the additional input-plane sampler.
      const auto samplerNames = BDSSamplerRegistry::Instance()->GetUniqueNamesPlane();
      if (!HasSamplerNamed(samplerNames, elementName + "_out") ||
          !HasSamplerNamed(samplerNames, elementName + "_in"))
        {
          std::cerr << "Legacy link construction did not create its existing input/output samplers" << std::endl;
          return 1;
        }
    }

  const G4double kineticEnergy = 100 * CLHEP::GeV;
  auto* particle = new BDSParticleDefinition(G4Proton::ProtonDefinition(),
                                             0,
                                             kineticEnergy,
                                             0,
                                             1);
  BDSParticleCoordsFull coordinates(1 * CLHEP::mm,
                                    -2 * CLHEP::mm,
                                    0,
                                    0,
                                    0,
                                    1,
                                    0,
                                    0,
                                    particle->TotalEnergy(),
                                    1);
  bunch.AddParticle(particle, coordinates, 17, 17);

  const G4int expectedSamplerID = link.GetLinkIndex(elementName);
  link.SelectLinkElement(elementName);
  link.BeamOn(1);

  const BDSHitsCollectionSamplerLink* hits = link.SamplerHits();
  if (!hits || hits->entries() != 1)
    {
      std::cerr << "Expected one link sampler hit" << std::endl;
      return 1;
    }
  const BDSHitSamplerLink* hit = (*hits)[0];
  if (hit->samplerID != expectedSamplerID)
    {
      std::cerr << "Hit belongs to sampler " << hit->samplerID
                << ", expected " << expectedSamplerID << std::endl;
      return 1;
    }
  return 0;
}
