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
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSKicker.hh"

#include "BDSDipoleStepper.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetType.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetType.hh"
#include "BDSSbendMagField.hh"
#include "BDSUtilities.hh"

#include "G4ClassicalRK4.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4VPhysicalVolume.hh"

BDSKicker::BDSKicker(G4String            name,
		     G4double            length,
		     G4double            bFieldIn,
		     G4double            kickAngle,
		     BDSMagnetType       type,
		     BDSBeamPipeInfo*    beamPipeInfo,
		     BDSMagnetOuterInfo* magnetOuterInfo):
  BDSMagnet(type, name, length,
	    beamPipeInfo, magnetOuterInfo),
  bField(bFieldIn),
  kickAngle(kickAngle)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << type.ToString() << ", angle: " << kickAngle << G4endl;
#endif
}

void BDSKicker::Build()
{
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
      
      BuildOuterFieldManager(2, BFldIron,0);
    }
}

void BDSKicker::BuildBPFieldAndStepper()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // don't build field if angle is 0 as stepper intolerant of this and wast of memory and cpu
  if (BDS::IsFinite(kickAngle))
    {
      // set up the magnetic field and stepper
      // set magnetic field direction dependingon whether it's a vertical kicker or not
      G4ThreeVector vectorBField;
      if (magnetType == BDSMagnetType::vkicker)
	{vectorBField = G4ThreeVector(-bField, 0, 0);} // vertical kicker
      else
	{vectorBField = G4ThreeVector(0, bField, 0);} // must be horizontal kicker

#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Name: " << name << ", type: " << magnetType.ToString() << G4endl;
      G4cout << __METHOD_NAME__ << "Kick angle: " << kickAngle << " B: " << vectorBField << G4endl;
#endif
      
      itsMagField = new BDSSbendMagField(vectorBField,chordLength,kickAngle);
      itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);
      itsStepper  = new G4ClassicalRK4(itsEqRhs);
    }
#ifdef BDSDEBUG
  else
    {G4cout << __METHOD_NAME__ << "kick angle isn't finite - not building field" << G4endl;}
#endif
}
