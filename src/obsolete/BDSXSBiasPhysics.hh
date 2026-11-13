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
#ifndef __BDS_XS_BIAS_PHYSICS_HH_
#define __BDS_XS_BIAS_PHYSICS_HH_

#include "G4VPhysicsConstructor.hh"
#include "BDSXSBias.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "BDSVProcess.hh"
#include "G4GammaConversionToMuons.hh"

class BDSXSBiasPhysics: public G4VPhysicsConstructor{
public:
  BDSXSBiasPhysics(G4String name);
  ~BDSXSBiasPhysics();
  virtual void ConstructProcess();
  virtual void ConstructParticle();

protected:
  G4bool _wasActivated;
  void ReplaceDiscreteProcess(G4ProcessManager* pmanager, G4VProcess* proc1, G4VProcess* proc2);
  
private:
  BDSXSBiasPhysics();

};
#endif
