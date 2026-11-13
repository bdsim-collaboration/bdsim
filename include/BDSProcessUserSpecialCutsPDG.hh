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
#ifndef BDSPROCESSUSERSPECIALCUTSPDG_HH
#define BDSPROCESSUSERSPECIALCUTSPDG_HH

#include "G4Types.hh"
#include "G4UserSpecialCuts.hh"

#include <set>

class G4Step;
class G4Track;

/**
 * @brief Apply regular cuts but not to a specified set of PDG IDs.
 * 
 * @author Laurie Nevay
 */

class BDSProcessUserSpecialCutsPDG: public G4UserSpecialCuts
{
public:
  BDSProcessUserSpecialCutsPDG();
  explicit BDSProcessUserSpecialCutsPDG(const std::set<G4int>& pdgIDsIn);
  virtual ~BDSProcessUserSpecialCutsPDG();
  
  virtual G4double PostStepGetPhysicalInteractionLength(const G4Track&    track,
							G4double          previousStepSize,
							G4ForceCondition* condition);
  
  BDSProcessUserSpecialCutsPDG(BDSProcessUserSpecialCutsPDG&) = delete;
  BDSProcessUserSpecialCutsPDG& operator=(const BDSProcessUserSpecialCutsPDG&) = delete;

private:
  std::set<G4int> pdgIDs;
};

#endif

