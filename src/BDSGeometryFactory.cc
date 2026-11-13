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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSSDEnergyDeposition.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGeometryFactoryBase.hh"
#ifdef USE_GDML
#include "BDSGeometryFactoryGDML.hh"
#endif
#include "BDSGeometryFactorySQL.hh"
#include "BDSGeometryType.hh"
#include "BDSSDType.hh"
#include "BDSUtilities.hh"

#include "G4String.hh"
#include "G4Types.hh"

#include <map>
#include <string>
#include <utility>

BDSGeometryFactory* BDSGeometryFactory::instance = nullptr;

BDSGeometryFactory* BDSGeometryFactory::Instance()
{
  if (!instance)
    {instance = new BDSGeometryFactory();}
  return instance;
}

BDSGeometryFactory::BDSGeometryFactory()
{
#ifdef USE_GDML
  gdml = new BDSGeometryFactoryGDML();
#else
  gdml = nullptr;
#endif
  sql  = new BDSGeometryFactorySQL();
}

BDSGeometryFactory::~BDSGeometryFactory()
{
  delete gdml;
  delete sql;
  for (auto& geom : storage)
    {delete geom;}
  instance = nullptr;
}

BDSGeometryFactoryBase* BDSGeometryFactory::GetAppropriateFactory(BDSGeometryType type)
{
  switch(type.underlying())
    {
#ifdef USE_GDML
    case BDSGeometryType::gdml:
      {return gdml; break;}
#endif
    case BDSGeometryType::mokka:
      {return sql; break;}
    default:
      {
        G4cout << "Unsupported factory type " << type;
        return nullptr;
      }
    }
}

BDSGeometryExternal* BDSGeometryFactory::BuildGeometry(G4String               componentName,
                                                       const G4String&        formatAndFileName,
                                                       std::map<G4String, G4Colour*>* colourMapping,
                                                       G4bool                 autoColour,
                                                       G4double               suggestedLength,
                                                       G4double               suggestedHorizontalWidth,
                                                       std::vector<G4String>* namedVacuumVolumes,
                                                       G4bool                 makeSensitive,
                                                       BDSSDType              sensitivityType,
                                                       BDSSDType              vacuumSensitivityType,
                                                       G4bool                 stripOuterVolumeAndMakeAssembly,
                                                       G4UserLimits*          userLimitsToAttachToAllLVs,
                                                       G4bool                 dontReloadGeometry)
{
  std::pair<G4String, G4String> ff = BDS::SplitOnColon(formatAndFileName);
  G4String fileName = BDS::GetFullPath(ff.second);

  G4String searchName = fileName;
  // If we strip the outer volume we're technically modifying the geometry
  // and should prepare a unique load of it and cache that otherwise, we
  // will get either the original or the stripped version only if we place
  // the load the same geometry twice with/without stripping
  if (stripOuterVolumeAndMakeAssembly)
    {searchName += "_stripped";}
  
  // we have this option to purposively not reload geometry uniquely for multiple
  // instances by different component names - we need this sometimes, but we must
  // use it cautiously knowing all the instances will be identical
  // therefore we use a fixed nonsense name that someone is prevented from calling
  // a component (because it's an option)
  G4String searchComponentName = componentName;
  if (dontReloadGeometry)
    {searchComponentName = "dontReloadGeometry";}
  // we must still build it with component name in case it's preprocessed. If we used
  // 'dontReloadGeometry' then variables / stuff in that file could be degeneratively
  // named in other unique geometry files that are loaded.
  
  auto nameAndField = std::make_pair((std::string)searchName, searchComponentName);
  const auto search = registry.find(nameAndField);
  if (search != registry.end())
    {return search->second;}// it was found already in registry
  // else wasn't found so continue

  // Check the file exists.
  if (!BDS::FileExists(fileName))
    {throw BDSException(__METHOD_NAME__, "No such file \"" + fileName + "\"");}
  
  BDSGeometryType format = BDS::DetermineGeometryType(ff.first);
  BDSGeometryFactoryBase* factory = GetAppropriateFactory(format);
  if (!factory)
    {return nullptr;}
  
  BDSGeometryExternal* result = factory->Build(componentName,
                                               fileName,
                                               colourMapping,
                                               autoColour,
                                               suggestedLength,
                                               suggestedHorizontalWidth,
                                               namedVacuumVolumes,
                                               makeSensitive,
                                               sensitivityType,
                                               vacuumSensitivityType,
                                               userLimitsToAttachToAllLVs);
  
  if (result)
    {
      if (stripOuterVolumeAndMakeAssembly)
        {result->StripOuterAndMakeAssemblyVolume();}

      // cache using optionally modified name
      auto key = std::make_pair((std::string)searchName, searchComponentName);
      registry[key] = result;
      storage.insert(result);
    }
  
  return result;
}
