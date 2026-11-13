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
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSGeometryDriver.hh"

BDSGeometryDriver::BDSGeometryDriver(){
}

BDSGeometryDriver::BDSGeometryDriver(G4String geomFile, G4LogicalVolume* markerVol, G4double markerLength):
  itsGeomFile(geomFile), itsMarkerLength(markerLength), itsMarkerVol(markerVol){
}

BDSGeometryDriver::~BDSGeometryDriver(){
}

void BDSGeometryDriver::Construct()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
}
 
