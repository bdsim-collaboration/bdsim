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
#ifndef BDSCOLOURS_H
#define BDSCOLOURS_H

#include "globals.hh"

#include <map>
#include <tuple>

class G4Colour;

/**
 * @brief Colour class that holds all colours used in BDSIM
 */

class BDSColours
{
public:
  typedef std::tuple<G4double, G4double, G4double, G4double> BDSColourKey;

  static BDSColours* Instance(); ///< singleton pattern

  ~BDSColours();

  /// Get a cached anonymous colour by values.
  G4Colour* GetColour(G4double red,
                      G4double green,
                      G4double blue,
                      G4double alpha = 1);

  /// Get colour from name
  G4Colour* GetColour(const G4String& type,
                      G4bool normaliseTo255 = true);

  /// Define a new colour.
  void DefineColour(const G4String& name,
		    G4double red,
		    G4double green,
		    G4double blue,
		    G4double alpha = 1,
		    G4bool   normaliseTo255 = true);

  /// Print out a full list of colours using Sphinx rst table syntax. Looks
  /// nice and easy to regenerate table for the manual.
  void Print();

private:
  BDSColours(); ///< Private constructor as singleton
  static BDSColours* instance;

  /// A map of the magnet name for which order it is, which
  /// can be used to access the appropriate colour map
  std::map<G4int, G4String> magnetName;

  /// A map of the colour for each type or component by name
  std::map<G4String, G4Colour*> colours;

  /// A map of colours with no name. Can only be retrieved by
  /// a tuple of their values - an optimisation for repeated loading
  /// of the same colours in GDML that we don't care about naming.
  std::map<BDSColourKey, G4Colour*> anonymousColours;
};

#endif
