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
#ifndef BDSPARALLELWORLDIMPORTANCE_H
#define BDSPARALLELWORLDIMPORTANCE_H

#include "BDSExtent.hh"
#include "BDSImportanceVolumeStore.hh"

#include "globals.hh" // geant4 types / globals
#include "G4GeometryCell.hh"
#include "G4VUserParallelWorld.hh"

#include <map>

class G4UserLimits;
class G4VisAttributes;
class G4VPhysicalVolume;

namespace GMAD {
  struct Element;
  template<typename T> class FastList;
  class Placement;
}

/**
 * @brief Class that constructs a parallel importance world
 *
 * @author Will Shields
 */

class BDSParallelWorldImportance: public G4VUserParallelWorld
{
public:
  BDSParallelWorldImportance(G4String name,
			     G4String importanceWorldGeometryFile,
			     G4String importanceValuesFile);
  virtual ~BDSParallelWorldImportance();

  /// Overridden Geant4 method that must be implemented. Constructs
  /// the Geant4 geometry and returns the finished world physical volume.
  void Construct();

  /// Create biasing operations.
  void BuildPhysicsBias();

  /// Public access to the world extent.
  BDSExtent WorldExtent() const {return worldExtent;}

  /// Get geometry cell from store.
  G4GeometryCell GetGeometryCell(G4int i) const;

  /// Create IStore for all importance sampling geometry cells.
  void AddIStore();

  virtual void ConstructSD();

  /// World volume getter required in parallel world utilities.
  inline G4VPhysicalVolume* GetWorldVolume() {return imWorldPV;}

private:
  /// assignment and copy constructor not implemented nor used
  BDSParallelWorldImportance& operator=(const BDSParallelWorldImportance&);
  BDSParallelWorldImportance(BDSParallelWorldImportance&);

  /// Build the world volume using the extent of the BDSBeamline instance created
  /// in BuildBeamline()
  void BuildWorld();

  /// Importance sampling world volume
  G4VPhysicalVolume* imWorldPV;

  BDSExtent worldExtent; ///< Record of the world extent.

  /// Store of geometry cells for importance volumes.
  BDSImportanceVolumeStore imVolumeStore;

  /// Container for all user placed physical volumes and corresponding importance values.
  std::map<G4String, G4double> imVolumesAndValues;

  G4String imGeomFile;
  G4String imVolMap;
  const G4String componentName; ///< String preprended to geometry with preprocessGDML

  ///@{ Cached global constants values.
  G4int            verbosity;
  G4UserLimits*    userLimits;
  G4VisAttributes* visAttr;
  ///@}

  /// Get importance value of a given physical volume name.
  G4double GetCellImportanceValue(const G4String& cellName);
};

#endif

