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
#include "BDSBeamline.hh"
#include "BDSBeamlineSet.hh"


void BDSBeamlineSet::DeleteContents()
{
  delete massWorld;
  delete curvilinearWorld;
  delete curvilinearBridgeWorld;
  delete endPieces;
}

const std::vector<G4ThreeVector> BDSBeamlineSet::GetMaximumExtentAbsolute() const
{
  std::vector<G4ThreeVector> extents;
  GetMaximumExtentAbsolute(extents);
  return extents;
}

void BDSBeamlineSet::GetMaximumExtentAbsolute(std::vector<G4ThreeVector>& extents) const
{
  if (massWorld)
    {extents.push_back(massWorld->GetMaximumExtentAbsolute());}
  if (curvilinearWorld)
    {extents.push_back(curvilinearWorld->GetMaximumExtentAbsolute());}
  if (curvilinearBridgeWorld)
    {extents.push_back(curvilinearBridgeWorld->GetMaximumExtentAbsolute());}
  if (endPieces)
    {extents.push_back(endPieces->GetMaximumExtentAbsolute());}
}

void BDSBeamlineSet::GetExtentGlobals(std::vector<BDSExtentGlobal>& extents) const
{
  if (massWorld)
    {extents.push_back(massWorld->GetExtentGlobal());}
  if (curvilinearWorld)
    {extents.push_back(curvilinearWorld->GetExtentGlobal());}
  if (curvilinearBridgeWorld)
    {extents.push_back(curvilinearBridgeWorld->GetExtentGlobal());}
  if (endPieces)
    {extents.push_back(endPieces->GetExtentGlobal());}
}
