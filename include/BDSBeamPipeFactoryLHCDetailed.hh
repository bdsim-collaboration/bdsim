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
#ifndef BDSBEAMPIPEFACTORYLHCDETAILED_H
#define BDSBEAMPIPEFACTORYLHCDETAILED_H

#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipe.hh"

#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"

class BDSBeamPipeInfo;
class G4Material;
class G4LogicalVolume;
class G4PVPlacement;
class G4VSolid;

/**
 * @brief Factory for detailed lhc aperture model beam pipes.
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeFactoryLHCDetailed: public BDSBeamPipeFactoryBase
{
public:
  BDSBeamPipeFactoryLHCDetailed();
  virtual ~BDSBeamPipeFactoryLHCDetailed(){;}
  
  BDSBeamPipe* CreateBeamPipe(const G4String&   name,
                              G4double          length,
                              const BDSBeamPipeInfo* bpi) override;

  /// Access cooling pipe information from factory for parameter tests
  G4double GetFullWidthOfCoolingPipe();

  /// Clean up extra pointers particular to this factory before use
  void CleanUp() override;

private:
  /// Do the actual clean up of members here in a non-virtual function so it can be
  /// used in the constructor.
  void CleanUpLHCDetailed();

  G4bool AngledFaces(const G4ThreeVector& v1,
                     const G4ThreeVector& v2);

  BDSBeamPipe* CreateBeamPipeAngled(const G4String&   name,
                                    G4double          length,
                                    const BDSBeamPipeInfo* bpi,
                                    const G4ThreeVector& inputFaceNormalIn,
                                    const G4ThreeVector& outputFaceNormalIn);
  
  /// only the solids are unique, once we have those, the logical volumes and placement in the
  /// container are the same.  group all this functionality together
  BDSBeamPipe* CommonFinalConstruction(const G4String&    name,
                                       G4Material* vacuumMaterial,
                                       G4Material* beamPipeMaterial,
                                       G4double    length,
                                       G4double    containerRadius);

  /// The angled ones have degeneracy in the geant4 solids they used so we can avoid code duplication
  /// by grouping common construction tasks.
  G4double CreateGeneralAngledSolids(const G4String&      name,
                                     G4double             length,
                                     const G4ThreeVector& inputface,
                                     const G4ThreeVector& outputface);

  void  BuildLogicalVolumes(const G4String&    nameIn,
                            G4Material* vacuumMaterialIn,
                            G4Material* beamPipeMaterialIn) override;
  void  SetVisAttributes(G4Material* beamPipeMaterialIn,
                         G4Material* bacuumMaterialIn) override;
  void  SetUserLimits(G4double length) override;
  void  PlaceComponents(const G4String& nameIn) override;

  /// Utility function to initialise the geometry parameters
  void InitialiseGeometricalParameters();

  /// Calculate the various radii and geometrical parameters for this design
  /// based on the input aperture parameters
  void CalculateGeometricalParameters(G4double aper1,
                                      G4double aper2,
                                      G4double aper3,
                                      G4double beamPipeThickness,
                                      G4double length);

  G4VSolid*        copperSkinSolid;
  G4VSolid*        screenSolid;
  G4VSolid*        coolingPipeSolid;
  G4LogicalVolume* copperSkinLV;
  G4LogicalVolume* screenLV;
  G4LogicalVolume* coolingPipeLV;
  G4PVPlacement*   copperSkinPV;
  G4PVPlacement*   screenPV;
  G4PVPlacement*   coolingPipeTopPV;
  G4PVPlacement*   coolingPipeBottomPV;

  /// Parameters that control the design
  G4double coldBoreThickness;
  G4double coolingPipeThickness;
  G4double coolingPipeRadius;
  G4double copperSkinThickness;

  /// Calculated parameters below here
  /// vacuum volume
  G4double vacRadius, vacBoxX, vacBoxY;
  
  /// Copper skin geometrical parameters
  G4double cuInnerRadius, cuInnerBoxX, cuInnerBoxY;
  G4double cuOuterRadius, cuOuterBoxX, cuOuterBoxY;

  /// Beam screen geometrical parameters
  G4double bsInnerRadius, bsInnerBoxX, bsInnerBoxY;
  G4double bsOuterRadius, bsOuterBoxX, bsOuterBoxY;
  
  /// Cold bore geometrical parameters
  G4double cbInnerRadius, cbOuterRadius;

  /// Container geometrical parameters
  G4double containerRadius;
  
  /// General length variable (to avoid mistakes)
  G4double vacHalfLength;
  G4double halfLength;

  /// Cooling pipe geometrical parameters
  G4double coolingPipeOffset;

  /// Whether given the aperture parameters the pipe is square on the sides and round at the top
  /// - the 'vertical'.
  G4bool verticalOrientation;

  /// Whether to build the cooling pipes on either side
  G4bool buildCoolingPipe; 
};
  
#endif
