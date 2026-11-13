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
#ifndef BDSREGION_H
#define BDSREGION_H

#include "globals.hh"

#include <ostream>

class G4ProductionCuts;
class G4Region;

namespace GMAD
{
  class Region;
}

/**
 * @brief Range cuts for a region. Help with defaults.
 *
 * @author Laurie Nevay
 */

class BDSRegion
{
public:
  explicit BDSRegion(G4String nameIn);
  BDSRegion(G4String nameIn,
	    const BDSRegion* defaultValue,
	    G4double defaultRangeCutIn,
	    G4double rangeCutElectronsIn,
	    G4double rangeCutPositronsIn,
	    G4double rangeCutProtonsIn,
	    G4double rangeCutPhotonsIn);
  BDSRegion(const GMAD::Region& parserRegion,
	    const BDSRegion*    defaultRegion);
	    
  ~BDSRegion();
  
  /// @{ Public members for simplicity.
  G4String name;
  G4double defaultRangeCut;
  G4double rangeCutElectrons;
  G4double rangeCutPositrons;
  G4double rangeCutProtons;
  G4double rangeCutPhotons;
  G4ProductionCuts* g4cuts;
  G4Region*         g4region;
  /// @}

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSRegion const &r);

private:
  /// Function to create cuts and region.
  void ProduceG4Region();
};

#endif
