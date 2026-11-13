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
#include "BDSSDFilterPDGIDSet.hh"

#include "G4ParticleDefinition.hh"
#include "G4Step.hh"
#include "G4Track.hh"

#include <set>

BDSSDFilterPDGIDSet::BDSSDFilterPDGIDSet(const G4String& name,
                                         const std::set<G4int>& PDGIDsToAcceptIn):
  G4VSDFilter(name),
  PDGIDsToAccept(PDGIDsToAcceptIn)
{;}

G4bool BDSSDFilterPDGIDSet::Accept(const G4Step* step) const
{
  auto pdgID = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
  return PDGIDsToAccept.count(pdgID) > 0; // ie in the set
}
