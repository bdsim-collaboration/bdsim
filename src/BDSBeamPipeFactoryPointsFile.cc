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
#include "BDSAperturePointsLoader.hh"
#include "BDSBeamPipeFactoryPointsFile.hh"

#include "G4Types.hh"
#include "G4TwoVector.hh"

#include <algorithm>
#include <vector>

BDSBeamPipeFactoryPointsFile::BDSBeamPipeFactoryPointsFile():
  maximumRadius(0)
{;}

void BDSBeamPipeFactoryPointsFile::GeneratePoints(G4double, G4double, G4double, G4double,
						  G4double beamPipeThickness, G4int)
{
  maximumRadius = 0;
  
  // the product will not own this points vector pointer - the cache does inside this function
  std::vector<G4TwoVector>* rawPoints = BDS::LoadAperturePoints(pointsFile, pointsUnit);
  
  // create a local copy
  vacuumEdge = std::vector<G4TwoVector>(rawPoints->begin(), rawPoints->end());

  // determine the winding order and if wrong, flip it
  // we rely on this order, so we calculate the outward facing normals correctly
  // further down - the sign of the pi/2
  auto detV1 = vacuumEdge[1] - vacuumEdge[0];
  auto detV2 = vacuumEdge[2] - vacuumEdge[1];
  G4double det = Determinant(detV1, detV2);
  
  if (det < 0)
    {std::reverse(vacuumEdge.begin(), vacuumEdge.end());}
  
  // calculate the unit vector for each point - so a direction from the 0,0 point to that point
  std::vector<G4TwoVector> vertexNormals;
  vertexNormals.reserve(vacuumEdge.size());
  
  // Loop round the polygon and calculate the normal of each segment
  // sum the normals of two segments and normalise to give an outwards
  // pointing unit vector at each vertex. We have a bit of if else for
  // the boundaries of the loop for back / front.
  G4TwoVector v1;
  G4TwoVector v2;
  G4TwoVector v3;
  for (G4int i = 0; i < (G4int)vacuumEdge.size(); i++)
    {
      if (i == 0)
        {
          v1 = vacuumEdge.back();
          v3 = vacuumEdge[i+1];
        }
      else if (i == (G4int)vacuumEdge.size() - 1)
        {
          v1 = vacuumEdge[i-1];
          v3 = vacuumEdge[0];
        }
      else
        {
          v1 = vacuumEdge[i-1];
          v3 = vacuumEdge[i+1];
        }
      v2 = vacuumEdge[i];
      
      auto normal1 = v2 - v1;
      auto normal2 = v3 - v2;
      normal1.rotate(-CLHEP::halfpi); // now it's a normal
      normal2.rotate(-CLHEP::halfpi);
      
      auto vertexNormal = (normal1 + normal2).unit();

      vertexNormals.push_back(vertexNormal); // outward pointing direction for expansion
    }
  
  for (G4int i = 0; i < (G4int)vacuumEdge.size(); i++)
    {
      auto ur = vertexNormals[i];
      beamPipeInnerEdge.emplace_back(       vacuumEdge[i] + ur*lengthSafetyLarge);
      beamPipeOuterEdge.emplace_back(       vacuumEdge[i] + ur*(lengthSafetyLarge   + beamPipeThickness));
      containerEdge.emplace_back(           vacuumEdge[i] + ur*(5*lengthSafetyLarge + beamPipeThickness));
      containerSubtractionEdge.emplace_back(vacuumEdge[i] + ur*(7*lengthSafetyLarge + beamPipeThickness));
      maximumRadius = std::max(maximumRadius, containerSubtractionEdge.back().mag());
    }
}

G4double BDSBeamPipeFactoryPointsFile::CalculateIntersectionRadius(G4double, G4double, G4double, G4double, G4double)
{
  return maximumRadius*1.01; // 1% margin on absolute size
}

G4double BDSBeamPipeFactoryPointsFile::Determinant(const G4TwoVector& v1,
                                                   const G4TwoVector& v2) const
{
  G4double det = v1.x()*v2.y() - v2.x()*v1.y();
  return det;
}