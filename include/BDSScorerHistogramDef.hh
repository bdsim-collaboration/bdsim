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
#ifndef BDSSCORERHISTOGRAMDEF_H
#define BDSSCORERHISTOGRAMDEF_H

#include "BDSHistBinMapper.hh"
#include "BDSScorerMeshInfo.hh"

#include "globals.hh"         // geant4 types / globals

/**
 * @brief Definition for a scorer histogram.
 * 
 * Information kept from when a scoring mesh is created
 * for passing to the output.
 * 
 * @author Laurie Nevay
 */

class BDSScorerHistogramDef: public BDSScorerMeshInfo
{
public:
  BDSScorerHistogramDef(const BDSScorerMeshInfo& meshInfo,
			const G4String&          uniqueNameIn,
			const G4String&          primitiveScorerNameIn,
			G4double                 primitiveScorerUnitValueIn,
			const BDSHistBinMapper&  coordinateMapperIn);

  virtual ~BDSScorerHistogramDef(){;}

  G4String uniqueName; ///< Unique name of mesh/scorer -> slash required by Geant4.
  G4String outputName; ///< Copy of unique name that's safe for output.
  G4String primitiveScorerName;        ///< Primitive scorer name used to look up units.
  G4double primitiveScorerUnitValue;   ///< Numerical factor to use for scorer units.
  BDSHistBinMapper coordinateMapper; ///< Coordinate mapper - this class owns it.

private:
  /// No default constructor required.
  BDSScorerHistogramDef() = delete;
};

#endif
