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
#ifndef BDSPRIMARYVERTEXINFORMATIONV_H
#define BDSPRIMARYVERTEXINFORMATIONV_H

#include "BDSPrimaryVertexInformation.hh"

#include "globals.hh"
#include "G4VUserPrimaryVertexInformation.hh"

#include <vector>

/**
 * @brief Full set of coordinates for association with primary vertex. Vector version.
 *
 * This stores a full set of coordinates in both the curvilinear and global
 * Cartesian coordinate frame that can be associated with the G4PrimaryVertex
 * for an event.
 *
 * This is similar to BDSPrimaryVertexInformation but stores a vector of all quantities
 * used in the case of a multi-particle vertex.
 * 
 * @author Laurie Nevay
 */

class BDSPrimaryVertexInformationV: public G4VUserPrimaryVertexInformation
{
public:
  BDSPrimaryVertexInformationV() = delete;
  explicit BDSPrimaryVertexInformationV(std::vector<BDSPrimaryVertexInformation>& verticesIn):
    vertices(verticesIn)
  {;}
  virtual ~BDSPrimaryVertexInformationV(){;}

  /// Required implementation by virtual base class.
  virtual void Print() const {;}

  std::vector<BDSPrimaryVertexInformation> vertices; ///< Full set of coordinates.

  inline G4int NVertices() const {return (G4int)vertices.size();}
};

#endif
