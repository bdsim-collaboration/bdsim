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
#ifndef BDSMAGNET_H
#define BDSMAGNET_H

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSFieldInfo.hh"
#include "BDSMagnetType.hh"

class BDSBeamPipe;
class BDSBeamPipeInfo;
class BDSMagnetOuter;
class BDSMagnetOuterInfo;
class BDSMagnetStrength;

/**
 * @brief Abstract base class that implements features common to all magnets. 
 *
 * Magnet consists of a beam pipe (with optional field) and an 'outer' (with
 * optional field) both placed in the same level in a container volume. The
 * outer is typically a yoke but can be any geometry as long as it doesn't
 * overlap with the beam pipe placed at the same level.
 * 
 * @author Laurie Nevay
 */

class BDSMagnet: public BDSAcceleratorComponent
{
public:
  /// Magnet base class constructor that is for basic geometric information rather
  /// magnetic field details, which are handled in the derived classes
  BDSMagnet(BDSMagnetType       type,
	    const G4String&     name, 
	    G4double            length,
	    BDSBeamPipeInfo*    beamPipeInfo,
	    BDSMagnetOuterInfo* magnetOuterInfo,
	    BDSFieldInfo*       vacuumFieldInfoIn,
	    G4double            angle            = 0,
	    BDSFieldInfo*       outerFieldInfoIn = nullptr,
	    G4bool              isThin           = false);
  
  virtual ~BDSMagnet();
  
  inline const BDSMagnetStrength* MagnetStrength() const {return vacuumFieldInfo ? vacuumFieldInfo->MagnetStrength() : nullptr;}

  /// @ { Delete existing field info and replace.
  void SetOuterField(BDSFieldInfo* outerFieldInfoIn);
  void SetVacuumField(BDSFieldInfo* vacuumFieldInfoIn);
  /// @}

  /// Determine which is the appropriate key to query a BDSMagnetStrength with for
  /// a given type of magnet.
  static G4String DetermineScalingKey(BDSMagnetType typeIn);

  /// @{ Update face normal and also to beam pipe and magnet outer.
  virtual void SetInputFaceNormal(const G4ThreeVector& input) override;
  virtual void SetOutputFaceNormal(const G4ThreeVector& output) override;
  /// @}

  /// @{ Actual normals of the constructed beam pipe / magnet outer geometry.
  virtual G4ThreeVector GeometryInputFaceNormal() const override;
  virtual G4ThreeVector GeometryOutputFaceNormal() const override;
  /// @}

  /// Accessor to outer material if it exists.
  virtual G4String Material() const;

  /// Override for a magnet - test on either field definition.
  virtual G4bool HasAField() const {return vacuumFieldInfo || outerFieldInfo;}
  
  /// Override function as we have different field recipe objects.
  virtual void SetFieldUsePlacementWorldTransform();

  /// Set the explicit local-to-global transform on the vacuum and outer
  /// fields when this magnet is delegated through BDSLink.
  virtual void SetFieldLinkTransform(const G4Transform3D& localToGlobal) override;

protected:
  /// Private default constructor to force the use of the supplied one.
  BDSMagnet() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSMagnet& operator=(const BDSMagnet&) = delete;
  BDSMagnet(BDSMagnet&) = delete;
  /// @}
  
  /// Overridden method of BDSAcceleratorComponent to not only build container, but
  /// first construct field objects. After using BDSAcceleratorComponent::Build() to
  /// build the container, the beam pipe and outer geometry are built.
  virtual void Build();
  
  /// Construct a general straight piece of beampipe. Virtual so it can be overloaded
  /// by derived classes as required - such as RBend.
  virtual void BuildBeampipe();

  /// Construct the field for the vacuum and attach it
  virtual void BuildVacuumField();

  /// Construct the outer magnet geometry beit poled geometry or cylinders. This
  /// function switches on the member variable of BDSMagnetType type, so is contained in
  /// the base class.
  virtual void BuildOuter();
  
  /// Construct the magnetic field for the outer magnet geometry.
  virtual void BuildOuterField();

  /// Necessary to provide this function a la BDSAcceleratorComponent. If there's an
  /// outer geometry, the containerSolid will have been set and build it into a logical
  /// volume. If no outer geometry, use the beam pipe container as this objects container
  /// in which case no need to build any logical volumes. Update extents if necessary. 
  virtual void BuildContainerLogicalVolume();

  /// Place the beam pipe and outer geometry in the overall container. If there's no outer
  /// geometry, then we don't need to place either as the beam pipe becomes the container.
  virtual void PlaceComponents();
  
  /// Magnet type
  BDSMagnetType magnetType;

  /// Model information for the outer volume construction
  BDSMagnetOuterInfo* magnetOuterInfo;

  /// Field information for vacuum field
  BDSFieldInfo* vacuumFieldInfo;

  /// Field information for outer magnetic field (optional)
  BDSFieldInfo* outerFieldInfo;
  
  /// The constructed beampipe
  BDSBeamPipe*    beampipe;

  /// In the case where there's no magnet outer geometry, we just use the beam pipe
  /// and therefore we don't need to place it again. Use this boolean to record that
  /// and decide upon it. Default = true.
  G4bool          placeBeamPipe;

  /// For outer volume construction
  G4double        horizontalWidth;

  /// Separate variable for the container radius so it can be changed if
  /// required by specific magnet geometry.
  G4double        containerRadius;
  
  /// Displacement (if required - default 0,0,0) for specific magnet geometry. Allows
  /// derived classes to specify different geometry without having to override placement
  /// which is provided in base class.
  G4ThreeVector magnetOuterOffset;
  
  /// The assembled outer magnet geometry
  BDSMagnetOuter* outer;

  /// Used to pass the placement offset to the field so that it can be offset from the
  /// local coordinates of the solid appropriately.
  G4Transform3D beamPipePlacementTransform;

  /// Boolean to store if the element is thin - will have no geometry constructed.
  G4bool isThin;
};

#endif
