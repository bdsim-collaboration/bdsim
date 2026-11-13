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
#include "BDSAcceleratorComponent.hh"
#include "BDSColours.hh"
#include "BDSDump.hh"
#include "BDSExtent.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

#include "CLHEP/Units/SystemOfUnits.h"

BDSDump::BDSDump(G4String nameIn, 
		 G4double lengthIn,
		 G4double horizontalWidthIn,
		 G4bool   circularIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "dump"),
  horizontalWidth(horizontalWidthIn),
  circular(circularIn)
{;}

BDSDump::~BDSDump()
{;}

void BDSDump::Build()
{
  // default construction
  BDSAcceleratorComponent::Build();

  // update container visualisation attributes
  G4Colour* reallyreallydarkgrey = BDSColours::Instance()->GetColour("reallyreallydarkgrey");
  G4VisAttributes* dumpVis = new G4VisAttributes(*reallyreallydarkgrey);
  RegisterVisAttributes(dumpVis);
  containerLogicalVolume->SetVisAttributes(dumpVis);
}

void BDSDump::BuildContainerLogicalVolume()
{
  if (circular)
    {
      containerSolid = new G4Tubs(name + "_solid",
				  0,
				  0.5*horizontalWidth,
				  0.5*chordLength - 2*lengthSafetyLarge,
				  0,
				  CLHEP::twopi);
    }
  else
    {
      containerSolid = new G4Box(name + "_solid",
				 0.5*horizontalWidth,
				 0.5*horizontalWidth,
				 0.5*chordLength - 2*lengthSafetyLarge);
    }
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_lv");
  
  BDSExtent ext(0.5*horizontalWidth, 0.5*horizontalWidth, 0.5*chordLength);
  SetExtent(ext);
}

void BDSDump::BuildUserLimits()
{
  userLimits = new G4UserLimits(1, // max 1mm step into dump
				 0, // max track length
				 0, // max time
				 0, // max kinetic energy
				 std::numeric_limits<double>::max());

  RegisterUserLimits(userLimits);
}
