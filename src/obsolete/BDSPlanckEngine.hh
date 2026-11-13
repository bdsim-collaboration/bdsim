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
#ifndef BDSPLANCKENGINE_H
#define BDSPLANCKENGINE_H

#include "G4ios.hh" 
#include "globals.hh"
#include "G4LorentzVector.hh"

#include "CLHEP/Units/PhysicalConstants.h"

class BDSPlanckEngine  
{ 
  public:
 
     BDSPlanckEngine(G4double aTemperature);
 
    ~BDSPlanckEngine();

     G4LorentzVector PerformPlanck();
     void SetTemperature(G4double aTemperature);

  protected:

  private:

  G4double PlanckSpectrum(G4double x);

private:
    G4double itsTemperature, kT;
    G4LorentzVector itsFourMom;

    G4double a,b,c,x1,x2,TotalArea,area1, area2, area3;

   const static G4int ntryMax = 10000000;

};


inline void BDSPlanckEngine::SetTemperature(G4double aTemperature)
{
 itsTemperature=aTemperature;
 kT=CLHEP::k_Boltzmann* itsTemperature;
}

inline G4double BDSPlanckEngine::PlanckSpectrum(G4double x)
{ return x*x/(exp(x)-1);}

#endif
