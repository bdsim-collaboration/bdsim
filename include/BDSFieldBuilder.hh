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
#ifndef BDSFIELDBUILDER_H
#define BDSFIELDBUILDER_H

#include "globals.hh" //G4 global constants & types

#include <map>
#include <set>
#include <vector>

class BDSFieldInfo;
class BDSFieldObjects;
class BDSMagnetStrength;
class G4LogicalVolume;

/**
 * @brief Register for all fields to be built and volumes to be attached to.
 * 
 * This keeps a registry of the field info (the template for a field) and the 
 * volume to attach it to, so all fields can actually be constructed at one
 * time irrespective of when they're apparently 'created' in the geometry / 
 * individual BDSAcceleratorComponents.
 * 
 * This register does not retain ownership of anything so multiple logical volumes
 * may be safely registered along with a single BDSFieldInfo instance.
 *
 * @author Laurie Nevay
 */

class BDSFieldBuilder
{
public:
  /// Singleton pattern accessor.
  static BDSFieldBuilder* Instance();

  ~BDSFieldBuilder();

  /// Register a field for construction - ie the field specification along with
  /// which logical volume to attach it to. The same field specification (info)
  /// can be registered for multiple logical volumes as this builder does not
  /// retain ownership of anything.
  void RegisterFieldForConstruction(const BDSFieldInfo*      info,
				    G4LogicalVolume*         logicalVolume,
				    const G4bool             propagateToDaughters     = false,
				    const BDSMagnetStrength* magnetStrengthForScaling = nullptr,
				    const G4String&          scalingKey               = "none");

  /// Similar version but vectorised.
  void RegisterFieldForConstruction(const BDSFieldInfo*      info,
				    const std::vector<G4LogicalVolume*>& logicalVolumes,
				    const G4bool             propagateToDaughters     = false,
                                    const BDSMagnetStrength* magnetStrengthForScaling = nullptr,
				    const G4String&          scalingKey               = "none");

  /// Similar version but for a set.
  void RegisterFieldForConstruction(const BDSFieldInfo*      info,
				    const std::set<G4LogicalVolume*>& logicalVolumes,
				    const G4bool             propagateToDaughters     = false,
                                    const BDSMagnetStrength* magnetStrengthForScaling = nullptr,
				    const G4String&          scalingKey               = "none");

  std::vector<BDSFieldObjects*> CreateAndAttachAll();

private:
  /// Private default constructor to enforce singleton pattern.
  BDSFieldBuilder();

  /// Singleton instance.
  static BDSFieldBuilder* instance;
  
  /// @{ Register of components to build.
  std::vector<const BDSFieldInfo*> infos;
  std::vector<std::vector<G4LogicalVolume*> > lvs;
  std::vector<G4bool>              propagators;
  /// @}

  /// @{ Optional register of scaling strengths and keys.
  std::map<G4int, const BDSMagnetStrength*> scalingStrengths;
  std::map<G4int, G4String> scalingKeys;
  /// @}
};

#endif
