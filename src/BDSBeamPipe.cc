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
#include "BDSBeamPipe.hh"
#include "BDSUtilities.hh"

#include "globals.hh"         // geant4 globals / types
#include "G4VSolid.hh"

BDSBeamPipe::BDSBeamPipe(G4VSolid*        containerSolidIn,
			 G4LogicalVolume* containerLVIn,
			 BDSExtent        extentIn,
			 G4VSolid*        containerSubtractionSolidIn,
			 G4LogicalVolume* vacuumLVIn,
			 G4bool           containerIsCircularIn,
			 G4double         containerRadiusIn,
			 G4ThreeVector    inputFaceNormalIn,
			 G4ThreeVector    outputFaceNormalIn):
  BDSGeometryComponent(containerSolidIn, containerLVIn, extentIn),
  containerSubtractionSolid(containerSubtractionSolidIn),
  vacuumLogicalVolume(vacuumLVIn),
  containerIsCircular(containerIsCircularIn),
  containerRadius(containerRadiusIn),
  inputFaceNormal(inputFaceNormalIn.unit()),
  outputFaceNormal(outputFaceNormalIn.unit())
{;}

BDSBeamPipe::~BDSBeamPipe()
{
  // we leak containerSubtractionSolid as it's slow to delete lots
  // of them because of g4 deregistering them - g4 will clean up
  //delete containerSubtractionSolid;
}

std::set<G4LogicalVolume*> BDSBeamPipe::GetVolumesForField() const
{
  std::set<G4LogicalVolume*> result = GetAllLogicalVolumes(); // from base class
  // We avoid setting the field on a tight-fitting container volume to avoid strong
  // field in small gaps. However, in the case of only a container volume, we should
  // return that - for when there's no beam pipe geometry.
   if (result.size() > 1)
    {result.erase(GetContainerLogicalVolume());}
   else if (!BDS::IsFinite((G4double)result.size()))
     {result = {GetContainerLogicalVolume()};}

  return result;
}
