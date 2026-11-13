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
#ifndef BDSPHYSICSMUONINELASTIC_H
#define BDSPHYSICSMUONINELASTIC_H

#include "BDSSingleUse.hh"

#include "G4VPhysicsConstructor.hh"

/**
 * @brief Only nuclear interactions for mu+-.
 * 
 *
 * Uses the G4PhysicsListHelper to set and order processes.
 *
 * @author Laurie Nevay
 */

class BDSPhysicsMuonInelastic: public G4VPhysicsConstructor, public BDSSingleUse
{
public:
  BDSPhysicsMuonInelastic();
  virtual ~BDSPhysicsMuonInelastic();

  /// Construct all particles (could be produced from interaction).
  virtual void ConstructParticle();

  /// Construct and attach the process for muons.
  virtual void ConstructProcess();
};
#endif
