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
#include "BDSAuxiliaryNavigator.hh"
#include "BDSDebug.hh"
#include "BDSDetectorConstruction.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParallelWorldCurvilinear.hh"

#include "G4String.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

class BDSBeamline;

BDSParallelWorldCurvilinear::BDSParallelWorldCurvilinear(const G4String& name):
  G4VUserParallelWorld("CurvilinearWorld_" + name),
  suffix(name),
  clWorldVis(nullptr)
{;}

BDSParallelWorldCurvilinear::~BDSParallelWorldCurvilinear()
{
  delete clWorldVis;
}

void BDSParallelWorldCurvilinear::Construct()
{// general abbreviation: 'cl' == curvilinear
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  G4VPhysicalVolume* clWorld = GetWorld();

  // TODO - only register main one for now
  // register read out world PV with our auxiliary navigator.
  if (suffix == "main")
    {BDSAuxiliaryNavigator::AttachWorldVolumeToNavigatorCL(clWorld);}

  G4LogicalVolume* clWorldLV = clWorld->GetLogicalVolume();

  // visualisation
  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  clWorldVis = new G4VisAttributes(*(globals->VisibleDebugVisAttr()));
  clWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  clWorldLV->SetVisAttributes(clWorldVis);

  BDSBeamlineSet blSet = BDSAcceleratorModel::Instance()->BeamlineSet(suffix);

  BDSDetectorConstruction::PlaceBeamlineInWorld(blSet.curvilinearWorld, clWorld,
						globals->CheckOverlaps(), false, true, true);
}
