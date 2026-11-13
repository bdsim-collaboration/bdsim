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
#ifndef BDSMODULATORINFO_H
#define BDSMODULATORINFO_H

#include "BDSModulatorType.hh"

#include "G4Types.hh"

/**
 * @brief Holder class for all information required to describe a modulator.
 * 
 * @author Laurie Nevay
 */

class BDSModulatorInfo
{
public:
  /// Deleted default constructor to ensure one of supplied constructors is used.
  BDSModulatorInfo() = delete;
  /// extra constructor to assign all members at once.
  BDSModulatorInfo(BDSModulatorType modulatorTypeIn,
		   G4double frequencyIn,
		   G4double phaseIn = 0,
		   G4double tOffsetIn = 0,
		   G4double scaleIn = 1.0,
		   G4double amplitudeOffsetIn = 0,
		   G4double T0In = 0,
		   G4double T1In = 0);

  ~BDSModulatorInfo() = default;
  
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSModulatorInfo const &info);

  ///@{ Public member for direct access
  BDSModulatorType modulatorType;
  G4double frequency;
  G4double phase;
  G4double tOffset;
  G4double scale;
  G4double amplitudeOffset;
  G4double T0;
  G4double T1;
  ///@}
  
  G4String nameOfParserDefinition;
};

#endif
