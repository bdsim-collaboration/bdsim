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
//BackScatterBias - particles being backscattered are split

#include "BDSBackScatterBias.hh"

BDSBackScatterBias::BDSBackScatterBias(const G4String& aName,
                                         G4ProcessType   aType)
  : G4WrapperProcess(aName, aType), eFactor(1.0)
{
}

BDSBackScatterBias::BDSBackScatterBias(const BDSBackScatterBias& right)
  : G4WrapperProcess(right), eFactor(right.eFactor)
{
}

BDSBackScatterBias::~BDSBackScatterBias()
{
}



G4VParticleChange* BDSBackScatterBias::PostStepDoIt(
                                                   const G4Track& track,
                                                   const G4Step&  stepData
                                                   )
{
#ifdef BDSDEBUG
  G4cout <<" ###PostStepDoIt " << G4endl;
  G4cout << "BDSBackScatterBias::PostStepDoit  Getting pChange" << G4endl;
#endif
  G4VParticleChange* pChange = pRegProcess->PostStepDoIt( track, stepData );
  pChange->SetVerboseLevel(0);
#ifdef BDSDEBUG
  G4cout << "BDSBackScatterBias::PostStepDoit Choosing setsecondaryweightbyprocess" << G4endl;
#endif
  pChange->SetSecondaryWeightByProcess(true);
  pChange->SetParentWeightByProcess(true);
#ifdef BDSDEBUG
  G4cout << "BDSBackScatterBias::PostStepDoit Getting parent weight" << G4endl;
#endif
  G4double w =  pChange->GetParentWeight();
  G4double ws = w / eFactor;
  //  G4double survivalProb = w - ws;
  
#ifdef BDSDEBUG
  G4cout << "BDSBackScatterBias::PostStepDoit Getting number of secondaries" << G4endl;
#endif
  G4int iNSec = pChange->GetNumberOfSecondaries();

#ifdef BDSDEBUG  
  G4cout << "BDSBackScatterBias::PostStepDoit Setting secondary weights" << G4endl;
#endif

  for (G4int i = 0; i < iNSec; i++) {
    pChange->GetSecondary(i)->SetWeight(ws); 
  }
  
#ifdef BDSDEBUG  
  G4cout << "BDSBackScatterBias::PostStepDoIt number of secondaries: " << pChange->GetNumberOfSecondaries() << G4endl;
#endif
  return pChange;
}

