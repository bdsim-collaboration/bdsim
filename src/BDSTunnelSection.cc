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
#include "BDSTunnelSection.hh"

#include "G4VSolid.hh"

BDSTunnelSection::BDSTunnelSection(G4String              nameIn,
				   G4double              chordLengthIn,
				   G4double              angleIn,
				   BDSGeometryComponent* tunnelGeometry,
				   G4VSolid*             innerIntersectionSolidIn):
  BDSAcceleratorComponent(nameIn, chordLengthIn, angleIn, "tunnel"),
  innerIntersectionSolid(innerIntersectionSolidIn)
{
  // chordLength is calculated from the angle and the arcLength by BDSAcceleratorComponent
  // but here we only have chord length.  Just reassign chord length.
  chordLength = chordLengthIn;

  // copy special bits from BDSGeometryComponent.  Can't use BDSGeometryComponent in
  // initialiser list as c++ only allows you to use constructors one up in the inheritance.
  RegisterDaughter(tunnelGeometry);
  InheritExtents(tunnelGeometry);
  containerSolid         = tunnelGeometry->GetContainerSolid();
  containerLogicalVolume = tunnelGeometry->GetContainerLogicalVolume();
}

BDSTunnelSection::~BDSTunnelSection()
{
  delete innerIntersectionSolid;
}

void BDSTunnelSection::Initialise()
{;}

void BDSTunnelSection::BuildContainerLogicalVolume()
{;}
