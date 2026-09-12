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
#ifndef BDSBEAMPIPEFACTORYPOINTSFILE_H
#define BDSBEAMPIPEFACTORYPOINTSFILE_H

#include "BDSBeamPipeFactoryPoints.hh"

#include "G4Types.hh"

/**
 * @brief Factory for beam pipe made from an list of x,y points in a file.
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeFactoryPointsFile: public BDSBeamPipeFactoryPoints
{
public:
  BDSBeamPipeFactoryPointsFile();
  virtual ~BDSBeamPipeFactoryPointsFile(){;}
  
private:
  /// Purely to fulfill interface - should not be used!
  virtual void GeneratePoints(G4double, G4double, G4double, G4double,
			      G4double beamPipeThickness, G4int=40);

  /// Calculate the radius of the solid used for intersection for angled faces.
  virtual G4double CalculateIntersectionRadius(G4double aper1,
					       G4double aper2,
					       G4double aper3,
					       G4double aper4,
					       G4double beamPipeThickness);
  
  /// Calculate the determinant of 2x G4TwoVectors.
  G4double Determinant(const G4TwoVector& v1,
                       const G4TwoVector& v2) const;
  
  G4double maximumRadius;
};
  
#endif
