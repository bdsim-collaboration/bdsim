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
#include "BDSPhysicalVolumeInfo.hh"
#include "G4Types.hh"
#include "G4String.hh"

#include <ostream>

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

std::ostream &operator<<(std::ostream &out, BDSPhysicalVolumeInfo const &info)
{
  out << "Name: \"" << info.name << "\" S pos: " << info.spos << " mm Precision: ";
  return out;
}
