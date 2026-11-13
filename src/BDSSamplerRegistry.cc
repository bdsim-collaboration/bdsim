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
#include "BDSSampler.hh"
#include "BDSSamplerRegistry.hh"

#include "globals.hh"
#include "G4Transform3D.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

BDSSamplerRegistry* BDSSamplerRegistry::instance = nullptr;

BDSSamplerRegistry* BDSSamplerRegistry::Instance()
{
  if (!instance)
    {instance = new BDSSamplerRegistry();}
  return instance;
}

BDSSamplerRegistry::BDSSamplerRegistry():
  numberOfEntries(0)
{
  samplerTypeToCategory = { {BDSSamplerType::plane,           BDSSamplerType::plane},
                            {BDSSamplerType::cylinder,        BDSSamplerType::cylinder},
                            {BDSSamplerType::cylinderforward, BDSSamplerType::cylinder},
                            {BDSSamplerType::sphere,          BDSSamplerType::sphere},
                            {BDSSamplerType::sphereforward,   BDSSamplerType::sphere} };
  samplerIDsPerType[BDSSamplerType::plane]    = std::vector<G4int>();
  samplerIDsPerType[BDSSamplerType::cylinder] = std::vector<G4int>();
  samplerIDsPerType[BDSSamplerType::sphere]   = std::vector<G4int>();
}

BDSSamplerRegistry::~BDSSamplerRegistry()
{
  for (auto s : samplerObjects)
    {delete s;}
  instance = nullptr;
}

G4int BDSSamplerRegistry::RegisterSampler(const G4String&      name,
					  BDSSampler*          sampler,
					  const G4Transform3D& transform,
					  G4double             S,
					  const BDSBeamlineElement* element,
					  BDSSamplerType       type,
					  G4double             radius)
{
  samplerObjects.insert(sampler);
  G4String uniqueName = name;
  auto result = existingNames.find(name);
  if (result == existingNames.end())
    {// wasn't found - introduce it
      existingNames[name] = 1;
    }
  else
    {
      uniqueName = name + "_" + std::to_string(existingNames[name]);
      existingNames[name]++;
    }
  BDSSamplerPlacementRecord info = BDSSamplerPlacementRecord(name, sampler, transform, S, element, uniqueName, type, radius);
  return RegisterSampler(info);
}

G4int BDSSamplerRegistry::RegisterSampler(BDSSamplerPlacementRecord& info)
{
  infos.push_back(info);

  G4int index = numberOfEntries; // copy the number of entries / the index of this entry
  numberOfEntries++;
  
  // cache the ID for this type of sampler
  samplerIDsPerType[samplerTypeToCategory[info.Type()]].push_back(index);
  return index;
}

std::vector<G4String> BDSSamplerRegistry::GetNames() const
{
  std::vector<G4String> names;
  for (const auto& info : infos)
    {names.push_back(info.Name());}
  return names;
}

std::vector<G4String> BDSSamplerRegistry::GetUniqueNames() const
{
  std::vector<G4String> names;
  for (const auto& info : infos)
    {names.push_back(info.UniqueName());}
  return names;
}

std::vector<G4String> BDSSamplerRegistry::GetUniqueNamesPlane() const
{
  std::vector<G4String> names;
  for (const auto& info: infos)
    {
      if (info.Type() == BDSSamplerType::plane)
        {names.push_back(info.UniqueName());}
    }
  return names;
}

std::vector<G4String> BDSSamplerRegistry::GetUniqueNamesCylinder() const
{
  std::vector<G4String> names;
  for (const auto& info: infos)
    {
      if (info.Type() == BDSSamplerType::cylinder || info.Type() == BDSSamplerType::cylinderforward)
        {names.push_back(info.UniqueName());}
    }
  return names;
}

std::vector<G4String> BDSSamplerRegistry::GetUniqueNamesSphere() const
{
  std::vector<G4String> names;
  for (const auto& info: infos)
    {
      if (info.Type() == BDSSamplerType::sphere || info.Type() == BDSSamplerType::sphereforward)
        {names.push_back(info.UniqueName());}
    }
  return names;
}

std::map<std::string, double> BDSSamplerRegistry::GetUniqueNameToRadiusCylinder() const
{
  std::map<std::string, double> result;
  for (const auto& info: infos)
    {
      if (info.Type() == BDSSamplerType::cylinder || info.Type() == BDSSamplerType::cylinderforward)
        {result[static_cast<std::string>(info.UniqueName())] = info.Radius()/CLHEP::m;}
    }
  return result;
}

std::map<std::string, double> BDSSamplerRegistry::GetUniqueNameToRadiusSphere() const
{
  std::map<std::string, double> result;
  for (const auto& info: infos)
    {
      if (info.Type() == BDSSamplerType::sphere || info.Type() == BDSSamplerType::sphereforward)
        {result[static_cast<std::string>(info.UniqueName())] = info.Radius()/CLHEP::m;}
    }
  return result;
}

std::vector<std::pair<G4String, G4double> > BDSSamplerRegistry::GetUniquePlaneNamesAndSPosition() const
{
  std::vector<std::pair<G4String, G4double> > result;
  for (const auto& info : infos)
    {
      if (info.Type() == BDSSamplerType::plane)
        {result.emplace_back(std::make_pair(info.UniqueName(), info.SPosition()));}
    }
  return result;
}
