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
#include "BDSPhotonCounter.hh"
#include "G4OpticalPhoton.hh"
#include "G4ParticleDefinition.hh"

BDSPhotonCounter* BDSPhotonCounter::_instance = nullptr;

BDSPhotonCounter* BDSPhotonCounter::Instance(){
  if(_instance==nullptr) {
    _instance = new BDSPhotonCounter();
  }
  return _instance;
}

BDSPhotonCounter::BDSPhotonCounter(){
  _nPhotons=0;
  _energy=0;
}

BDSPhotonCounter::~BDSPhotonCounter(){
  _instance = nullptr;
}

void BDSPhotonCounter::countPhoton(const G4Track* aTrack){
  if(aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
    { // particle is optical photon                                                                                                                             
      if(aTrack->GetParentID()>0)
	{ // particle is secondary                                                                                                                                
	  _nPhotons++;
	  _energy+=aTrack->GetTotalEnergy();
	}
    }
}


