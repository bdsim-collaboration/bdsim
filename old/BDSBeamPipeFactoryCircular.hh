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
#ifndef BDSBEAMPIPEFACTORYCIRCULAR_H
#define BDSBEAMPIPEFACTORYCIRCULAR_H

#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipe.hh"

/**
 * @brief Factory for circular beam pipes.
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeFactoryCircular: public BDSBeamPipeFactoryBase
{
public:
  BDSBeamPipeFactoryCircular();
  virtual ~BDSBeamPipeFactoryCircular(){;}
  
  virtual BDSBeamPipe* CreateBeamPipe(const G4String& nameIn,
				      G4double    lengthIn,
				      G4double    aper1               = 0,
				      G4double    aper2               = 0,
				      G4double    aper3               = 0,
				      G4double    aper4               = 0,
				      G4Material* vacuumMaterialIn    = nullptr,
				      G4double    beamPipeThicknessIn = 0,
				      G4Material* beamPipeMaterialIn  = nullptr,
				      const G4String& pointsFileIn    = "",
				      const G4String& pointsUnitIn    = "");
  
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
				      const G4String& pointsUnitIn      = "");

private:
  /// Abstract common build features to one function
  /// use member variables unique to this factory to pass them around.
  BDSBeamPipe* CommonFinalConstruction(const G4String&    nameIn,
				       G4Material* vacuumMaterialIn,
				       G4Material* beamPipeMaterialIn,
				       G4double    lengthIn,
				       G4double    containerRadiusIn);

  /// Pass containerRadius by reference to update.
  void CreateGeneralAngledSolids(const G4String&      nameIn,
				 G4double             lengthIn,
				 G4double             aper1In,
				 G4double             beamPipeThicknessIn,
				 const G4ThreeVector& inputfaceIn,
				 const G4ThreeVector& outputfaceIn,
				 G4double&     containerRadius);
};
  
#endif
