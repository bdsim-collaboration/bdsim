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
#ifndef BDSPARTICLEEXTERNAL_H
#define BDSPARTICLEEXTERNAL_H 
#include "BDSParticleCoordsFull.hh"

#include "G4Types.hh"

class BDSParticleDefinition;

/**
 * @brief A set of particle coordinates from an external interface.
 * 
 * @author Laurie Nevay
 */

class BDSParticleExternal
{
public:
  /// Remove default constructor.
  BDSParticleExternal() = delete;
  BDSParticleExternal(BDSParticleDefinition*       particleDefinitionIn,
		      const BDSParticleCoordsFull& coordsIn,
		      G4int                        externalParticleIDIn,
		      G4int                        externalParentIDIn);
  /// @{ Define copy, move and move-assignment constructor as this class owns the particleDefinition.
  BDSParticleExternal(const BDSParticleExternal& other);
  BDSParticleExternal(BDSParticleExternal&& other) noexcept;
  BDSParticleExternal& operator=(BDSParticleExternal&& other) noexcept;
  /// @}
  ~BDSParticleExternal();

  BDSParticleDefinition* particleDefinition;
  BDSParticleCoordsFull  coords;
  G4int                  externalParticleID;
  G4int                  externalParentID;
};

#endif
