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
#include "BDSProcessUserSpecialCutsPDG.hh"

#include "G4Types.hh"

#include <limits>
#include <set>

BDSProcessUserSpecialCutsPDG::BDSProcessUserSpecialCutsPDG():
  G4UserSpecialCuts("BDSUserSpecialCutsPDGID")
{;}

BDSProcessUserSpecialCutsPDG::BDSProcessUserSpecialCutsPDG(const std::set<G4int>& pdgIDsIn):
  G4UserSpecialCuts("BDSUserSpecialCutsPDGID"),
  pdgIDs(pdgIDsIn)
{;}

BDSProcessUserSpecialCutsPDG::~BDSProcessUserSpecialCutsPDG()
{;}

G4double BDSProcessUserSpecialCutsPDG::PostStepGetPhysicalInteractionLength(const G4Track&    track,
									 G4double          previousStepSize,
									 G4ForceCondition* condition)
{
  G4int pdgID = track.GetDynamicParticle()->GetPDGcode();
  if (pdgIDs.count(pdgID) > 0)
    {
      *condition = NotForced;
      return std::numeric_limits<double>::max();
    }
  else
    {return G4UserSpecialCuts::PostStepGetPhysicalInteractionLength(track, previousStepSize, condition);}
}
