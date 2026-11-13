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
#include "BDSDebug.hh"
#include "BDSFieldManager.hh"
#include "BDSIntegratorMag.hh"
#ifdef BDSDEBUG
#include "BDSUtilities.hh"
#endif

#include "G4Track.hh"

BDSFieldManager::BDSFieldManager(G4Field*       field, 
				 G4ChordFinder* chordFinder, 
				 G4bool         fieldChangesEnergy):
  G4FieldManager(field, chordFinder, fieldChangesEnergy),
  currentTrackIsPrimary(false)
{;}

BDSFieldManager::BDSFieldManager(G4MagneticField* field):
  G4FieldManager(field),
  currentTrackIsPrimary(false)
{;}

void BDSFieldManager::ConfigureForTrack(const G4Track* track)
{
  currentTrackIsPrimary = track->GetParentID() == 0;
  BDSIntegratorMag::currentTrackIsPrimary = currentTrackIsPrimary;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Primary? " << BDS::BoolToString(currentTrackIsPrimary) << G4endl;
#endif
}
