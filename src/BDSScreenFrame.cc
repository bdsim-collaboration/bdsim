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
#include "BDSColours.hh"
#include "BDSGlobalConstants.hh"
#include "BDSScreenFrame.hh"

#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"

BDSScreenFrame::BDSScreenFrame(G4String      nameIn,
			       G4ThreeVector sizeIn,
			       G4TwoVector   windowSizeIn,
			       G4TwoVector   windowOffsetIn,
			       G4Material*   materialIn):
  name(nameIn),
  size(sizeIn),
  windowSize(windowSizeIn),
  windowOffset(windowOffsetIn),
  material(materialIn),
  logVol(nullptr)
{
  checkOverlaps = BDSGlobalConstants::Instance()->CheckOverlaps();
  SetDefaultVisAtts();
}

BDSScreenFrame::~BDSScreenFrame()
{
  delete logVol;
  delete visAtt;
}

void BDSScreenFrame::SetDefaultVisAtts()
{
  visAtt = new G4VisAttributes(BDSColours::Instance()->GetColour("screenframe"));
	visAtt->SetForceWireframe(true);
  visAtt->SetVisibility(true);
}

void BDSScreenFrame::SetVisAtts(G4LogicalVolume* logVolIn,
				G4VisAttributes* visAttIn)
{
  if (logVolIn)
    {
      if (!visAttIn)
	{logVolIn->SetVisAttributes(visAtt);}
      else
	{logVolIn->SetVisAttributes(visAttIn);}
    }
}

void BDSScreenFrame::SetVisAtts()
{
  SetVisAtts(logVol);
}
