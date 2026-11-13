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
#ifndef BDSVISCOMMANDSCENEADDQUERYMAGNETICFIELD_H
#define BDSVISCOMMANDSCENEADDQUERYMAGNETICFIELD_H

#include "G4String.hh"
#include "G4Version.hh"
#include "G4VisCommandsScene.hh"

class BDSDetectorConstruction;
class G4UIcommand;

/**
 * @brief Scene describer for adding magnetic field lines according to a query.
 * 
 * @author Laurie Nevay
 */

class BDSVisCommandSceneAddQueryMagneticField: public G4VVisCommandScene
{
public:
  BDSVisCommandSceneAddQueryMagneticField(const BDSDetectorConstruction* realWorldIn);
  virtual ~BDSVisCommandSceneAddQueryMagneticField();

  virtual G4String GetCurrentValue(G4UIcommand* command);
  virtual void SetNewValue(G4UIcommand* command, G4String newValue);

private:
  const BDSDetectorConstruction* realWorld;
  G4UIcommand* command;

#if G4VERSION_NUMBER < 1060
  /// Simplified copy of G4VVisCommands::CheckSceneAndNotifyHandlers from G4 V6 and upwards.
  void CheckSceneAndNotifyHandlers(G4Scene* scene);
#endif
};

#endif
