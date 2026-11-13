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
#include "BDSModulatorInfo.hh"
#include "BDSModulatorType.hh"

#include "globals.hh"

#include <ostream>

BDSModulatorInfo::BDSModulatorInfo(BDSModulatorType modulatorTypeIn,
                                   G4double         frequencyIn,
                                   G4double         phaseIn,
                                   G4double         tOffsetIn,
                                   G4double         scaleIn,
                                   G4double         amplitudeOffsetIn,
                                   G4double         T0In,
                                   G4double         T1In):
  modulatorType(modulatorTypeIn),
  frequency(frequencyIn),
  phase(phaseIn),
  tOffset(tOffsetIn),
  scale(scaleIn),
  amplitudeOffset(amplitudeOffsetIn),
  T0(T0In),
  T1(T1In),
  nameOfParserDefinition("")
{;}

std::ostream& operator<< (std::ostream& out, BDSModulatorInfo const& info)
{
  out << "Parser definition name: \"" << info.nameOfParserDefinition << "\"\n";
  out << "modulator type : \"" << info.modulatorType << "\n";
  out << "frequency: " << info.frequency << "\n";
  out << "phase: " << info.phase << "\n";
  out << "tOffset: " << info.tOffset << "\n";
  out << "scale: " << info.scale << "\n";
  out << "amplitudeOffset: " << info.amplitudeOffset << "\n";
  out << "T0 " << info.T0 << "\n";
  out << "T1 " << info.T1 << "\n";
  out << G4endl;
  return out;
}
