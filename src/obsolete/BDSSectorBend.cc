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
#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"

#include "BDSSectorBend.hh"

#include "BDSDipoleStepper.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetType.hh"
#include "BDSSbendMagField.hh"

#include "G4Mag_UsualEqRhs.hh"

#include "globals.hh"             // geant4 types / globals

BDSSectorBend::BDSSectorBend(G4String            name,
			     G4double            arcLength,
			     G4double            angleIn,
			     G4double            bField,
			     G4double            bGrad,
			     BDSBeamPipeInfo*    beamPipeInfo,
			     BDSMagnetOuterInfo* magnetOuterInfo):
  BDSMagnet(BDSMagnetType::sectorbend, name, arcLength,
	    beamPipeInfo, magnetOuterInfo, angleIn),
  itsBField(bField),itsBGrad(bGrad)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "name:         " << name        << G4endl;
  G4cout << __METHOD_NAME__ << "angle:        " << angle       << G4endl;
  G4cout << __METHOD_NAME__ << "field:        " << itsBField   << G4endl;
  G4cout << __METHOD_NAME__ << "arc length:   " << arcLength   << G4endl;
  G4cout << __METHOD_NAME__ << "chord length: " << chordLength << G4endl;
#endif
}

void BDSSectorBend::Build()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnet::Build();
  
  if(BDSGlobalConstants::Instance()->IncludeIronMagFields())
    {
      G4double polePos[4];
      G4double Bfield[3];
      
      //coordinate in GetFieldValue
      polePos[0]=0.;
      polePos[1]= BDSGlobalConstants::Instance()->MagnetPoleRadius();
      polePos[2]=0.;
      polePos[3]=-999.;//flag to use polePos rather than local track
      
      itsMagField->GetFieldValue(polePos,Bfield);
      G4double BFldIron=
	sqrt(Bfield[0]*Bfield[0]+Bfield[1]*Bfield[1])*
            BDSGlobalConstants::Instance()->MagnetPoleSize()/
	(BDSGlobalConstants::Instance()->ComponentBoxSize()/2-
            BDSGlobalConstants::Instance()->MagnetPoleRadius());
      
      // Magnetic flux from a pole is divided in two directions
      BFldIron/=2.;
      
      BuildOuterFieldManager(2, BFldIron,CLHEP::halfpi);
    }
}

void BDSSectorBend::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  G4ThreeVector Bfield(0.,itsBField,0.);
  // B-Field constructed with arc length for radius of curvature
  itsMagField = new BDSSbendMagField(Bfield,arcLength,angle);
  itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);  
  BDSDipoleStepper* dipoleStepper = new BDSDipoleStepper(itsEqRhs);
  dipoleStepper->SetBField(itsBField);
  dipoleStepper->SetBGrad(itsBGrad);
  itsStepper = dipoleStepper;
}
