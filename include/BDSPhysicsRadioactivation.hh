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
#ifndef BDSRADIOACTIVATIONPHYSICS_H
#define BDSRADIOACTIVATIONPHYSICS_H

#include "G4Version.hh"
#if G4VERSION_NUMBER > 1039

#include "BDSSingleUse.hh"

#include "G4Types.hh"
#include "G4VPhysicsConstructor.hh"

/**
 * @brief Radioactivation processes.
 * 
 * @author Ben Ledroit.
 */

class BDSPhysicsRadioactivation: public G4VPhysicsConstructor, public BDSSingleUse
{
public:
  explicit BDSPhysicsRadioactivation(G4bool atomicRearrangementIn = false);
  virtual ~BDSPhysicsRadioactivation();
  
  // This is a dummy method for physics
  virtual void ConstructParticle();
  
  // This method will be invoked in the Construct() method.
  // each physics process will be instantiated and
  // registered to the process manager of each particle type
  virtual void ConstructProcess();

protected:
  G4bool atomicRearrangement;
};
#endif

#endif
