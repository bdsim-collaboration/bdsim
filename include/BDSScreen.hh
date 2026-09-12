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
#ifndef BDSSCREEN_H
#define BDSSCREEN_H

#include "BDSDrift.hh"

#include "globals.hh"
#include "G4TwoVector.hh"

#include <list>

class BDSBeamPipeInfo;
class BDSMultilayerScreen;

/**
 * @brief A multilayer screen in a beam pipe.
 *
 * @author Lawrence Deacon
 */

class BDSScreen: public BDSDrift
{
public:
  BDSScreen(G4String         name,  
	    G4double         chordength,  // Length of the drift section containing the screen
	    BDSBeamPipeInfo* beamPipeInfo,// Beam pipe specifications.
	    G4TwoVector      size = G4TwoVector(), // X Y dimensions of screen
	    G4double         screenAngle = 0);     //Phi angle of screen

  virtual ~BDSScreen();

  /// Add a screen layer.
  void AddScreenLayer(G4double thickness, G4String material, G4int isSampler=0);

private:
  BDSMultilayerScreen* mlScreen;
  std::list<G4double> layerThicknesses;
  G4TwoVector size;
  G4double screenAngle;
  std::list<G4String> layerMaterials;
  
  virtual void Build();
  void PlaceScreen();
  G4ThreeVector screenPos;
  G4RotationMatrix* screenRot;
  G4int nLayers;
};

#endif
