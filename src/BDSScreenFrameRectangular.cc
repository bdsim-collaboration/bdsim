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
#include "BDSMaterials.hh"
#include "BDSScreenFrameRectangular.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"


BDSScreenFrameRectangular::BDSScreenFrameRectangular(G4String      nameIn,
						     G4ThreeVector sizeIn,
						     G4TwoVector   windowSizeIn,
						     G4TwoVector   windowOffsetIn,
						     G4Material*   materialIn):
  BDSScreenFrame(nameIn, sizeIn, windowSizeIn, windowOffsetIn, materialIn),
  zeroRot(new G4RotationMatrix(0,0,0))
{
  Build();
}

BDSScreenFrameRectangular::~BDSScreenFrameRectangular()
{
  delete zeroRot;
}

void BDSScreenFrameRectangular::Build()
{
  G4double lenSaf = BDSGlobalConstants::Instance()->LengthSafety();
  //G4double tinyLenSaf = 1e-3*CLHEP::nm;

  G4Box* frameBox = new G4Box("frameBox",
			      size.x()/2.0,
			      size.y()/2.0,
			      size.z()/2.0);


  G4Box* windowBox = new G4Box("windowBox",
			       windowSize.x()/2.0 + lenSaf,
			       windowSize.y()/2.0 + lenSaf,
			       size.z()/2.0 + lenSaf); // must be thicker than frame.

	cavityPos = G4ThreeVector(windowOffset.x(), windowOffset.y(), 0);
	G4SubtractionSolid* frameSS = new G4SubtractionSolid((G4String)"frameSS", frameBox, windowBox, zeroRot, cavityPos);
	logVol = new G4LogicalVolume(frameSS, material, name+"_lv");
  SetVisAtts();
}

void BDSScreenFrameRectangular::Place(G4RotationMatrix* rot,
				      G4ThreeVector     pos,
				      G4LogicalVolume*  motherVol)
{
  new G4PVPlacement(rot,
		    pos,
		    logVol,
		    "screenFrame_pv",
		    motherVol,
		    false,
		    0,
		    checkOverlaps);

}

void BDSScreenFrameRectangular::SetVisAtts()
{
  BDSScreenFrame::SetVisAtts(logVol);
}
