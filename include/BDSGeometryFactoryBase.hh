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
#ifndef BDSGEOMETRYFACTORYBASE_H
#define BDSGEOMETRYFACTORYBASE_H

#include "BDSExtent.hh"
#include "BDSFactoryBase.hh"
#include "BDSSDType.hh"

#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4String.hh"

#include <map>
#include <set>
#include <vector>

class G4Colour;
class G4LogicalVolume;
class G4UserLimits;
class G4VisAttributes;
class G4VPhysicalVolume;
class G4VSolid;
class BDSColourMap;
class BDSGeometryExternal;

/**
 * @brief Base class for external geometry loading factories.
 * 
 * @author Laurie Nevay
 */

class BDSGeometryFactoryBase: public BDSFactoryBase
{
public:
  BDSGeometryFactoryBase();
  virtual ~BDSGeometryFactoryBase();

  /// Main method to load and construct geometry.
  virtual BDSGeometryExternal* Build(G4String componentName,
                                     G4String fileName,
                                     std::map<G4String, G4Colour*>* colourMapping    = nullptr,
                                     G4bool                 autoColour               = true,
                                     G4double               suggestedLength          = 0,
                                     G4double               suggestedHorizontalWidth = 0,
                                     std::vector<G4String>* vacuumBiasVolumeNames    = nullptr,
                                     G4bool                 makeSensitive            = true,
                                     BDSSDType              sensitivityType          = BDSSDType::energydep,
                                     BDSSDType              vacuumSensitivityType    = BDSSDType::energydepvacuum,
                                     G4UserLimits*          userLimitsToAttachToAllLVs = nullptr) = 0;

  /// Apply a colour mapping to a set of logical volumes. This applies a colour from the map
  /// if the key value is found as a substring or whole part of the logical volume name. Ie
  /// the BDSColour* (red) is defined to key 'quad' and any logical volume with 'quad'
  /// (case sensitive) will be set as red. Caches common G4VisAttributes (so no repeats for
  /// same colour) and returns those constructed. Map is searched through so key order gives
  /// precedence order.
  virtual std::set<G4VisAttributes*> ApplyColourMapping(const std::set<G4LogicalVolume*>& lvs,
                                                        const std::map<G4String, G4Colour*>& mapping,
                                                        G4bool                            autoColour,
                                                        const G4String&                   preprocessPrefix = "");

  /// Attach a set of user limits to every logical volume supplied.
  virtual void ApplyUserLimits(const std::set<G4LogicalVolume*>& lvsIn,
                               G4UserLimits* userLimits);

  /// Attach the relevant general and vacuum sensitivity to each volume.
  virtual void ApplySensitivity(BDSGeometryExternal* result,
                                const std::set<G4LogicalVolume*>& allLogicalVolumesIn,
                                BDSSDType generalSensitivity,
                                const std::set<G4LogicalVolume*>& vacuumLogicalVolumes,
                                BDSSDType vacuumSensitivity);

protected:
  /// Virtual clean up that derived classes can override that calls CleanUpBase().
  virtual void CleanUp();

  /// Provide the preprocessed object (such as volume) names in case they're processed
  /// whilst loading from external formats. By default, no action.
  virtual G4String PreprocessedName(const G4String& objectName,
                                    const G4String& acceleratorComponentName) const;

  /// Get the volumes that match the name. Volume names are matched exactly and are case sensitive.
  std::set<G4LogicalVolume*> GetVolumes(const std::set<G4LogicalVolume*>& allLVs,
                                        std::vector<G4String>*            volumeNames,
                                        G4bool                            preprocessGDML,
                                        const G4String&                   componentName) const;
  
  
  /// Initialise variables - used to reset variables before each use of the factory.
  /// Non-virtual as used in constructor.
  void CleanUpBase();

  /// Expand the acuumulated extents using a (possibly asymmetric) extent instance.
  void ExpandExtent(const BDSExtent& extent);
  
  /// Expand the accumulated extents with a symmetric extent in each dimension
  /// (rx,ry,rz) about the offset x0,y0,z0.
  void ExpandExtent(G4double xO, G4double rx,
                    G4double y0, G4double ry,
                    G4double z0, G4double rz);

  /// Expand the extent but asymmetrically.
  void ExpandExtent(G4double x0, G4double xLow, G4double xHigh,
                    G4double y0, G4double yLow, G4double yHigh,
                    G4double z0, G4double zLow, G4double zHigh);

  /// @{ Extent in one dimension.
  G4double xmin;
  G4double xmax;
  G4double ymin;
  G4double ymax;
  G4double zmin;
  G4double zmax;
  /// @}

  BDSExtent Extent() const {return BDSExtent(xmin, xmax, ymin, ymax, zmin, zmax);}
};

#endif
