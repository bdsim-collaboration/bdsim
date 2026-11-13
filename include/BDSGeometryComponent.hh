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
#ifndef BDSGEOMETRYCOMPONENT_H
#define BDSGEOMETRYCOMPONENT_H

#include "BDSExtent.hh"
#include "BDSSDType.hh"

#include "globals.hh"           // geant4 globals / types
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"

#include <map>
#include <set>
#include <utility>              //for std::pair

class G4AssemblyVolume;
class G4UserLimits;
class G4VisAttributes;
class G4VPhysicalVolume;
class G4VSensitiveDetector;
class G4VSolid;
namespace CLHEP {
  class HepRotation;
}
typedef CLHEP::HepRotation G4RotationMatrix;

/**
 * @brief A generic geometry component for a bdsim model.
 * 
 * This class contains the minimum information required
 * for any geometrical object for a bdsim geant4 model.
 * 
 * It represents one complete geometrical object and all
 * the (c++) objects that compose it - therefore, once constructed,
 * a BDSGeometryComponent instance owns all the objects of its members.
 *
 * Fundamentally, the piece of geometry can have an outer 'container'
 * that is either a Logical Volume or an Assembly Volume.
 * 
 * Note, the container solid and container logical volume are automatically
 * registered by the constructor and do not need to be registered separately
 * like all other volumes. There are safeguards to prevent double registration
 * and the consequential seg faults.
 *
 * Many accessors construct a new set of pointers to return rather than a
 * const reference to the member set as the pointers from daughter objects
 * are appended to the temporary set. The originals are retained untouched.
 * 
 * @author Laurie Nevay
 */

class BDSGeometryComponent
{
public:
  BDSGeometryComponent(G4VSolid*            containerSolidIn,
		       G4LogicalVolume*     containerLVIn,
		       const BDSExtent&     extentIn            = BDSExtent(),
		       const BDSExtent&     innerExtentIn       = BDSExtent(),
		       const G4ThreeVector& placementOffsetIn   = G4ThreeVector(0,0,0),
		       G4RotationMatrix*    placementRotationIn = nullptr);
  
  BDSGeometryComponent(G4AssemblyVolume* containerAssemblyIn,
                       const BDSExtent&     extentIn            = BDSExtent(),
                       const BDSExtent&     innerExtentIn       = BDSExtent(),
                       const G4ThreeVector& placementOffsetIn   = G4ThreeVector(0,0,0),
                       G4RotationMatrix*    placementRotationIn = nullptr);
  
  /// Copy constructor (no copying of registered objects)
  BDSGeometryComponent(const BDSGeometryComponent& component);
  /// Assignment operator not used
  BDSGeometryComponent& operator=(const BDSGeometryComponent&) = delete;
  virtual ~BDSGeometryComponent();
  /// Whether the container is an assembly. If not, it's a logical volume.
  G4bool ContainerIsAssembly() const {return containerIsAssembly;}

  /// @{ Accessor - see member for more info
  virtual inline G4String  GetName()                   const {return containerLogicalVolume->GetName();}
  inline G4VSolid*         GetContainerSolid()         const {return containerSolid;}
  inline G4LogicalVolume*  GetContainerLogicalVolume() const {return containerLogicalVolume;}
  inline G4AssemblyVolume* GetContainerAssemblyVolume() const {return containerAssembly;}
  inline G4Transform3D     GetPlacementTransform()     const;
  inline G4ThreeVector     GetPlacementOffset()        const {return placementOffset;}
  inline G4RotationMatrix* GetPlacementRotation()      const {return placementRotation;}
  inline BDSExtent         GetExtent()                 const {return outerExtent;}
  inline BDSExtent         GetInnerExtent()            const {return innerExtent;}
  inline std::pair<G4double,G4double> GetExtentX()     const {return outerExtent.ExtentX();}
  inline std::pair<G4double,G4double> GetExtentY()     const {return outerExtent.ExtentY();}
  inline std::pair<G4double,G4double> GetExtentZ()     const {return outerExtent.ExtentZ();}
  inline std::pair<G4double,G4double> GetInnerExtentX() const {return innerExtent.ExtentX();}   
  inline std::pair<G4double,G4double> GetInnerExtentY() const {return innerExtent.ExtentY();}
  inline std::pair<G4double,G4double> GetInnerExtentZ() const {return innerExtent.ExtentZ();}
  virtual std::set<G4VPhysicalVolume*> GetAllPhysicalVolumes()  const {return allPhysicalVolumes;}
  virtual std::set<G4RotationMatrix*>  GetAllRotationMatrices() const {return allRotationMatrices;}
  virtual std::set<G4VisAttributes*>   GetAllVisAttributes()    const {return allVisAttributes;}
  virtual std::set<G4UserLimits*>      GetAllUserLimits()       const {return allUserLimits;}
  virtual std::set<BDSGeometryComponent*> GetAllDaughters()     const {return allDaughters;}
  virtual std::set<G4VSolid*>          GetAllSolids()           const {return allSolids;}
  /// @}
  
  /// Set the offset from 0,0,0 that the object should ideally be placed in its parent
  inline void SetPlacementOffset(const G4ThreeVector& offsetIn) {placementOffset = G4ThreeVector(offsetIn);}

  /// @{ Set extent
  inline void SetExtent(const BDSExtent& extIn)      {outerExtent = extIn;}
  inline void SetInnerExtent(const BDSExtent& extIn) {innerExtent = extIn;}
  /// @}
  
  /// Get the extent of the object in the positive direction in all dimensions
  G4ThreeVector GetExtentPositive() const {return outerExtent.ExtentPositive();}

  /// Get the extent of the object in the negative direction in all dimensions
  G4ThreeVector GetExtentNegative() const {return outerExtent.ExtentNegative();}

  /// Update the extents of this object with those of another object
  void InheritExtents(BDSGeometryComponent const * const anotherComponent);

  /// Update the extents of this object with those of another object
  /// whilst accounting for any offset.
  void InheritExtents(BDSGeometryComponent const * const anotherComponent,
		      const G4ThreeVector& offset);

  /// Register another geometry component as belonging to this one. This component will
  /// then own and delete it as necessary.
  void RegisterDaughter(BDSGeometryComponent* anotherComponent) {allDaughters.insert(anotherComponent);}
  
  /// Register a solid as belonging to this geometry component, which then becomes responsible
  /// for it. Note, the container solid given in the constructor is automatically registered.
  inline void RegisterSolid(G4VSolid* solid) {allSolids.insert(solid);}

  /// Similarly for a set of unique solids.
  void RegisterSolid(const std::set<G4VSolid*>& solids);
  
  /// Register a logical volume as belonging to this geometry component, which then becomes
  /// responsible for it. Note the container logical volume for this piece of geometry is
  /// automatically registered separately.
  inline void RegisterLogicalVolume(G4LogicalVolume* logicalVolume) {allLogicalVolumes.insert(logicalVolume);}
  
  /// Apply RegisterLogicalVolume() to a set of logical volumes.
  void RegisterLogicalVolume(const std::set<G4LogicalVolume*>& localVolumes);
  
  /// Mark a volume as one that should be made sensitive. This method will also
  /// check and ensure that the sensitive logical volume is an already registered
  /// logical volume with this geometry component.  
  void RegisterSensitiveVolume(G4LogicalVolume* sensitiveVolume,
			       BDSSDType sensitivityType);

  /// Apply RegisterSensitiveVolume(G4LogicalVolume* sensitiveVolume) to a
  /// set of logical volumes.
  void RegisterSensitiveVolume(const std::set<G4LogicalVolume*>& sensitiveVolumes,
                               BDSSDType sensitivityType);

  /// Copy in a mapping of sensitive volumes to sensitive detectors.
  void RegisterSensitiveVolume(const std::map<G4LogicalVolume*, BDSSDType>& sensitiveVolumes);
    
  /// Register a physical volume as belonging to this geometry component, which then becomes
  /// responsible for it.
  inline void RegisterPhysicalVolume(G4VPhysicalVolume* physicalVolume) {allPhysicalVolumes.insert(physicalVolume);}

  /// Apply RegisterPhysicalVolume() to a set of physical volumes.
  void RegisterPhysicalVolume(const std::set<G4VPhysicalVolume*>& physicalVolumes);

  /// Register a rotation matrix as belonging to this geometry component, which then becomes
  /// responsible for it.
  inline void RegisterRotationMatrix(G4RotationMatrix* rotationMatrix) {allRotationMatrices.insert(rotationMatrix);}

  /// Apply Register RotationMatrix() to a set of rotation matrices
  void RegisterRotationMatrix(const std::set<G4RotationMatrix*>& rotationMatrices);

  /// Register a visualisation attribute object as belonging to this geometry component, which
  /// then becomes responsible for it.
  inline void RegisterVisAttributes(G4VisAttributes* visAttribute) {allVisAttributes.insert(visAttribute);}

  /// Apply RegisterVisAttribute() to a set of visualisation attributes
  void RegisterVisAttributes(const std::set<G4VisAttributes*>& visAttributes);

  /// Register a user limits object as belonging to this geometry component, which then
  /// becomes responsible for it
  inline void RegisterUserLimits(G4UserLimits* userLimit) {allUserLimits.insert(userLimit);}

  /// Apply RegisterUserLimit to a set of user limits.
  void RegisterUserLimits(const std::set<G4UserLimits*>& userLimits);
  
  /// Utility method to copy all the logical & physical volumes plus rotation matrices from a
  /// BDSGeometryComponent instance to this one. Useful for example when prefabricated objects
  /// from factories are made to belong to an object.
  void InheritObjects(BDSGeometryComponent* component);

  /// Access all logical volumes belonging to this component
  virtual std::set<G4LogicalVolume*> GetAllLogicalVolumes() const;

  /// Return all logical volumes that should be used for biasing minus any that are in the excluded set.
  virtual std::set<G4LogicalVolume*> GetAllBiasingVolumes() const;
  
  /// Access all sensitive volumes belonging to this component
  virtual std::map<G4LogicalVolume*, BDSSDType> GetAllSensitiveVolumes() const;

  /// Make all logical volumes sensitive by setting flag for later attachment with
  /// AttachSensitiveDetectors. This propagates to all daughter volumes.
  inline void MakeAllVolumesSensitive(BDSSDType stype = BDSSDType::energydep) {RegisterSensitiveVolume(GetAllLogicalVolumes(), stype);}

  /// Attach a sensitive detector class to all registered sensitive volumes in this component.
  virtual void AttachSensitiveDetectors();

  /// Remove a particular logical volume from the logical volumes that will be
  /// returned for biasing.
  virtual void ExcludeLogicalVolumeFromBiasing(G4LogicalVolume* lv);

  /// Change from a container logical volume to an assembly volume.
  void StripOuterAndMakeAssemblyVolume();

  /// Utility function to apply user limits to an assembly volume as there's not interface.
  static void AttachUserLimitsToAssembly(G4AssemblyVolume* av,
                                         G4UserLimits* ul);

protected:
  G4bool           containerIsAssembly; ///< True if the 'container' is really an assembly; false if an LV.
  G4VSolid*        containerSolid;
  G4LogicalVolume* containerLogicalVolume;
  G4AssemblyVolume* containerAssembly;
  BDSExtent        outerExtent;
  BDSExtent        innerExtent;
  
  /// registry of all daughter geometry components
  std::set<BDSGeometryComponent*> allDaughters;
  
  /// registry of all solids belonging to this component
  std::set<G4VSolid*> allSolids;
  
  std::set<G4LogicalVolume*> allLogicalVolumes;
  // we have to keep a registry of all logical volumes to be able to associate
  // information with them at construction time - for example S position - that
  // can't be stored in the Logical Volume class itself without modifying geant

  /// Mapping of which volumes have which sensitivity. Not all volumes need be
  /// registered. The way of saying no sensitivity is to not have the volume registered.
  std::map<G4LogicalVolume*, BDSSDType> sensitivity;

  /// If true, the sensitivity will be applied irrespective of the general store options.
  /// So if eloss is turned off but you really want from this component, you can.
  G4bool overrideSensitivity;

  /// registry of all physical volumes belonging to this component
  std::set<G4VPhysicalVolume*> allPhysicalVolumes;

  /// registry of all rotation matrices belonging to this component
  std::set<G4RotationMatrix*> allRotationMatrices;

  /// registry of all visualisation attributes belonging to this component
  std::set<G4VisAttributes*> allVisAttributes;

  /// registry of all user limits belonging to this component
  std::set<G4UserLimits*>    allUserLimits;

  /// The offset to be applied BEFORE rotation when placed - used to account for
  /// any asymmetry the component may have.
  G4ThreeVector                 placementOffset;

  /// The rotation to be applied when placed - used to account for any required rotation
  /// of the geometry before placement such as being built along a different axis. This
  /// is defaulted to a nullptr so MUST be tested before use - this saves memory as every
  /// piece of geometry uses this class.
  G4RotationMatrix*             placementRotation;

  /// Volumes that should not be included when return GetAllBiasingVolumes().
  std::set<G4LogicalVolume*>* lvsExcludedFromBiasing;
};

inline G4Transform3D BDSGeometryComponent::GetPlacementTransform() const
{
  if (!placementRotation)
    {return G4Transform3D(G4RotationMatrix(), placementOffset);}
  else
    {return G4Transform3D(*placementRotation, placementOffset);}
}

#endif
