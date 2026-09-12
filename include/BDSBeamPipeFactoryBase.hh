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
#ifndef BDSBEAMPIPEFACTORYBASE_H
#define BDSBEAMPIPEFACTORYBASE_H

#include "BDSBeamPipe.hh"
#include "BDSFactoryBase.hh"

#include "globals.hh"         // geant4 globals / types
#include "G4RotationMatrix.hh"

#include <set>

class G4LogicalVolume;
class G4Material;
class G4PVPlacement;
class G4UserLimits;
class G4VSolid;

/**
 * @brief Abstract base class for beampipe factory classes.
 * 
 * Abstract base class that defines the interface of each factory
 * to build any type of beampipe required. There are four general
 * possibilities apart from the shape (each derived class will give
 * a different cross-section) and these dictate the angled faces.
 * 
 * Uses Geant4 default units by default, ie mm, rad (not metres)
 * 
 * Note, any type of aperture (in MADX) requires up to 4 parameters
 * 
 * Default values in constructors result in global defaults being used
 * ie beampipe material will be steel - the default from options.
 *
 * 'In' in argument variables is used to distinguish between that and possible
 * internal member variables with the same name - avoiding 'itsVariable'
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeFactoryBase: public BDSFactoryBase
{
public:
  /// create a flat ended beampipe
  virtual BDSBeamPipe* CreateBeamPipe(const G4String& nameIn,                  // name
				      G4double    lengthIn,                    // length [mm]
				      G4double    aper1 = 0,                   // aperture parameter 1
				      G4double    aper2 = 0,                   // aperture parameter 2
				      G4double    aper3 = 0,                   // aperture parameter 3
				      G4double    aper4 = 0,                   // aperture parameter 4
				      G4Material* vacuumMaterialIn = nullptr,  // vacuum material
				      G4double    beamPipeThicknessIn = 0,     // beampipe thickness [mm]
				      G4Material* beamPipeMaterialIn = nullptr,// beampipe material
				      const G4String& pointsFileIn = "",
				      const G4String& pointsUnitIn = ""
				      ) = 0;

  /// Create a beam pipe with angled faces as described by unit normal vectors. The input
  /// face normal vector must have a negative z component and the output face a positive
  /// z component.
  virtual BDSBeamPipe* CreateBeamPipe(const G4String&      nameIn,
				      G4double             lengthIn,
				      const G4ThreeVector& inputFaceNormalIn,
				      const G4ThreeVector& outputFaceNormalIn,
				      G4double      aper1               = 0,
				      G4double      aper2               = 0,
				      G4double      aper3               = 0,
				      G4double      aper4               = 0,
				      G4Material*   vacuumMaterialIn    = nullptr,
				      G4double      beamPipeThicknessIn = 0,
				      G4Material*   beamPipeMaterialIn  = nullptr,
				      const G4String& pointsFileIn      = "",
				      const G4String& pointsUnitIn      = "") = 0;

  /// Virtual base destructor
  virtual ~BDSBeamPipeFactoryBase(){;}

protected:
  /// base constructor
  BDSBeamPipeFactoryBase();

  /// Initialiser and can be used to reset factory pointers after use. Non-virtual as
  /// it's used in the constructor to avoid duplication of code, but it can't be virtual there.
  void CleanUpBase();

  virtual void CleanUp();
  
  /// finalise beampipe construction
  void CommonConstruction(const G4String& nameIn,
			  G4Material* vacuumMaterialIn,
			  G4Material* beamPipeMaterialIn,
			  G4double    length);

  /// build beampipe and register logical volumes
  BDSBeamPipe* BuildBeamPipeAndRegisterVolumes(BDSExtent extent,
					       G4double  containerRadius,
					       G4bool    containerIsCircular = false);

  /// check if a beam pipe volume with angled faces can be constructed
  static void CheckAngledVolumeCanBeBuilt(G4double length,
                                          const G4ThreeVector &inputfaceAngle,
                                          const G4ThreeVector &outputfaceAngle,
                                          G4double horizontalWidth,
                                          const G4String& name);

  // methods called by CommonConstruction, can be implemented by derived classes
  
  /// build logical volumes
  virtual void BuildLogicalVolumes(const G4String& nameIn,
				   G4Material* vacuumMaterialIn,
				   G4Material* beamPipeMaterialIn);
  /// Set visual attributes.
  virtual void SetVisAttributes(G4Material* beamPipeMaterialIn,
                                G4Material* vacuumMaterialIn);

  /// Set user limits.
  virtual void SetUserLimits(G4double length);

  /// Place volumes.
  virtual void PlaceComponents(const G4String& nameIn);
  
  G4bool           sensitiveBeamPipe;    ///< Whether the beam pipe will record energy deposition.
  G4bool           sensitiveVacuum;      ///< Whether the vacuum will record any energy deposition.
  G4bool           storeApertureImpacts; ///< Whether to store aperture impacts.
  G4VSolid*        vacuumSolid;
  G4VSolid*        beamPipeSolid;
  G4VSolid*        containerSolid;
  /// Longer (in length) version of container solid for unambiguous subtraction.
  G4VSolid*        containerSubtractionSolid; 
  G4LogicalVolume* vacuumLV;
  G4LogicalVolume* beamPipeLV;
  G4LogicalVolume* containerLV;
  G4PVPlacement*   vacuumPV;
  G4PVPlacement*   beamPipePV;
  
  /// @{ For recording the face normals in the finished pipe component.
  G4ThreeVector inputFaceNormal;
  G4ThreeVector outputFaceNormal;
  /// @}
};

#endif
