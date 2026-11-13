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
#ifndef BDSIONDEFINITION_H
#define BDSIONDEFINITION_H

#include "globals.hh"

#include <ostream>

/**
 * @brief Class to parse an ion particle definition.
 *
 * @author Laurie Nevay
 */

class BDSIonDefinition
{
public:
  explicit BDSIonDefinition(const G4String& definition);
  BDSIonDefinition(G4int    aIn,
		   G4int    zIn,
		   G4double qIn);
  ~BDSIonDefinition(){;}

  /// @{ Accessor.
  inline G4int    A()                const {return a;}
  inline G4int    Z()                const {return z;}
  inline G4double Charge()           const {return charge;}
  inline G4double ExcitationEnergy() const {return energy;}
  inline G4bool   OverrideCharge()   const {return overrideCharge;}
  inline G4int    NElectrons()       const {return nElectrons;}
  /// @}

  /// Output streamer.
  friend std::ostream& operator<< (std::ostream& out, BDSIonDefinition const& io);

private:
  /// No default constructor.
  BDSIonDefinition() = delete;

  /// Parse the definition.
  void Parse(const G4String& definition);

  G4int    a;
  G4int    z;
  G4double charge;         ///< In units of eplus
  G4double energy;         ///< Excitation energy. G4units.
  G4bool   overrideCharge; ///< Whether to specify the charge.
  G4int    nElectrons;     ///< Inferred number of bound electrons if any.
};

#endif
