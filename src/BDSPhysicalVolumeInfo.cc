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
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "G4LogicalVolume.hh"
#include "G4Types.hh"
#include "G4String.hh"
#include "G4VPhysicalVolume.hh"

#include <ostream>
#include <string>

BDSPhysicalVolumeInfo::BDSPhysicalVolumeInfo(G4double sPosAtMiddleOfElement):
  name("unknown"),
  placementName("unknown"),
  spos(sPosAtMiddleOfElement),
  beamlineIndex(-1),
  beamline(nullptr),
  beamlineMassWorld(nullptr),
  beamlineMassWorldIndex(-1)
{;}

BDSPhysicalVolumeInfo::BDSPhysicalVolumeInfo(G4String nameIn,
                                             G4String placementNameIn,
                                             G4double sPosIn,
                                             G4int    beamlineIndexIn,
					     BDSBeamline* beamlineIn):
  name(nameIn),
  placementName(placementNameIn),
  spos(sPosIn),
  beamlineIndex(beamlineIndexIn),
  beamline(beamlineIn),
  beamlineMassWorld(beamlineIn),
  beamlineMassWorldIndex(beamlineIndexIn)
{
  // Variables are initialised with beam line and index. Here, we update them by referece if needs be.
  BDSAcceleratorModel::Instance()->MassWorldBeamlineAndIndex(beamlineMassWorld, beamlineMassWorldIndex);
}

BDSPhysicalVolumeInfo::~BDSPhysicalVolumeInfo()
{;}

G4int BDSPhysicalVolumeInfo::RegisterSubElement(G4VPhysicalVolume* pv)
{
  if (!pv)
    {return -1;}
  auto search = subElementIDs.find(pv);
  if (search != subElementIDs.end())
    {return search->second;}
  G4int id = (G4int)subElementNames.size();
  subElementIDs[pv] = id;
  subElementNames.push_back(pv->GetName());
  return id;
}

G4int BDSPhysicalVolumeInfo::GetSubElementID(const G4VPhysicalVolume* pv) const
{
  auto search = subElementIDs.find(pv);
  return search != subElementIDs.end() ? search->second : -1;
}

G4String BDSPhysicalVolumeInfo::GetSubElementName(G4int id) const
{
  if (id < 0 || id >= (G4int)subElementNames.size())
    {return "";}
  return subElementNames[id];
}

void BDSPhysicalVolumeInfo::RegisterSubElementsRecursively(G4VPhysicalVolume* pv)
{
  if (!pv)
    {return;}
  RegisterSubElement(pv);
  G4LogicalVolume* lv = pv->GetLogicalVolume();
  for (G4int i = 0; i < (G4int)lv->GetNoDaughters(); i++)
    {RegisterSubElementsRecursively(lv->GetDaughter(i));}
}

void BDSPhysicalVolumeInfo::RegisterSubElementsFromMassWorld()
{
  if (!beamlineMassWorld || beamlineMassWorldIndex < 0 ||
      beamlineMassWorldIndex >= (G4int)beamlineMassWorld->size())
    {return;}
  BDSBeamlineElement* massElement = *(beamlineMassWorld->begin() + beamlineMassWorldIndex);
  // only the curvilinear volume built 1:1 for this element is named <element>_cl_<index>
  if (name != massElement->GetName() + "_cl_" + std::to_string(beamlineMassWorldIndex))
    {return;}
  const auto* massPVs = BDSPhysicalVolumeInfoRegistry::Instance()->PVsForBeamlineElement(massElement);
  if (!massPVs)
    {return;}
  for (auto pv : *massPVs)
    {RegisterSubElementsRecursively(pv);}
}

std::ostream &operator<<(std::ostream &out, BDSPhysicalVolumeInfo const &info)
{
  out << "Name: \"" << info.name << "\" S pos: " << info.spos << " mm Precision: ";
  return out;
}
