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
//===================================================.

#ifndef BDSDetectorMessenger_h
#define BDSDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class BDSDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

//===================================================.

class BDSDetectorMessenger: public G4UImessenger
{
public:
  BDSDetectorMessenger(BDSDetectorConstruction* );
  ~BDSDetectorMessenger();
  
  void SetNewValue(G4UIcommand*, G4String);
  
private:
  BDSDetectorConstruction* BDSDetector;    

  //    G4UIdirectory*             BDSdetDir;
  //    G4UIcmdWithAString*        AbsMaterCmd;
  //    G4UIcmdWithAString*        GapMaterCmd;
  //    G4UIcmdWithADoubleAndUnit* AbsThickCmd;
  //    G4UIcmdWithADoubleAndUnit* GapThickCmd;
  //    G4UIcmdWithADoubleAndUnit* SizeYZCmd;
  //    G4UIcmdWithAnInteger*      NbLayersCmd;    
  //    G4UIcmdWithADoubleAndUnit* MagFieldCmd;
  //    G4UIcmdWithoutParameter*   UpdateCmd;
};

#endif

