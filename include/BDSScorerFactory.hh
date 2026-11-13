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
#ifndef BDSSCORERFACTORY_H
#define BDSSCORERFACTORY_H

#include "BDSScorerType.hh"

#include "G4String.hh"
#include "G4Types.hh"

class BDSHistBinMapper;
class BDSScorerInfo;
class BDSSDFilterAnd;

class G4LogicalVolume;
class G4Material;
class G4VPrimitiveScorer;

/**
 * @brief Create primitive scorers on demand.
 *
 * @author Robin Tesse
 */

class BDSScorerFactory
{
public:
  BDSScorerFactory();
  ~BDSScorerFactory(){;}
  
  /// Main function to create a scorer.
  G4VPrimitiveScorer* CreateScorer(const BDSScorerInfo*    info,
				   const BDSHistBinMapper* mapper,
				   G4double*               unit    = nullptr,
				   G4LogicalVolume*        worldLV = nullptr);
  
private:  
  ///@{ Unused default constructors.
  BDSScorerFactory(const BDSScorerFactory&) = delete;
  BDSScorerFactory& operator=(const BDSScorerFactory&) = delete;
  ///@}

  /// Construct the primitive scorer required.
  G4VPrimitiveScorer* GetAppropriateScorer(const BDSScorerInfo& info,
					   const BDSHistBinMapper* mapper,
					   G4double*                 unit = nullptr);

  /// Create a combined filter with AND logic for the scorer.
  BDSSDFilterAnd* CreateFilter(const G4String&      name,
			       const BDSScorerInfo* info,
			       G4LogicalVolume*     worldLV = nullptr) const;
};

#endif
