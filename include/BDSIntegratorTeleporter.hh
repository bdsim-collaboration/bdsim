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
#ifndef BDSINTEGRATORTELEPORTER_H
#define BDSINTEGRATORTELEPORTER_H

#include "BDSIntegratorMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

class G4Mag_EqRhs;
class BDSPTCOneTurnMap;

/**
 * @brief Custom unphysical integrator to advance particle in teleporter.
 *
 * @author Laurie Nevay
 */

class BDSIntegratorTeleporter: public BDSIntegratorMag
{
public:
  BDSIntegratorTeleporter(G4Mag_EqRhs*      eqOfMIn,
			  G4Transform3D     teleporterDeltaIn,
			  G4double          teleporterLengthIn,
			  BDSPTCOneTurnMap* oneTurnMapIn=nullptr);

  virtual ~BDSIntegratorTeleporter();

  virtual void Stepper(const G4double yIn[],
		       const G4double dydx[],
		       const G4double h,
		       G4double       yOut[],
		       G4double       yErr[]);

private:

  const G4Transform3D     transform;
  const G4ThreeVector     dPos;
  const G4double          teleporterLength;
  BDSPTCOneTurnMap*       oneTurnMap;
  G4bool                  threeDMethod;  ///< Whether to use full 3D transform.
};

#endif
