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
#ifndef BDSPHYSICSXRAYREFLECTION_H
#define BDSPHYSICSXRAYREFLECTION_H

#include "G4Version.hh"
#if G4VERSION_NUMBER > 1119

#include "BDSSingleUse.hh"

#include "G4VPhysicsConstructor.hh"

/**
 * @brief Xray reflection.
 * 
 * Constructs Xray reflection process and attaches to the gamma.
 *
 */

class BDSPhysicsXrayReflection: public G4VPhysicsConstructor, public BDSSingleUse
{
public:
  explicit BDSPhysicsXrayReflection(G4double surfaceRoughnessIn = 0);
  BDSPhysicsXrayReflection() = delete;
  virtual ~BDSPhysicsXrayReflection();

  /// Construct gammas.
  virtual void ConstructParticle();

  /// Construct and attach the processes to the relevant particles.
  virtual void ConstructProcess();

private:
  G4double surfaceRoughness;
};
#endif

#endif
