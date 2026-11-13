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
#include "BDSHistBinMapper.hh"
#include "BDSScorerHistogramDef.hh"

#include "G4String.hh"
#include "G4Types.hh"

#include <algorithm>

BDSScorerHistogramDef::BDSScorerHistogramDef(const BDSScorerMeshInfo& mesh,
					     const G4String&          uniqueNameIn,
					     const G4String&          primitiveScorerNameIn,
					     G4double                 primitiveScorerUnitValueIn,
					     const BDSHistBinMapper&  coordinateMapperIn):
  BDSScorerMeshInfo(mesh),
  uniqueName(uniqueNameIn),
  primitiveScorerName(primitiveScorerNameIn),
  primitiveScorerUnitValue(primitiveScorerUnitValueIn),
  coordinateMapper(coordinateMapperIn)
{
  outputName = uniqueNameIn;
  std::replace(outputName.begin(), outputName.end(), '/', '-');
}
