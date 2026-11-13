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
#include "BDSGlobalConstants.hh" 
#include "BDSMultiFacetLayer.hh"
#include "BDSMaterials.hh"
#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               

//============================================================
BDSMultiFacetLayer::BDSMultiFacetLayer (G4String nameIn, BDSLensFacet* facet, G4double gapWidth, G4double nFacets):
  _facet(facet), _gapWidth(gapWidth), _nFacets(nFacets)
{
  _gapWidth=0;
  name = nameIn+"_MultiFacetLayer";
  computeDimensions();
  build();
}

void BDSMultiFacetLayer::computeDimensions(){
  size.setZ(_facet->totalHeight());
  size.setY(_facet->size().y());
  size.setX(_nFacets*_facet->size().x() + 
	    (_nFacets-1)*_gapWidth);
  //Compute positions of the facets relative to the mother volume.
  G4double x=0,y=0,z=0;
  for(double i=0; i<_nFacets; i++){
    x=(i+0.5)*_facet->size().x()-size.x()/2.0+i*_gapWidth;
    _facetPos.push_back(G4ThreeVector(x,y,z));
  }
}

void BDSMultiFacetLayer::build(){
  buildScreen();
  placeFacets();
}


void BDSMultiFacetLayer::buildScreen(){
  solid  = new G4Box((name+"_solid").c_str(),size.x()/2.0,size.z()/2.0,size.y()/2.0);
  log = new G4LogicalVolume(solid,BDSMaterials::Instance()->GetMaterial(
          BDSGlobalConstants::Instance()->VacuumMaterial()),(name+"_log").c_str(),0,0,0);
  G4VisAttributes* visAtt = new G4VisAttributes();
  visAtt->SetColor(G4Colour(0.7,0.2,0.0,0.3));
  visAtt->SetForceSolid(true);
  log->SetVisAttributes(visAtt);
}

void BDSMultiFacetLayer::placeFacets(){
  //Place the facets in the mother volume at the locations in _facetPos.
  
  for(int i=0; i<_nFacets; i++){
    new G4PVPlacement(_facet->rotation(), //The facet (trapezoid) must be rotated into the correct coordinate system (z normal to base).
		      _facetPos[i],
		      _facet->log(),
		      _facet->name(),
		      log,
		      true,
		      i,
		      false);
  }
}

BDSMultiFacetLayer::~BDSMultiFacetLayer(){
}
