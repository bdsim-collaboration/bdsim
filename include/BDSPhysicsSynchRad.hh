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
#ifndef BDSSYNCHRADPHYSICS_H
#define BDSSYNCHRADPHYSICS_H

#include "BDSSingleUse.hh"

#include "globals.hh" // geant4 types / globals
#include "G4VPhysicsConstructor.hh"

/**
 * @brief A physics constructor that only constructs Synchrotron Radiation.
 */

class BDSPhysicsSynchRad: public G4VPhysicsConstructor, public BDSSingleUse
{
public:
  BDSPhysicsSynchRad();
  virtual ~BDSPhysicsSynchRad();

  /// Construct all leptons and the photon.
  virtual void ConstructParticle();
  
  /// Construct and attach G4SynchrotronRadiation to all charged particles.
  virtual void ConstructProcess();
};

#endif
