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
#ifndef BDSBEAMPIPEFACTORYCLICPCL_H
#define BDSBEAMPIPEFACTORYCLICPCL_H

#include "BDSBeamPipeFactoryPoints.hh"
#include "BDSBeamPipe.hh"

/**
 * @brief Factory for CLIC post collision line beam pipes.
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeFactoryClicPCL: public BDSBeamPipeFactoryPoints
{
public:
  BDSBeamPipeFactoryClicPCL();
  virtual ~BDSBeamPipeFactoryClicPCL(){;}
  
private:
  /// Generate quarters of a circle to represent the edges joined by straight lines.
  /// Overloaded (required) from BDSBeamPipeFactoryPoints
  virtual void GeneratePoints(G4double aper1,
			      G4double aper2,
			      G4double aper3,
			      G4double aper4,
			      G4double beamPipeThickness,
			      G4int    pointsPerTwoPi = 40);

  /// Calculate the radius of the solid used for intersection for angled faces.
  virtual G4double CalculateIntersectionRadius(G4double aper1,
					       G4double aper2,
					       G4double aper3,
					       G4double aper4,
					       G4double beamPipeThickness);

  void GenerateClicPCL(std::vector<G4TwoVector>& vec,
		       G4double aper1,
		       G4double aper2,
		       G4double aper3,
		       G4double aper4,
		       G4int    pointsPerTowPi,
		       G4double margin = 0);

protected:
  /// Clear member vectors and run base class clean up to clear pointers between runs.
  virtual void CleanUp();
  
private:
  /// Clear member vectors - used for both initialisation and virtual CleanUp.
  void CleanUpClicPCL();
  
  /// Overloads BDSBeamPipeFactoryPoints to make asymmetric extents, otherwise the same.
  virtual BDSBeamPipe* CommonFinalConstruction(const G4String& nameIn,
					       G4Material* vacuumMaterialIn,
					       G4Material* beamPipeMaterialIn,
					       G4double    lengthIn);

  G4double extentYLow;   ///< Cache of extent to pass around.
  G4double extentYHigh;  ///< Cache of extent to pass around.
};
  
#endif
