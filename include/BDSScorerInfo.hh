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
#ifndef BDSSCORERINFO_H
#define BDSSCORERINFO_H

#include "BDSScorerType.hh"

#include "globals.hh"         // geant4 types / globals

#include <vector>

namespace GMAD
{
  class Scorer;
  class ScorerMesh;
}
class G4Material;
class G4ParticleDefinition;

/**
 * @brief Recipe class for scorer. Checks values.
 *
 * Convert a parser scorer instance to one using Geant4 types
 * and units. Check values and find particle definition.
 *
 * @author Robin Tesse
 */

class BDSScorerInfo
{
public:
  /// No default constructor as unused.
  BDSScorerInfo() = delete;

  /// Constructor. If upgradeTo3D is true, some scorer types will
  /// be upgraded to 3d as required for use in a mesh.
  explicit BDSScorerInfo(const GMAD::Scorer& scorer,
			 G4bool upgradeTo3D = false);

  /// Utility function to check valid pointer and throw exception if not.
  void CheckParticle(G4ParticleDefinition* particleIn,
		     const G4String& nameIn);
  
  BDSScorerType scorerType;          ///< Scorer type.
  G4String      name;                ///< Scorer name.
  G4ParticleDefinition* particle;    ///< Particle filter
  G4double      minimumKineticEnergy;
  G4double      maximumKineticEnergy;
  G4double      minimumTime;
  G4double      maximumTime;
  G4String      filename;            ///< Name of the conversion factor file
  G4String      pathname;            ///< Path of the conversion factor file (for ambient dose)
  std::vector<G4Material*> materialsToInclude; /// Which materials to include for scoring.
  std::vector<G4Material*> materialsToExclude; /// Which materials to exclude for scoring.
  G4bool        worldVolumeOnly;     ///< Whether to score from the world volume only
  G4bool        primariesOnly;
};

#endif
