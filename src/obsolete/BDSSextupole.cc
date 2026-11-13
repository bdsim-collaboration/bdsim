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
#include "BDSSextupole.hh"

#include "BDSGlobalConstants.hh" 
#include "BDSMagnet.hh"
#include "BDSMagnetType.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSSextMagField.hh"
#include "BDSSextStepper.hh"

#include "G4Mag_UsualEqRhs.hh"

BDSSextupole::BDSSextupole(G4String            name,
                           G4double            length,
                           G4double            bDblPrime,
                           BDSBeamPipeInfo*    beamPipeInfo,
                           BDSMagnetOuterInfo* magnetOuterInfo):
  BDSMagnet(BDSMagnetType::sextupole, name, length,
            beamPipeInfo, magnetOuterInfo),
  itsBDblPrime(bDblPrime)
{;}

void BDSSextupole::Build()
{
  BDSMagnet::Build();
  if(BDSGlobalConstants::Instance()->IncludeIronMagFields())
  {
    G4double polePos[4];
    G4double Bfield[3];
      
    //coordinate in GetFieldValue
    polePos[0]=-BDSGlobalConstants::Instance()->MagnetPoleRadius()*sin(CLHEP::pi/6);
    polePos[1]= BDSGlobalConstants::Instance()->MagnetPoleRadius()*cos(CLHEP::pi/6);
    polePos[2]=0.;
    polePos[3]=-999.;//flag to use polePos rather than local track
      
    itsMagField->GetFieldValue(polePos,Bfield);
    G4double BFldIron =
            sqrt(Bfield[0]*Bfield[0]+Bfield[1]*Bfield[1])*
                    BDSGlobalConstants::Instance()->MagnetPoleSize()/
                    (BDSGlobalConstants::Instance()->ComponentBoxSize()/2-
                     BDSGlobalConstants::Instance()->MagnetPoleRadius());
      
      // Magnetic flux from a pole is divided in two directions
      BFldIron/=2.;
      
      BuildOuterFieldManager(6, BFldIron,CLHEP::pi/6);
    }
}

void BDSSextupole::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  itsMagField = new BDSSextMagField(1*itsBDblPrime);
  itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);

  BDSSextStepper* sextStepper = new BDSSextStepper(itsEqRhs);
  sextStepper->SetBDblPrime(itsBDblPrime);
  itsStepper = sextStepper;
}
