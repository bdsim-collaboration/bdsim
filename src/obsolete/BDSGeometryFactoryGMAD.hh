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
#ifndef BDSGEOMETRYFACTORYGMAD_H
#define BDSGEOMETRYFACTORYGMAD_H

#include "BDSGeometryFactoryBase.hh"

#include "globals.hh"

#include <fstream>
#include <map>
#include <vector>

class BDSGeometryExternal;
class G4Colour;
class G4LogicalVolume;
class G4VSolid;

/**
 * @brief Factory for GMAD geometry description.
 *
 * Originally part of BDSIM. Adapated for recent refactoring.
 */

class BDSGeometryFactoryGMAD: public BDSGeometryFactoryBase
{
public:
  BDSGeometryFactoryGMAD();
  virtual ~BDSGeometryFactoryGMAD(){;}

  /// Main interface overrides base class method to construct a file with
  /// optional colour mapping.
  virtual BDSGeometryExternal* Build(G4String componentName,
				     G4String fileName,
				     std::map<G4String, G4Colour*>* colourMapping    = nullptr,
				     G4bool                 autoColour               = true,
				     G4double               suggestedLength          = 0,
				     G4double               suggestedHorizontalWidth = 0,
				     std::vector<G4String>* vacuumBiasVolumeNames    = nullptr);
  
private:
  /// Simple there was an error print out.
  inline void Error() {G4cout << "ggmad: error in file format" << G4endl;}

  /// Read a single word from the ifstream.
  G4String GetWord(std::ifstream& infputf) const;

  /// Get a numerical parameter from the ifstream.
  void GetParameter(std::ifstream&  inputf,
		            G4double&       x,
		            const G4String& name,
		            const G4String& lastToken) const;

  /// Get a string parameter from the ifstream.
  void GetParameter(std::ifstream&  inputf,
		            G4String&       lval,
		            const G4String& name,
		            const G4String& lastToken) const;

  /// Construct logical volume, rotation and placement.
  void Finish(G4String         name,
	      G4VSolid*        solid,
	      G4String         materialName,
	      G4LogicalVolume* mother,
	      G4double         phi   = 0,
	      G4double         theta = 0,
	      G4double         psi   = 0,
	      G4double         x0 = 0,
	      G4double         y0 = 0,
	      G4double         z0 = 0);
};

#endif
