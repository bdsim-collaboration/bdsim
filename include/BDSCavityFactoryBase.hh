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
#ifndef BDSCAVITYFACTORYBASE_H
#define BDSCAVITYFACTORYBASE_H

#include "BDSFactoryBase.hh"
#include "BDSSDType.hh"

#include "globals.hh"
#include "G4String.hh"
#include "G4RotationMatrix.hh"

#include <map>
#include <set>

class BDSCavity;
class BDSCavityInfo;
class BDSExtent;

class G4LogicalVolume;
class G4Material;
class G4UserLimits;
class G4VisAttributes;
class G4VPhysicalVolume;
class G4VSolid;

/**
 * @brief Abstract base class for rf cavity geometry factories.
 *
 * @author Laurie Nevay
 */

class BDSCavityFactoryBase: public BDSFactoryBase
{
public:
  virtual ~BDSCavityFactoryBase(){;}

  /// Method that calls protected virtual methods in order. Sets general
  /// recipe of construction - in order below.
  BDSCavity* CreateCavity(const G4String&      name,
			  G4double             totalChordLength,
			  const BDSCavityInfo* info,
			  G4Material*          vacuumMaterial);
  
protected:
  BDSCavityFactoryBase();

  /// Ensure all internal members are reset before reuse. Calls CleanUpBase.
  virtual void CleanUp();

  /// Non-virtual clean up that can be used in the constructor safely.
  void CleanUpBase();

  /// Create vacuumSolid and cavitySolid. Must return the container radius.
  virtual G4double CreateSolids(const G4String&      name,
				G4double             totalChordLength,
				const BDSCavityInfo* info) = 0;

  /// Create vacuumLV, cavityLV and containerLV.
  virtual void CreateLogicalVolumes(const G4String&      name,
				    const BDSCavityInfo* info,
				    G4Material*          vacuumMaterial);

  /// If the length is shorter than the default user limits in global constants,
  /// create a new user limits object and apply to all lvs in allLogicalVolumes.
  virtual void SetUserLimits(G4double length);

  /// Set vis attributes for cavityLV, vacuumLV and containerLV.
  virtual void SetVisAttributes(const G4String& colourName = "rfcavity");

  /// Place the vacuum and cavity centrally in the container logical volume.
  virtual void PlaceComponents(const G4String& name);

  /// Construct output object and register all temporary objects from vectors.
  virtual BDSCavity* BuildCavityAndRegisterObjects(const BDSExtent& extent);
  
  std::map<G4LogicalVolume*, BDSSDType> sensitiveVolumes; ///< Map to sensitivity for volumes.

  /// @{ Cache of particular solid or lv for common functionality in this class.
  G4VSolid*        vacuumSolid;
  G4VSolid*        cavitySolid; 
  G4VSolid*        containerSolid;
  G4LogicalVolume* vacuumLV;
  G4LogicalVolume* cavityLV;
  G4LogicalVolume* containerLV;
  /// @}

  /// @{ Cache of variable from BDSGlobalConstants.
  G4Material* emptyMaterial;
  /// @}
  G4bool      sensitiveBeamPipe;  ///< Whether the beam pipe will record energy deposition.
  G4bool      sensitiveVacuum;    ///< Whether the vacuum will record energy deposition.
};

#endif
