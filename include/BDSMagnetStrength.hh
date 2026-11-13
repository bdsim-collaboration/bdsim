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
#ifndef BDSMAGNETSTRENGTH_H
#define BDSMAGNETSTRENGTH_H

#include "globals.hh" // geant4 globals / types

#include <map>
#include <ostream>
#include <vector>

/**
 * @brief Efficient storage of magnet strengths.
 *
 * Based on std::map, this class stores magnet strengths. 
 *
 * As there will be one of these for every magnet and there are many parameters
 * (e.g. up to ~40 doubles), it is more efficient to only store the required
 * parameters. A pure quadrupole has no need of k20s, but a multipole may do. A static
 * vector contains the allowed keys that can be checked against before setting
 * the value. If a (valid) key is not specified in the map, its value is returned as 0.
 *
 * Angle in rad, Field in Geant4 units. k strengths as original (ie not converted to G4).
 * 
 * @author Laurie Nevay
 */

class BDSMagnetStrength
{
private:
  /// Typedefs up first so we can declare public iterators.
  typedef std::map<G4String, G4double> StrengthMap;

  /// A map containing the values for various magnet strengths. It only contains
  /// the set magnet strengths. If a strength isn't specified, it's assumed to be zero
  /// without having to store the 0 value.
  StrengthMap strengths;
  
public:
  /// Default constructor does nothing as class will return 0 for uninitialised keys
  BDSMagnetStrength(){;}

  /// This constructor allows instantiation with a map of keys and values.
  explicit BDSMagnetStrength(const std::map<G4String, G4double>& keyvalues);

  ~BDSMagnetStrength(){;}

  /// Output stream.
  friend std::ostream& operator<< (std::ostream &out, BDSMagnetStrength const &st);

  /// Print out each magnet strength value in order according to keys multiplied by
  /// the correct factors to return to the units on the input side of BDSIM rather than
  /// the Geant4 ones used internally.
  std::ostream& WriteValuesInSIUnitsForSuvey(std::ostream& out,
					     const G4int precision = 14) const;

  /// Structure for unit names and numerical factors from Geant4 to SI(ish).
  struct unitsFactors
  {
    G4String unit;
    G4double factor;
  };
  
  /// Accessors with array / map [] operator.
  G4double& operator[](const G4String& key);
  const G4double& operator[](const G4String& key) const;
  const G4double& at(const G4String& key) const {return (*this)[key];}

  /// Accessor to all keys.
  static const std::vector<G4String>& AllKeys() {return keys;}
  
  /// Whether or not the supplied key is a valid magnet strength parameter.
  static G4bool ValidKey(const G4String& key);

  /// Accessor to all units.
  static const std::map<G4String, unitsFactors>& UnitsAndFactors() {return unitsFactorsMap;}

  /// Accessor for normal component keys - k1 - k12.
  static inline std::vector<G4String> NormalComponentKeys() {return normalComponentKeys;}

  /// Accessor for skew component keys - k1 - k12.
  static inline std::vector<G4String> SkewComponentKeys() {return skewComponentKeys;}

  /// Accessor for all normal components - k1 - k12.
  std::vector<G4double> NormalComponents() const;

  /// Accessor for all skew components - k1 - k12.
  std::vector<G4double> SkewComponents() const;

  /// Access a unit name for a given key.
  static G4String UnitName(const G4String& key);

  /// Access a unit factor for a given key.
  static G4double Unit(const G4String& key);
  
  /// Whether a key has been set.
  G4bool KeyHasBeenSet(const G4String& key) const;
  
  ///@{ Iterator mechanics.
  typedef StrengthMap::iterator       iterator;
  typedef StrengthMap::const_iterator const_iterator;
  iterator       begin()       {return strengths.begin();}
  iterator       end()         {return strengths.end();}
  const_iterator begin() const {return strengths.begin();}
  const_iterator end()   const {return strengths.end();}
  G4bool         empty() const {return strengths.empty();}
  ///@}
  
private:
  /// Accessor similar to [] but without linear search through keys to check validity.
  /// for fast internal use.
  const G4double& GetValue(const G4String& key) const;

  /// Vector of the allowed strength parameters.
  static const std::vector<G4String> keys;

  /// Map of each unit name and numerical factor to each key.
  static const std::map<G4String, unitsFactors> unitsFactorsMap;

  /// Vector of the normal component strength parameters.
  static const std::vector<G4String> normalComponentKeys;

  /// Vector of the normal component strength parameters.
  static const std::vector<G4String> skewComponentKeys;

  /// Keep a single copy of 0.0 as it needs to be returned as a reference not a value.
  static const G4double zero;

  /// Dummy variable that can be overwritten.
  static G4double variable;
};


#endif
