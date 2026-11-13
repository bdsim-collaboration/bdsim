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
//XSBias with artificially scaled cross section

#include "BDSXSBias.hh"

BDSXSBias::BDSXSBias(const G4String& aName,
		     G4ProcessType   aType)
  : G4WrapperProcess(aName, aType), _eFactor(1)
{
}

BDSXSBias::BDSXSBias(const BDSXSBias& right)
  : G4WrapperProcess(right)
{
  // what about _eFactor? JS
}

BDSXSBias::~BDSXSBias()
{
}

G4VParticleChange* BDSXSBias::PostStepDoIt(const G4Track& track,
					   const G4Step&  stepData)
{ 
#ifdef BDSDEBUG
  G4cout << "BDSXSBias::PostStepDoIt>" << G4endl;
  G4cout << "BDSXSBias::PostStepDoit> Getting pChange" << G4endl;
#endif
  G4VParticleChange* pChange = pRegProcess->PostStepDoIt( track, stepData );
  pChange->SetVerboseLevel(0);
#ifdef BDSDEBUG
  G4cout << "BDSXSBias::PostStepDoit> Choosing SetSecondaryWeightByProcess" << G4endl;
#endif
  pChange->SetSecondaryWeightByProcess(true);
  pChange->SetParentWeightByProcess(true);
#ifdef BDSDEBUG
  G4cout << "BDSXSBias::PostStepDoit> Getting parent weight" << G4endl;
#endif
  G4double w =  pChange->GetParentWeight();
  G4double ws = w / eFactor();
  G4double survivalProb = w - ws;
  
#ifdef BDSDEBUG
  G4cout << "BDSXSBias::PostStepDoit> Getting number of secondaries" << G4endl;
#endif
  G4int iNSec = pChange->GetNumberOfSecondaries();

#ifdef BDSDEBUG  
  G4cout << "BDSXSBias::PostStepDoit> Setting secondary weights" << G4endl;
#endif

  G4bool pionEvent = false;
  G4bool gammaInPionEvent= false;
  for (G4int i = 0; i < iNSec; i++) {
    pChange->GetSecondary(i)->SetWeight(ws); 
    if(std::abs(pChange->GetSecondary(i)->GetDefinition()->GetPDGEncoding())==211) {
      pionEvent=true;
    }
    if(std::abs(pChange->GetSecondary(i)->GetDefinition()->GetPDGEncoding())==22){
      if (pionEvent==true){
        gammaInPionEvent = true;
      }
    }
  }
  
  if(pionEvent){
    G4cout << "Pion event" << G4endl;
    if(gammaInPionEvent){
#ifdef BDSDEBUG      
      G4cout << "gammaInPionEvent" << G4endl;
#endif
    }
    else {
#ifdef BDSDEBUG      
      G4cout << "NO gammaInPionEvent" << G4endl;
#endif
    }
  }
  
  if (pionEvent){
    G4Track* secTrack[100];
    for (G4int i = 0; i < iNSec; i++) {
#ifdef BDSDEBUG      
      //      G4cout << "BDSXSBias::PostStepDoIt Correcting kinetic energy of pion" << G4endl;
#endif
      secTrack[i] = pChange->GetSecondary(i);
      //      G4double EkCorrected = secTrack[i]->GetDynamicParticle()->GetKineticEnergy()/1000;
      //      secTrack[i]->SetKineticEnergy(EkCorrected);
      secTrack[i]->SetWeight(ws);

    }
    pChange->Clear();
    pChange->SetNumberOfSecondaries(iNSec);  
    for(G4int i=0; i<iNSec; i++){
      pChange->AddSecondary(secTrack[i]);
    }
  }
  
#ifdef BDSDEBUG   
  G4cout << "BDSXSBias::PostStepDoit Testing for primary survival" << G4endl;
#endif
  if(G4UniformRand()<survivalProb){
    pChange->ProposeParentWeight(survivalProb);
    pChange->ProposeTrackStatus(fAlive);
  }
  
#ifdef BDSDEBUG  
  G4cout << "BDSXSBias::PostStepDoIt number of secondaries: " << pChange->GetNumberOfSecondaries() << G4endl;
#endif
  return pChange;
}

