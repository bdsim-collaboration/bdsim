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
#include "BDSParticleCoordsFull.hh"
#include "BDSParticleDefinition.hh"
#include "BDSParticleExternal.hh"

#include "G4Types.hh"

BDSParticleExternal::BDSParticleExternal(BDSParticleDefinition* particleDefinitionIn,
					 const BDSParticleCoordsFull& coordsIn,
					 G4int                        externalParticleIDIn,
					 G4int                        externalParentIDIn):
  particleDefinition(particleDefinitionIn),
  coords(coordsIn),
  externalParticleID(externalParticleIDIn),
  externalParentID(externalParentIDIn)
{;}

BDSParticleExternal::BDSParticleExternal(const BDSParticleExternal& other):
  particleDefinition(new BDSParticleDefinition(*other.particleDefinition)),
  coords(other.coords),
  externalParticleID(other.externalParticleID),
  externalParentID(other.externalParentID)
{;}

BDSParticleExternal::BDSParticleExternal(BDSParticleExternal&& other) noexcept:
  coords(other.coords),
  externalParticleID(other.externalParticleID),
  externalParentID(other.externalParentID)
{
  particleDefinition = other.particleDefinition;
  other.particleDefinition = nullptr;
}

BDSParticleExternal& BDSParticleExternal::operator=(BDSParticleExternal&& other) noexcept
{
  if (this != &other)
    {
      delete particleDefinition;
      particleDefinition = other.particleDefinition;
      other.particleDefinition = nullptr;

      coords = other.coords;
      externalParticleID = other.externalParticleID;
      externalParentID   = other.externalParentID;
    }
  return *this;
}

BDSParticleExternal::~BDSParticleExternal()
{
  delete particleDefinition;
}
