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
#ifndef BDSBUNCHLINK_H
#define BDSBUNCHLINK_H

#include "BDSBunch.hh"
#include "BDSParticleExternal.hh"

#include <vector>

class BDSParticleCoordsFull;
class BDSParticleDefinition;

/**
 * @brief A bunch distribution that holds a bunch from a Link.
 *
 * @author Stewart Boogert
 */

class BDSBunchLink: public BDSBunch
{
public: 
  BDSBunchLink();
  virtual ~BDSBunchLink();

  /// Get the next particle.
  virtual BDSParticleCoordsFull GetNextParticleLocal();

  /// Append particle to the bunch for tracking.
  void AddParticle(BDSParticleDefinition* particleDefinitionIn,
                   const BDSParticleCoordsFull& coordsIn,
                   int   externalParticleID,
                   int   externalParentID);

  /// Delete all particle objects in the bunch and clear the vector.
  void ClearParticles();

  /// @{ Accessor.
  inline size_t Size() const {return particles.size();}
  inline int    CurrentExternalParticleID() const {return currentExternalParticleID;}
  inline int    CurrentExternalParentID()   const {return currentExternalParentID;}
  /// @}

  /// Access the current particle definition of the current particle in the bunch.
  inline virtual const BDSParticleDefinition* ParticleDefinition() const {return currentParticleDefinition;}

  /// TBC
  void UpdateGeant4ParticleDefinition(G4int pdgID);

  /// Override method in BDSBunch to use our local currentParticleDefinition member.
  virtual void UpdateIonDefinition();
  
private:
  G4int currentIndex;
  G4int currentExternalParticleID;
  G4int currentExternalParentID;
  BDSParticleDefinition* currentParticleDefinition;

  G4int size;         ///< Number of particles (1 counting).
  std::vector<BDSParticleExternal*> particles;
};
#endif
