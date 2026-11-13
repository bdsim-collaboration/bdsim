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
#ifndef BDSLENSFACET_H
#define BDSLENSFACET_H 

#include "globals.hh"
#include "G4LogicalVolume.hh"

#include "G4VisAttributes.hh"
#include "G4VSolid.hh"
#include "G4TwoVector.hh"

class BDSLensFacet 
{
public:
  BDSLensFacet(G4String name, G4TwoVector size, G4double angle, G4double baseHeight, G4String material);
  ~BDSLensFacet();
  inline G4LogicalVolume* log(){return _log;}
  inline G4String name(){return _name;}
  inline G4TwoVector size(){return _size;} //The x-y size of the facet.
  inline G4double angle(){return _angle;} 
  inline G4double totalHeight(){return _totalHeight;} //The total height (z size) of the facet.
  inline G4double baseHeight(){return _baseHeight;} //The thickness of the base.
  inline G4RotationMatrix* rotation(){return _rotation;}  //A rotation to rotate the facet to the correct coordinate system.

private:
  void computeDimensions();
  void build();
  void visAtt();

  G4String _name;
  G4TwoVector _size;
  G4double _angle;
  G4double _totalHeight;
  G4double _baseHeight;
  G4RotationMatrix* _rotation;
  G4String _logName;
  G4String _solidName;
  G4String _material;
  // Geometrical objects:
  G4LogicalVolume* _log;
  G4VSolid* _solid;
  G4VisAttributes* _visAtt;
  std::vector<G4ThreeVector> _facetPos;
};

#endif
