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
#ifndef BDSFIELDMANAGER_H
#define BDSFIELDMANAGER_H

#include "globals.hh"
#include "G4FieldManager.hh"

class G4ChordFinder;
class G4Field;
class G4Track;

/**
 * @brief Wrapper for Geant4's G4FieldManager to distinguish primaries.
 * 
 * This implements the ConfigureForTrack virtual method that by default
 * does nothing and flags the track as primary or not in BDSIntegratorMag.
 * 
 * @author Laurie Nevay
 */

class BDSFieldManager: public G4FieldManager
{
public:
  BDSFieldManager(G4Field*       field              = nullptr, 
		  G4ChordFinder* chordFinder        = nullptr, 
		  G4bool         fieldChangesEnergy = true);

  BDSFieldManager(G4MagneticField* field);

  virtual ~BDSFieldManager(){;}

  /// Check whether the track has parent ID == 0 and then flag
  /// as a primary.
  virtual void ConfigureForTrack(const G4Track* track);

private:
  G4bool currentTrackIsPrimary; ///< Whether current track is a primary.
};

#endif
