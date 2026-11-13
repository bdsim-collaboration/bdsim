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
#ifndef BDSUICMDSTRINGINT_H
#define BDSUICMDSTRINGINT_H

#include "G4UIcommand.hh"

class G4UImessenger;

/**
 * @brief UI command with string, then optional integer argument.
 *
 * @author Laurie Nevay
 */

class BDSUIcmdStringInt: public G4UIcommand
{
public:
  BDSUIcmdStringInt() = delete;
  BDSUIcmdStringInt(const char* theCommandPath, G4UImessenger* theMessenger);
  virtual ~BDSUIcmdStringInt() = default;

  /// Set the names in the parameter guidnace with tab complete.
  void SetParameterName(const char* theName0,
			const char* theName1,
			G4bool omittable,
			G4bool currentAsDefault = false);

  /// @{ Unused - copied from example.
  void SetCandidates(const char* candidateList);
  void SetDefaultValue(const char* defVal);
  /// @}
};

#endif
