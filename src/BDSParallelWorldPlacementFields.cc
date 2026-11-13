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
#include "BDSAcceleratorModel.hh"
#include "BDSDetectorConstruction.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSNavigatorPlacements.hh"
#include "BDSParallelWorldPlacementFields.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4String.hh"
#include "G4Types.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include <vector>


BDSParallelWorldPlacementFields::BDSParallelWorldPlacementFields(const G4String& name):
  G4VUserParallelWorld("PlacementFieldsWorld_" + name),
  pfWorldVis(nullptr)
{;}

BDSParallelWorldPlacementFields::~BDSParallelWorldPlacementFields()
{
  // we leak all the placements as g4 is incredibly slow to delete
  // them as it deregisters them - g4 will clean up
  //for (auto placement : placements)
  //  {delete placement;}
  delete pfWorldVis;
}

void BDSParallelWorldPlacementFields::Construct()
{
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  /// "placement fields" -> pf
  G4VPhysicalVolume* pfWorld   = GetWorld();
  G4LogicalVolume*   pfWorldLV = pfWorld->GetLogicalVolume();
  
  /// Register it (statically) to all navigators for fields in the placement world.
  BDSNavigatorPlacements::AttachWorldVolumeToNavigator(pfWorld);

  pfWorldVis = new G4VisAttributes(*(globals->VisibleDebugVisAttr()));
  pfWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  pfWorldLV->SetVisAttributes(pfWorldVis);

  G4bool checkOverlaps = globals->CheckOverlaps();
  const auto& placementsToMake = BDSAcceleratorModel::Instance()->PlacementFieldPWPlacements();
  for (const auto& placement : placementsToMake)
    {
      // record placements for cleaning up at destruction.
      G4PVPlacement* pl = new G4PVPlacement(placement.transform,
                                            placement.name,
                                            placement.lvToPlace,
                                            pfWorld,
                                            false,
                                            placement.copyNumber,
                                            checkOverlaps);
      placements.push_back(pl);
    }
}
