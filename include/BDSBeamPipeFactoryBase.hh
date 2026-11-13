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

class BDSBeamPipeInfo;

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
  virtual BDSBeamPipe* CreateBeamPipe(const G4String&   name,
                                      G4double          length,
                                      const BDSBeamPipeInfo* bpi) = 0;
  
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
  void CommonConstruction(const G4String&    nameIn,
                          G4Material* vacuumMaterialIn,
                          G4Material* beamPipeMaterialIn,
                          G4double    length);

  /// build beampipe and register logical volumes
  BDSBeamPipe* BuildBeamPipeAndRegisterVolumes(const BDSExtent& extent,
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
  virtual void BuildLogicalVolumes(const G4String&    nameIn,
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
