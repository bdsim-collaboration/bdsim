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
#include "BDSExtent.hh"
#include "BDSMaterials.hh"
#include "BDSTunnelInfo.hh"
#include "BDSTunnelType.hh"

#include "CLHEP/Units/SystemOfUnits.h"

BDSTunnelInfo::BDSTunnelInfo(BDSTunnelType typeIn,
			     G4double      thicknessIn,
			     G4double      soilThicknessIn,
			     G4Material*   materialIn,
			     G4Material*   soilMaterialIn,
			     G4bool        buildFloorIn,
			     G4double      floorOffsetIn,
			     G4double      aper1In,
			     G4double      aper2In,
			     G4bool        sensitiveIn,
			     G4bool        visibleIn):
  type(typeIn), thickness(thicknessIn),
  soilThickness(soilThicknessIn), material(materialIn),
  soilMaterial(soilMaterialIn), buildFloor(buildFloorIn),
  floorOffset(floorOffsetIn), aper1(aper1In), aper2(aper2In),
  sensitive(sensitiveIn), visible(visibleIn)
{;}

BDSTunnelInfo::BDSTunnelInfo(G4String typeIn,
			     G4double thicknessIn,
			     G4double soilThicknessIn,
			     G4String materialIn,
			     G4String soilMaterialIn,
			     G4bool   buildFloorIn,
			     G4double floorOffsetIn,
			     G4double aper1In,
			     G4double aper2In,
			     G4bool   sensitiveIn,
			     G4bool   visibleIn):
  thickness(thicknessIn), soilThickness(soilThicknessIn),
  buildFloor(buildFloorIn), floorOffset(floorOffsetIn),
  aper1(aper1In), aper2(aper2In),
  sensitive(sensitiveIn), visible(visibleIn)
{
  type         = BDS::DetermineTunnelType(typeIn);
  material     = BDSMaterials::Instance()->GetMaterial(materialIn);
  soilMaterial = BDSMaterials::Instance()->GetMaterial(soilMaterialIn);
}

BDSExtent BDSTunnelInfo::IndicativeExtent() const
{
  G4double maxParam = std::max(aper1, aper2);
  maxParam += thickness + soilThickness + 1*CLHEP::cm;

  BDSExtent result = BDSExtent(-maxParam, maxParam,
			       -maxParam, maxParam,
			       0,0);
  return result;
}
