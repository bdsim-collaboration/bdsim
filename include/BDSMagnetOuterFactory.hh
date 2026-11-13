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
#ifndef BDSMAGNETOUTERFACTORY_H
#define BDSMAGNETOUTERFACTORY_H

#include "BDSMagnetOuter.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSMagnetType.hh"

#include "globals.hh"           // geant4 globals / types

class BDSBeamPipe;
class BDSFieldInfo;
class BDSMagnetOuterFactoryBase;
class BDSMagnetOuterInfo;
class G4Material;

/**
 * @brief The main interface for using the magnet outer factories.
 * 
 * Each function in this class mirrors one in BDSMagnetOuterFactoryBase 
 * but with the addition of a BDSMagnetGeometryType enum as an argument. 
 * This factory dispatches the call to the correct factory and should 
 * be the only one used by the caller.
 * 
 * @author Laurie Nevay
 */

class BDSMagnetOuterFactory
{
public:
  /// Singleton accessor
  static BDSMagnetOuterFactory* Instance();
  
  ~BDSMagnetOuterFactory();

  /// Main interface to creating a magnet outer piece of geometry. Specified by magnet type,
  /// the recipe, the length of the magnet outer section, the length of the appropriately fitting
  /// container volume that's also constructed and w.r.t. an already constructed beam pipe.
  BDSMagnetOuter* CreateMagnetOuter(BDSMagnetType       magnetType,
				    BDSMagnetOuterInfo* outerInfo,
				    G4double            outerLength,
				    G4double            chordLength,
				    BDSBeamPipe*        beampipe);
  
private:
  BDSMagnetOuterFactory();
  static BDSMagnetOuterFactory* instance; ///< Singleton instance.

  /// Get the appropriate derived factory for the required magnet style.
  BDSMagnetOuterFactoryBase* GetAppropriateFactory(BDSMagnetGeometryType magnetTypeIn);

  /// Create the magnet yoke from externally provided geometry and create a
  /// suitable magnet container solid.
  BDSMagnetOuter* CreateExternal(const G4String&     name,
				 BDSMagnetOuterInfo* info,
				 G4double            length,
				 G4double            magnetContainerLength,
				 BDSBeamPipe*        beampipe);

  /// Create a container based on an external geometry component. Exists here as the
  /// using the external factory exists here and doesn't belong in a derived factory.
  /// Uses the beam pipe instance to get the surface normals as the container must match.
  /// Here, length is the full length of the final object desired.
  BDSGeometryComponent* CreateContainerForExternal(const G4String&      name,
						   G4double             length,
						   BDSGeometryExternal* external,
						   BDSBeamPipe*         beampipe);

  void CheckOuterBiggerThanBeamPipe(const G4String&           name,
				    const BDSMagnetOuterInfo* outerInfo,
				    const BDSBeamPipe*        beamPipe) const;
  
  G4double lengthSafetyLarge; ///< Cache of global constant.
  /// @{ Factory instance.
  BDSMagnetOuterFactoryBase* none;
  BDSMagnetOuterFactoryBase* cylindrical;
  BDSMagnetOuterFactoryBase* polescircular;
  BDSMagnetOuterFactoryBase* polessquare;
  BDSMagnetOuterFactoryBase* polesfacet;
  BDSMagnetOuterFactoryBase* polesfacetcrop;
  BDSMagnetOuterFactoryBase* lhcright;
  BDSMagnetOuterFactoryBase* lhcleft;
  /// @}
  G4bool sensitiveOuter; ///< Cache of global option.
};



#endif
