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
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGlobalConstants.hh"
#include "BDSImportanceFileLoader.hh"
#include "BDSParallelWorldImportance.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4GeometryCell.hh"
#include "G4IStore.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

#include <iomanip>
#include <map>
#include <string>
#include <fstream>

BDSParallelWorldImportance::BDSParallelWorldImportance(G4String name,
                                                       G4String importanceWorldGeometryFile,
                                                       G4String importanceValuesFile):
  G4VUserParallelWorld("importanceWorld_" + name),
  imWorldPV(nullptr),
  imGeomFile(importanceWorldGeometryFile),
  imVolMap(importanceValuesFile),
  componentName("importanceWorld")
{
  userLimits = BDSGlobalConstants::Instance()->DefaultUserLimits();
  visAttr    = BDSGlobalConstants::Instance()->VisibleDebugVisAttr();
  verbosity  = BDSGlobalConstants::Instance()->VerboseImportanceSampling();
#ifdef BDSDEBUG
  verbosity  = 10;
#endif
}

void BDSParallelWorldImportance::Construct()
{
  // load the cell importance values
  G4String importanceMapFile = BDS::GetFullPath(imVolMap);
  if (importanceMapFile.rfind("gz") != std::string::npos)
    {
#ifdef USE_GZSTREAM
      BDSImportanceFileLoader<igzstream> loader;
      imVolumesAndValues = loader.Load(importanceMapFile);
#else
      throw BDSException(__METHOD_NAME__, "Compressed file loading - but BDSIM not compiled with ZLIB.");
#endif
    }
  else
    {
      BDSImportanceFileLoader<std::ifstream> loader;
      imVolumesAndValues = loader.Load(importanceMapFile);
    }

  // build world
  BuildWorld();
}

BDSParallelWorldImportance::~BDSParallelWorldImportance()
{;}

void BDSParallelWorldImportance::BuildWorld()
{
  // load the importance world geometry - we give a 'component' name that *may* be
  // prepended to the loaded volume names to ensure it's unique - for example with
  // GDML preprocessing turned on.
  BDSGeometryExternal* geom = BDSGeometryFactory::Instance()->BuildGeometry(componentName,
                                                                            imGeomFile,
                                                                            nullptr,   // colour mapping
                                                                            false,     // autoColour
                                                                            0, 0,      // suggested dimensions
                                                                            nullptr,   // vacuum volumes
                                                                            false);    // sensitive

  // clone mass world for parallel world PV
  imWorldPV = GetWorld();

  G4LogicalVolume* worldLV = imWorldPV->GetLogicalVolume();

  // set parallel world vis attributes
  G4VisAttributes* importanceWorldVis = new G4VisAttributes(*(visAttr));
  importanceWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  worldLV->SetVisAttributes(importanceWorldVis);

  // set limits
  worldLV->SetUserLimits(userLimits);

  G4LogicalVolume* container = geom->GetContainerLogicalVolume();

  // Set motherLV for all daughters to be world LV, and add geometry cell
  for (G4int i = 0; i < (G4int)container->GetNoDaughters(); i++)
    {
      G4VPhysicalVolume* daughter = container->GetDaughter(i);
      daughter->SetMotherLogical(worldLV);

      G4VPhysicalVolume* parallelPV = new G4PVPlacement(daughter->GetRotation(),
							daughter->GetTranslation(),
							daughter->GetLogicalVolume(),
							daughter->GetName(),
							worldLV,
							false,
							0);
      // second arg (0) is replica number - assume no replication of cells
      G4GeometryCell cell(*parallelPV, 0);
      imVolumeStore.AddPVolume(cell);
    }
}

G4GeometryCell BDSParallelWorldImportance::GetGeometryCell(G4int i) const
{
  const G4VPhysicalVolume* p = imVolumeStore.GetPVolume(i);
  if (p)
    {return G4GeometryCell(*p,0);}
  else
    {
      G4cout << __METHOD_NAME__ << " Couldn't get G4GeometryCell" << G4endl;
      return G4GeometryCell(*imWorldPV,-2);
    }
}

void BDSParallelWorldImportance::AddIStore()
{
  G4IStore* aIstore = G4IStore::GetInstance(imWorldPV->GetName());

  // create a geometry cell for the world volume replicaNumber is 0!
  G4GeometryCell gWorldVolumeCell(*imWorldPV, 0);

  // set world volume importance to 1
  aIstore->AddImportanceGeometryCell(1, gWorldVolumeCell);

  // set importance values
  for (const auto& cell : imVolumeStore)
    {
      G4String cellName        = cell.GetPhysicalVolume().GetName();
      G4double importanceValue = GetCellImportanceValue(cellName);

      if (!aIstore->IsKnown(cell))
        {aIstore->AddImportanceGeometryCell(importanceValue, cell.GetPhysicalVolume(), 0);}
      else
        {
          G4String message = "Geometry cell \"" + cellName + "\" already exists and has been previously\n";
          message += "added to the IStore.";
          throw BDSException(__METHOD_NAME__, message);
        }
    }

  // feedback - user controllable
  if (verbosity > 0)
    {
      auto flagsCache(G4cout.flags());
      G4cout << imVolumeStore;
      for (const auto& cellAndImportance : imVolumesAndValues)
	{G4cout << std::left << std::setw(25) << cellAndImportance.first << " " << cellAndImportance.second << G4endl;}
      G4cout.flags(flagsCache);
    }
}

G4double BDSParallelWorldImportance::GetCellImportanceValue(const G4String& cellName)
{
  // strip off the prepended componentName that we introduce in the geometry factory
  // this is controlled by the member variable of this class above
  G4String pureCellName = cellName;
  if (BDS::StrContains(pureCellName, componentName))
    {
      // +1 for "_" that's added in the geometry factory
      pureCellName = pureCellName.erase(0, componentName.size()+1);
    }

  // strip off possible stupid PREPEND default from pyg4ometry
  const G4String prepend = "PREPEND";
  if (BDS::StrContains(pureCellName, prepend))
    {
      std::size_t found = pureCellName.find(prepend); // should be found as contains() found it
      pureCellName.erase(found, found + prepend.size());
    }

  // replace stupid double pv suffix from pyg4ometry perpetual bug
  if (BDS::StrContains(pureCellName, "_pv_pv")) // would only be pv pv from pyg4ometry - can't tell otherwise
    {
      std::size_t found = pureCellName.find("_pv_pv");
      pureCellName.erase(found+3, found+6);
    }

  auto result = imVolumesAndValues.find(pureCellName);
  if (result != imVolumesAndValues.end())
    {
      G4double importanceValue = (*result).second;
      // importance value must be finite and positive.
      if (importanceValue < 0)
        {
          G4String message = "Importance value is negative for cell \"" + pureCellName + "\".";
          throw BDSException(__METHOD_NAME__, message);
        }
      else if (!BDS::IsFinite(importanceValue))
        {
          G4String message = "Importance value is zero for cell \"" + pureCellName + "\".";
          throw BDSException(__METHOD_NAME__, message);
        }
      return importanceValue;
    }
  else
    {
      // exit if trying to get the importance value for a PV that isnt provided by the user.
      G4String message = "An importance value was not found for the cell \"" + pureCellName + "\" in \n";
      message += "the importance world geometry.";
      throw BDSException(__METHOD_NAME__, message);
    }
}

void BDSParallelWorldImportance::ConstructSD()
{
  /*
  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  // Sensitive Detector Name
  G4String importanceSamplingSDname = "ImportanceSamplingSD";

  // Create multifunctional detector for parallel world
  G4MultiFunctionalDetector* MFDet = new G4MultiFunctionalDetector(importanceSamplingSDname);
  SDman->AddNewDetector(MFDet); // Register SD to SDManager

  // Neutron filter
  G4SDParticleFilter* neutronFilter = new G4SDParticleFilter("neutronFilter", "neutron");
  MFDet->SetFilter(neutronFilter);
  */
}
