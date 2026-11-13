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
#include "BDSNavigatorPlacements.hh"

#include "G4AffineTransform.hh"
#include "G4Navigator.hh"
#include "G4ThreeVector.hh"

#include <utility>

G4Navigator*       BDSNavigatorPlacements::navigator         = new G4Navigator();
G4int              BDSNavigatorPlacements::numberOfInstances = 0;
G4VPhysicalVolume* BDSNavigatorPlacements::worldPV           = nullptr;

BDSNavigatorPlacements::BDSNavigatorPlacements():
  globalToLocal(G4AffineTransform()),
  localToGlobal(G4AffineTransform())
{
  numberOfInstances++;
}

BDSNavigatorPlacements::~BDSNavigatorPlacements()
{
  // Only delete static navigator objects when last instance is deleted
  if (numberOfInstances == 1)
    {
      delete navigator; navigator   = nullptr;
    }
  numberOfInstances--;
}

void BDSNavigatorPlacements::ResetNavigatorStates()
{
  navigator->ResetStackAndState();
}

G4ThreeVector BDSNavigatorPlacements::ConvertToLocal(const G4ThreeVector& globalPosition,
						     G4bool& foundAPlacementVolume) const
{
  foundAPlacementVolume = InitialiseTransform(globalPosition);
  if (!foundAPlacementVolume)
    {return G4ThreeVector();}
  return globalToLocal.TransformPoint(globalPosition);
}

G4ThreeVector BDSNavigatorPlacements::ConvertToLocalNoSetup(const G4ThreeVector& globalPosition) const
{
  return globalToLocal.TransformPoint(globalPosition);
}

G4ThreeVector BDSNavigatorPlacements::ConvertAxisToGlobal(const G4ThreeVector& localAxis) const
{
  return localToGlobal.TransformAxis(localAxis);
}

std::pair<G4ThreeVector, G4ThreeVector> BDSNavigatorPlacements::ConvertAxisToGlobal(const std::pair<G4ThreeVector, G4ThreeVector>& localAxis) const
{
  G4ThreeVector globalB = localToGlobal.TransformAxis(localAxis.first);
  G4ThreeVector globalE = localToGlobal.TransformAxis(localAxis.second);
  return std::make_pair(globalB, globalE);
}

G4bool BDSNavigatorPlacements::InitialiseTransform(const G4ThreeVector& globalPosition) const
{
  G4VPhysicalVolume* foundPVVolume = navigator->LocateGlobalPointAndSetup(globalPosition);
  if (foundPVVolume == worldPV)
    {return false;}
  globalToLocal = navigator->GetGlobalToLocalTransform();
  localToGlobal = navigator->GetLocalToGlobalTransform();
  return true; // found a placement volume ok
}
