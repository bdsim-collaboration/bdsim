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
#include "BDSSamplerPlacementRecord.hh"
#include "BDSSamplerType.hh"

#include "globals.hh" // geant4 types / globals

BDSSamplerPlacementRecord::BDSSamplerPlacementRecord(const G4String&           nameIn,
                                                     BDSSampler*               samplerIn,
                                                     const G4Transform3D&      transformIn,
                                                     G4double                  sPositionIn,
                                                     const BDSBeamlineElement* elementIn,
                                                     const G4String&           uniqueNameIn,
                                                     BDSSamplerType            typeIn,
                                                     G4double                  radiusIn):
  name(nameIn),
  sampler(samplerIn),
  transform(transformIn),
  transformInverse(transformIn.inverse()),
  sPosition(sPositionIn),
  element(elementIn),
  uniqueName(uniqueNameIn),
  type(typeIn),
  radius(radiusIn)
{
  if (uniqueName.empty())
    {uniqueName = name;}
}
