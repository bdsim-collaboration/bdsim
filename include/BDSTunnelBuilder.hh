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
#ifndef BDSTUNNELBUILDER_H
#define BDSTUNNELBUILDER_H

#include "globals.hh" // geant4 types / globals

#include "BDSBeamline.hh"

/**
 * @brief A class that constructs tunnel segments around a beamline. 
 *
 * It uses a beam line instance - the flat beam line
 * and creates a new beam line of tunnel segments that can then be placed.
 * 
 * @author Laurie Nevay
 */

class BDSTunnelBuilder
{
public:
  explicit BDSTunnelBuilder(G4double tunnelMaxSegmentLengthIn = 50*CLHEP::m);
  ~BDSTunnelBuilder();

  /// Main function of class - use tunnel factories and builds sections
  /// of tunnel around the flat beamline (of accelerator components).
  /// Tunnel segments are placed in order in a new beamline instance,
  /// which is returned.
  BDSBeamline* BuildTunnelSections(const BDSBeamline* flatBeamLine) const;

private:
  /// Decide whether this is a good point to break the tunnel based on
  /// various cumulative parameters.
  G4bool BreakTunnel(BDSBeamline::const_iterator proposedStart,
                     BDSBeamline::const_iterator proposedEnd,
		     const G4double& halfWidth) const;

  /// Maximum distance tolerable that the beamline can deviate from the previous
  /// tunnel break point before another tunnel break point is made.
  G4double displacementTolerance;

  /// Maximum number of items before split.
  G4double maxItems;

  /// Maximum length before split.
  G4double maxLength;

  /// Maximum angle before split.
  G4double maxAngle;

  /// Minimum length to angle ratio to allow a split.
  G4double minLength;
};


#endif
