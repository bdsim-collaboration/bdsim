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
#include "BDSGeometry.hh"
#include "BDSGeometryType.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 globals / types

BDSGeometry::BDSGeometry()
{
  format = BDSGeometryType::gdml;
  file   = "";
  Init();
}

BDSGeometry::BDSGeometry(BDSGeometryType formatIn, G4String fileIn):
  format(formatIn),
  file(fileIn)
{
  containingDir = BDS::GetFullPath(fileIn,true); // strip of the file name effictively
  Init();
}

BDSGeometry::~BDSGeometry()
{;} // doesn't seem to own anything

void BDSGeometry::Init()
{
  alignInVolume  = nullptr;
  alignOutVolume = nullptr;
  length         = 0;
  field          = nullptr;
}
