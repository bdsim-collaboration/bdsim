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
#ifndef BDSCOLOURFROMMATERIAL_H
#define BDSCOLOURFROMMATERIAL_H
#include "G4String.hh"

#include <map>

class G4Colour;
class G4Material;
class G4PhysicsFreeVector;

/**
 * @brief Automatic colours from materials.
 * 
 * @author Laurie Nevay
 */

class BDSColourFromMaterial
{
public:
  static BDSColourFromMaterial* Instance(); ///< Singleton pattern.

  ~BDSColourFromMaterial();

  /// Get colour from name
  G4Colour* GetColour(const G4Material* material,
                      const G4String& prefixToStripFromName = "");
  
  /// Get colour from name - if not found return the supplied default.
  G4Colour* GetColourWithDefault(const G4Material* material,
                                 G4Colour* defaultIn) const;

private:
  BDSColourFromMaterial(); ///< Private constructor as singleton
  static BDSColourFromMaterial* instance;

  std::map<G4String, G4Colour*> defines; ///< Specially defined material colours.
  G4PhysicsFreeVector* generalDensity;
};

#endif
