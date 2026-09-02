/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#ifndef BDSPHYSICSCHANNELLING_H
#define BDSPHYSICSCHANNELLING_H

#include "G4Version.hh"
#if G4VERSION_NUMBER > 1039

#include "BDSSingleUse.hh"

#include "G4VPhysicsConstructor.hh"

/**
 * @brief Channelling physics process.
 * 
 * Constructs G4Channeling physics process and attaches to all charged particles.
 * Completely based on the G4ChannelingPhysics.cc in examples/extended/exoticphysics/channeling/src.
 *
 * Uses the G4PhysicsListHelper to set and order processes.
 * 
 * @author Laurie Nevay
 */

class BDSPhysicsChannelling: public G4VPhysicsConstructor, public BDSSingleUse
{
public:
  explicit BDSPhysicsChannelling(G4bool disableCoulombScatteringIn = false);
  virtual ~BDSPhysicsChannelling(){;}
  virtual void ConstructParticle();

  /// Construct and attach the processes to the relevant particles.
  virtual void ConstructProcess();

private:
  G4bool disableCoulombScattering;
};
#endif

#endif
