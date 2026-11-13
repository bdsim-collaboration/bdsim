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
#include "BDSXSBiasPhysics.hh"
#include "G4GammaConversionToMuons.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "BDSVProcess.hh"

BDSXSBiasPhysics::BDSXSBiasPhysics(G4String name):G4VPhysicsConstructor(name),_wasActivated(false)
{;}

BDSXSBiasPhysics::~BDSXSBiasPhysics()
{;}

void BDSXSBiasPhysics::ConstructParticle(){;}

void BDSXSBiasPhysics::ConstructProcess(){;}


//Deactivates an old process with the same name as proc1 and inserts a new one, or just inserts the new one if the old one is not present.
void BDSXSBiasPhysics::ReplaceDiscreteProcess(G4ProcessManager* pmanager, G4VProcess* proc1, G4VProcess* proc2){
  G4ProcessVector* processVector = pmanager->GetProcessList();
  //  G4int processIndex=-1;
//Replace the existing gammaconversiontomuons process with our wrappered process;
  for(int i=0; i < processVector->entries(); ++i){
    G4VProcess* tempProc = processVector->removeAt(i);
    if(tempProc->GetProcessName() != (proc1)->GetProcessName()){
      processVector->insertAt(i,tempProc);
      //  pmanager->SetProcessActivation(processVector->index(processVector[i]),false);
    } else {
      pmanager->AddDiscreteProcess(proc2);
      return;
    }
  }
}
