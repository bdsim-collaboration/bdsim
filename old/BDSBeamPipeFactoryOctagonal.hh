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
#ifndef BDSBEAMPIPEFACTORYOCTAGONAL_H
#define BDSBEAMPIPEFACTORYOCTAGONAL_H

#include "BDSBeamPipeFactoryPoints.hh"

namespace BDS
{
  struct FourPoints
  {
    G4double aper1;
    G4double aper2;
    G4double aper3;
    G4double aper4;
  };
}

/**
 * @brief Factory for octagonal aperture model beampipes.
 * 
 * This is a rectangular aperture with faceted flat corners. In the 
 * positive quadrant (+ve x & y) the user specifies (x,y) of the full
 * width in the horizontal and the full height in the vertical. A
 * second set of (x,y) coordinates specify the x and y points of the 
 * cut points. Therefore, the second x,y are always <  than the first ones.
 * This is reflected in all four quadrants. Practically, a G4ExtrudedSolid
 * is used where simply points are defined and this shape is extruded.
 *
 * aper1,2 are used to represent x1,y1 and aper3,4 are x2,y2 respectively.
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeFactoryOctagonal: public BDSBeamPipeFactoryPoints
{
public:
  BDSBeamPipeFactoryOctagonal();
  virtual ~BDSBeamPipeFactoryOctagonal(){;}
  
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

  /// Append 2D points on an octagon in a clockwise direction into the vector argument.
  void GenerateOctagonal(std::vector<G4TwoVector>& vec,
			 G4double x1,
			 G4double y1,
			 G4double x2,
			 G4double y2);

  /// Calculate the corresponding aper1234 values if the octagonal beam pipe shape were
  /// to be expanded by "distance". This calculates the points, then works out the corresponding
  /// x,y coordinates for the expanded points such that each side of the octagon is expanded by
  /// "distance". The aper1234 parameterisation is then calculated from these x,y, coordinates.
  BDS::FourPoints ExpandOctagon(G4double aper1,
			   G4double aper2,
			   G4double aper3,
			   G4double aper4,
			   G4double distance);
};
  
#endif
