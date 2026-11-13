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
#ifndef BDSMULTIFACETLAYER_H
#define BDSMULTIFACETLAYER_H 

#include "globals.hh"

#include "BDSScreenLayer.hh"
#include "BDSLensFacet.hh"
#include <vector>

class BDSMultiFacetLayer: public BDSScreenLayer 
{
public:
  BDSMultiFacetLayer(G4String name, BDSLensFacet* facet, G4double gapWidth, G4double nFacets);
  virtual ~BDSMultiFacetLayer();

private:
  void computeDimensions();
  void build();
  void buildScreen();
  void placeFacet(G4double xPos);
  void placeFacets();
  BDSLensFacet* _facet;
  std::vector<G4ThreeVector> _facetPos;
  G4double _gapWidth;
  G4double _nFacets;
};

#endif
