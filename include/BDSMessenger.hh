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
#ifndef BDSMESSENGER_H
#define BDSMESSENGER_H

#include <string>

#include "globals.hh"
#include "G4UImessenger.hh"

class BDSUIcmdStringInt;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcommand;
class G4UIdirectory;

/**
 * @brief User interface commands.
 *
 * @author Stewart Boogert
 */

class BDSMessenger: public G4UImessenger
{
public:
  BDSMessenger();
  virtual ~BDSMessenger();
  
  void        SetNewValue(G4UIcommand* command, G4String newValue);
  void        BeamLineList();
  std::string BDSBeamlineElementToString(G4int iElement);
  void        ElementNameSearch(std::string name);
  void        GoToElement(const std::string& value);
  void        ElementTypeSearch(std::string type);
  void        SamplerList();
  void        ViewSamplers();
  std::string BDSSamplerToString(G4int iSampler);
  std::string BDSSamplerToString(std::string samplerName);
  void        ListQueries();

protected:
  G4UIdirectory*           bdsDirectory;
  G4UIdirectory*           bdsBeamlineDirectory;
  G4UIcmdWithoutParameter* beamlineListCmd;
  G4UIcmdWithAString*      elementNameSearchCmd;
  BDSUIcmdStringInt*       elementGoToCmd;
  G4UIdirectory*           bdsSamplersDirectory;
  G4UIcmdWithoutParameter* samplerListCmd;
  G4UIcmdWithoutParameter* samplerViewCmd;
  G4UIcmdWithoutParameter* queryListCmd;
};

#endif
