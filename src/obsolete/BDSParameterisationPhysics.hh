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
#ifndef BDSPARAMETERISATIONPHYSICS_H
#define BDSPARAMETERISATIONPHYSICS_H

#include "G4VPhysicsConstructor.hh"

class G4FastSimulationManagerProcess;

/** 
 * @brief G4 fast simulation manager constructor to allowed parameterised showers.
 */

class BDSParameterisationPhysics: public G4VPhysicsConstructor
{
public:
  BDSParameterisationPhysics();
  virtual ~BDSParameterisationPhysics(){;}
  void ConstructProcess();
  void ConstructParticle();
  
private:
  G4bool activated;
  G4FastSimulationManagerProcess* fastSimulationManagerProcess;
};

#endif
