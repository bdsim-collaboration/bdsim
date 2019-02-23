/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGlobalConstants.hh"
#include "BDSImportanceFileLoader.hh"
#include "BDSParallelWorldImportance.hh"
#include "BDSSDManager.hh"
#include "BDSUtilities.hh"

#include "G4GeometryCell.hh"
#include "G4IStore.hh"
#include "G4LogicalVolume.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4SDParticleFilter.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSensitiveDetector.hh"


BDSParallelWorldImportance::BDSParallelWorldImportance(G4String name):
  G4VUserParallelWorld("importanceWorld_" + name),
  imWorldPV(nullptr)
{
  userLimits = BDSGlobalConstants::Instance()->DefaultUserLimits();
  imVolMap   = BDSGlobalConstants::Instance()->ImportanceVolumeMapFile();
  imGeomFile = BDSGlobalConstants::Instance()->ImportanceWorldGeometryFile();
  visAttr    = BDSGlobalConstants::Instance()->VisibleDebugVisAttr();
}

void BDSParallelWorldImportance::Construct()
{
  // load the cell importance values
  G4String importanceMapFile = BDS::GetFullPath(imVolMap);
  BDSImportanceFileLoader importanceLoader;
  imVolumesAndValues = importanceLoader.Load(importanceMapFile);

  // build world
  BuildWorld();
}

BDSParallelWorldImportance::~BDSParallelWorldImportance()
{;}

void BDSParallelWorldImportance::BuildWorld()
{
  // load the importance world geometry
  BDSGeometryExternal* geom = BDSGeometryFactory::Instance()->BuildGeometry("importanceWorld", imGeomFile, nullptr, 0, 0);

  // clone mass world for parallel world PV
  imWorldPV = GetWorld();

  G4LogicalVolume *worldLV = imWorldPV->GetLogicalVolume();

  // set parallel world vis attributes
  G4VisAttributes* samplerWorldVis = new G4VisAttributes(*(visAttr));
  samplerWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  worldLV->SetVisAttributes(samplerWorldVis);

  // set limits
  worldLV->SetUserLimits(userLimits);

  G4LogicalVolume* container = geom->GetContainerLogicalVolume();

  // Set motherLV for all daughters to be world LV, and add geometry cell
  for (G4int i = 0; i < container->GetNoDaughters(); i++)
    {
      G4VPhysicalVolume* daughter = container->GetDaughter(i);
      daughter->SetMotherLogical(worldLV);

      G4VPhysicalVolume *parallelPV = new G4PVPlacement(daughter->GetRotation(),
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

G4VPhysicalVolume* BDSParallelWorldImportance::GetWorldVolume()
  {return imWorldPV;}


G4GeometryCell BDSParallelWorldImportance::GetGeometryCell(G4int i){
  const G4VPhysicalVolume *p = imVolumeStore.GetPVolume(i);
  if (p)
    {return G4GeometryCell(*p,0);}
  else {
    G4cout << __METHOD_NAME__ << " Couldn't get G4GeometryCell" << G4endl;
    return G4GeometryCell(*imWorldPV,-2);
  }
}

void BDSParallelWorldImportance::AddIStore() {
  G4IStore* aIstore = G4IStore::GetInstance(imWorldPV->GetName());

  // create a geometry cell for the world volume replicaNumber is 0!
  G4GeometryCell gWorldVolumeCell(*imWorldPV, 0);

  // set world volume importance to 1
  aIstore->AddImportanceGeometryCell(1, gWorldVolumeCell);

  // set importance values and create scorers
  G4int numCells = (G4int) imVolumesAndValues.size();

  for (G4int cell = 0; cell < numCells; cell++) {
    G4GeometryCell gCell  = GetGeometryCell(cell);
    G4String cellName     = gCell.GetPhysicalVolume().GetName();
    G4double importanceValue = GetCellImportanceValue(cellName);

    //only add to store if it hasn't already been added. 0 is replica index.
    if (!aIstore->IsKnown(gCell))
      {aIstore->AddImportanceGeometryCell(importanceValue, gCell.GetPhysicalVolume(), 0);}
    else
      {
        G4cerr << __METHOD_NAME__ << "Geometry cell " << cellName << "already exists and has been previously "
               << "added to the IStore." << G4endl;
        exit(1);
      }
  }
}

G4double BDSParallelWorldImportance::GetCellImportanceValue(G4String cellName)
  {
    // set importance value to 1 if trying to get the value for a PV that isnt provided by the user.
    // cell value of 1 is global default
    G4double importanceValue = (imVolumesAndValues)[cellName];
    if (!BDS::IsFinite(importanceValue))
      {
        G4String finalCellName = cellName;
        // prependage and appendage added in pyg4ometry
        G4String preString = "importanceWorld_PREPEND";
        G4String postString = "_pv";

        // only modify name if it contains the prestring - we modify in pyg4ometry (PREPEND)
        // and this class (importanceWorld_), whereas the user will only know the name they defined.
        // can't check for poststring as G4 PV naming convention includes it.
        if (cellName.contains(preString))
          {
            cellName = cellName.erase(0, preString.size());
            finalCellName = cellName.erase(cellName.size() - postString.size(), postString.size());
          }

        G4cerr << __METHOD_NAME__<< "An importance value was not found (or is " << G4endl;
        G4cerr << "zero) for the importance world cell " << finalCellName <<". Please check that your importanceVolumeMap " << G4endl;
        G4cerr << "file has a finite importance value for this volume." << G4endl;
        exit(1);
      }
    if (importanceValue < 0)
      {
        G4cerr << __METHOD_NAME__<< "Importance value is negative for cell " << cellName <<"." << G4endl;
        exit(1);
      }

    return importanceValue;
  }

void BDSParallelWorldImportance::ConstructSD()
{
  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  // Sensitive Detector Name
  G4String importanceSamplingSDname = "ImportanceSamplingSD";

  // Create multifunctional detector for parallel world
  G4MultiFunctionalDetector* MFDet = new G4MultiFunctionalDetector(importanceSamplingSDname);
  SDman->AddNewDetector(MFDet); // Register SD to SDManager

  // Neutron filter
  G4SDParticleFilter* neutronFilter = new G4SDParticleFilter("neutronFilter", "neutron");
  MFDet->SetFilter(neutronFilter);

  //TODO: Add scorers here if necessary. Follow recipe in G4 example B03ImportanceDetectorConstruction.
}