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
/** BDSIM, v0.1   

Last modified 15.11.2005 by Ilya Agapov

**/


//
//  Synchrotron radiation energy loss process
//

#include "BDSGlobalConstants.hh" 
#include "BDSContinuousSR.hh"
#include "G4ios.hh"
#include "CLHEP/Units/PhysicalConstants.h"

//--------------------------
// SYNCHROTRON RAD ***
G4double BDSLocalRadiusOfCurvature=DBL_MAX;// Used in Mean Free Path calc.
//--------------------------

BDSContinuousSR::BDSContinuousSR(const G4String& processName)
  : G4VDiscreteProcess(processName)
     // initialization
{

  G4cout<<"initializing contSR"<<G4endl;

  nExpConst=5*CLHEP::fine_structure_const/(2*sqrt(3.0))/CLHEP::electron_mass_c2;
  CritEngFac=3./2.*CLHEP::hbarc/pow(CLHEP::electron_mass_c2,3);

} 
 

G4VParticleChange* 
BDSContinuousSR::PostStepDoIt(const G4Track& trackData,
				     const G4Step& stepData)
{
  aParticleChange.Initialize(trackData);

  G4double eEnergy=trackData.GetTotalEnergy();

  G4double R=BDSLocalRadiusOfCurvature;

  G4double NewKinEnergy = trackData.GetKineticEnergy();
 
  G4double GamEnergy=0;

  aParticleChange.SetNumberOfSecondaries(0);

  if(fabs(R)>0) {
    G4double l = trackData.GetStep()->GetStepLength();
    const G4DynamicParticle* aParticle = trackData.GetDynamicParticle();
    G4double mass = aParticle->GetMass();
    G4double gamma = 1.e-3 * eEnergy / mass; // in 1.e3 units
    
    G4double r0 = 2.817940325e-3*CLHEP::m; // classical electron radius in 1..e-12 units

    // G4cout<<"mass="<<mass<<G4endl;
//     G4cout<<"energy="<<eEnergy<<G4endl;
//     G4cout<<"gamma="<<gamma<<G4endl;
//     G4cout<<"R="<<R<<G4endl;  
   
    // energy loss (in MeV)
    GamEnergy = l * (gamma * gamma * gamma * gamma ) * 2. * r0 * mass / ( 3. * R * R) ;
    
    //G4cout<<"constSr : poststepdoit, l= "<<l<<" energy loss =" << GamEnergy  <<" MeV"<<G4endl;
  }

  NewKinEnergy -= GamEnergy;

  if(GamEnergy>0)
    {
      
      if (NewKinEnergy > 0.)
	{
	  //
	  // Update the incident particle 
	  aParticleChange.ProposeEnergy(NewKinEnergy);
	} 
      else
	{ 
	  aParticleChange.ProposeEnergy( 0. );
	  aParticleChange.ProposeLocalEnergyDeposit (0.);
	  G4double charge= trackData.GetDynamicParticle()->GetCharge();
	  if (charge<0.) aParticleChange.ProposeTrackStatus(fStopAndKill);
	  else       aParticleChange.ProposeTrackStatus(fStopButAlive);
	}
      
    }
  
  return G4VDiscreteProcess::PostStepDoIt(trackData,stepData);
}


BDSContinuousSR::~BDSContinuousSR(){
}

