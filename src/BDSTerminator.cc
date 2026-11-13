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
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSDManager.hh"
#include "BDSTerminator.hh"
#include "BDSSDTerminator.hh"
#include "BDSTerminatorUserLimits.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"

#include <limits>

BDSTerminator::BDSTerminator(G4double horizontalWidthIn):
  BDSAcceleratorComponent("terminator", BDSSamplerPlane::ChordLength(), 0, "terminator"),
  horizontalWidth(horizontalWidthIn)
{;}

BDSTerminator::~BDSTerminator()
{;}

void BDSTerminator::Build()
{
  BuildContainerLogicalVolume();
}

void BDSTerminator::BuildContainerLogicalVolume()
{
  containerSolid = new G4Box(name + "_container_solid",
                             horizontalWidth * 0.5,
                             horizontalWidth * 0.5,
                             chordLength * 0.5);
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
                                               emptyMaterial,
                                               name + "_container_lv");

  // Make the terminator sensitive to count the turns of the primary particle
  containerLogicalVolume->SetSensitiveDetector(BDSSDManager::Instance()->Terminator());

  // Dynamic user limits - the logic of killing particles on last turn.
  // The numerical values are the default G4UserLimit values so everything will
  // normally be tracked. BDSTerminatorUserLimits has the logic inside it to
  // respond to turn number.
  BDSTerminatorUserLimits* termUL = new BDSTerminatorUserLimits(std::numeric_limits<double>::max(),
                                                                std::numeric_limits<double>::max(),
                                                                std::numeric_limits<double>::max(),
                                                                0.,0.);
  RegisterUserLimits(termUL);
  containerLogicalVolume->SetUserLimits(termUL);

  // visual attributes
  containerLogicalVolume->SetVisAttributes(containerVisAttr);

  // register extents with BDSGeometryComponent base class
  SetExtent(BDSExtent(0.5*horizontalWidth, 0.5*horizontalWidth, chordLength*0.5));
}
