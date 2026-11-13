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
#ifndef BDSBEAMPIPEFACTORYCIRCULARVACUUM_H
#define BDSBEAMPIPEFACTORYCIRCULARVACUUM_H

#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipe.hh"

/**
 * @brief Factory for vacuum-only circular volumes.
 *
 * No metal beam pipe is made surrounding the vacuum, there is only vacuum.
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeFactoryCircularVacuum: public BDSBeamPipeFactoryBase
{
public:
  BDSBeamPipeFactoryCircularVacuum();
  virtual ~BDSBeamPipeFactoryCircularVacuum(){;}

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
  /// use member variables unique to this factory to pass them around
  BDSBeamPipe* CommonFinalConstruction(const G4String& nameIn,
				       G4Material* vacuumMaterialIn,
				       G4double    lengthIn,
				       G4double    containerRadiusIn);
};
  
#endif
