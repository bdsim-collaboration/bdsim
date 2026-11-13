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
#include "G4VPhysicalVolume.hh"

#include <map>

//============================================================

BDSDrift::BDSDrift (G4String aName, 
		    G4double aLength, 
                    std::list<G4double> blmLocZ, 
		    std::list<G4double> blmLocTheta, 
		    G4double aperX, 
		    G4double aperY, 
		    G4String tunnelMaterial, 
		    G4bool   aperset, 
		    G4double aper, 
		    G4double tunnelOffsetX, 
		    G4double phiAngleIn, 
		    G4double phiAngleOut):
  BDSMultipole(aName, 
	       aLength, 
	       aper, 
	       aper, 
	       blmLocZ, 
	       blmLocTheta, 
	       tunnelMaterial, 
	       "", 
	       aperX, 
	       aperY, 
	       0, 
	       0, 
	       tunnelOffsetX, 
	       phiAngleIn, 
	       phiAngleOut),
  itsStartOuterR(0.0),
  itsEndOuterR(0.0),
  itsAperset(aperset)
{
  if(!itsAperset){
    itsStartOuterR=aperX + BDSGlobalConstants::Instance()->GetBeampipeThickness();
    itsEndOuterR=aperY + BDSGlobalConstants::Instance()->GetBeampipeThickness();
    SetStartOuterRadius(itsStartOuterR);
    SetEndOuterRadius(itsEndOuterR);
  }
}
BDSDrift::BDSDrift(G4String     name,
		   G4double     length,
		   beamPipeInfo beamPipeInfoIn,
		   G4double     boxSize,
		   G4String     outerMaterial,
		   G4String     tunnelMaterial,
		   G4double     tunnelRadius,
		   G4double     tunnelOffsetX):
  BDSMultipole(name,length,beamPipeInfoIn,boxSize,outerMaterial,tunnelMaterial,tunnelRadius,tunnelOffsetX)
{;}

void BDSDrift::Build() {
  BDSMultipole::Build();

  static G4VisAttributes* VisAtt1 = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
  VisAtt1->SetVisibility(false);
  VisAtt1->SetForceSolid(true);
  itsMarkerLogicalVolume->SetVisAttributes(VisAtt1);
}

void BDSDrift::BuildBPFieldAndStepper(){
    // set up the magnetic field and stepper
  itsMagField = new BDSMagField(); //Zero magnetic field.
  itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);
  itsStepper  = new BDSDriftStepper(itsEqRhs);
}

void BDSDrift::BuildBLMs(){
  itsBlmLocationR = std::max(itsStartOuterR, itsEndOuterR) - itsBpRadius;
  BDSAcceleratorComponent::BuildBLMs(); // resets itsBlmLocationR! -- JS
}

BDSDrift::~BDSDrift()
{
}
