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
#ifndef BDSBEAMPIPEFACTORYRACETRACK_H
#define BDSBEAMPIPEFACTORYRACETRACK_H

#include "BDSBeamPipeFactoryPoints.hh"

/**
 * @brief Factory for racetrack aperture model beampipes.
 * 
 * This is a rectangular aperture with curved edges. In the 
 * positive quadrante (+ve x & y) the user specifies (x,y) as
 * well as radius r that forms a quarter circle from this point.
 * This is reflected to the other quadrants. To achieve this using
 * Geant4 solids, a vector of points is built up transversely in 
 * clockwise order to form a solid G4Polyhedra. To create the beam
 * pipe, two are created and subtracted. The number of points in the
 * curved sections can also be chosen but increasing the detail will
 * also reduce the efficiency of course.
 *
 * aper1,2 and 3 are used to represent x,y and r respectively. aper4 is ignored.
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeFactoryRaceTrack: public BDSBeamPipeFactoryPoints
{
public:
  BDSBeamPipeFactoryRaceTrack();
  virtual ~BDSBeamPipeFactoryRaceTrack(){;}
  
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

  void GenerateRaceTrack(std::vector<G4TwoVector>& vec,
			 G4double x,
			 G4double y,
			 G4double r,
			 G4int    pointsPerTowPi);
};
  
#endif
