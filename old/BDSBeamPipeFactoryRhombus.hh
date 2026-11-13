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
#ifndef BDSBEAMPIPEFACTORYRHOMBUS_H
#define BDSBEAMPIPEFACTORYRHOMBUS_H

#include "BDSBeamPipeFactoryPoints.hh"

namespace BDS
{
  struct ThreePoints
  {
    G4double aper1;
    G4double aper2;
    G4double aper3;
  };
}

/**
 * @brief Factory for rhombus aperture model beampipes.
 * 
 * This is a diamond shaped aperture. It optionally has a corner radius
 * for rounded edges. aper1 is the full extent of what would be the unrounded
 * corner in x; aper2 is the full extent of what would be the the unrounded
 * corner in y. aper3 is the radius of curvature for the corners and is optional,
 * with the default value of 0.
 *
 * Constructed using an extruded solid with a series of points.
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeFactoryRhombus: public BDSBeamPipeFactoryPoints
{
public:
  BDSBeamPipeFactoryRhombus();
  virtual ~BDSBeamPipeFactoryRhombus(){;}
  
private:
  /// Generate all the sets of required points for each surface.
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
  
  /// Generate either 4 points or set of points with curved edges.
  void GenerateRhombus(std::vector<G4TwoVector>& vec,
                       G4double x1,
                       G4double y1,
                       G4double cornerRadius,
                       G4int pointsPerTwoPi);

  /// Expand the rhombus as needed.
  BDS::ThreePoints ExpandRhombus(G4double aper1,
                                 G4double aper2,
                                 G4double aper3,
                                 G4double distance);
};
  
#endif
