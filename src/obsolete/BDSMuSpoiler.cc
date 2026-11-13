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

#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include "BDSBeamPipeInfo.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetType.hh"
#include "BDSMagnet.hh"
#include "BDSMuSpoiler.hh"

class BDSTiltOffset;

BDSMuSpoiler::BDSMuSpoiler(G4String            name,
			   G4double            length,
			   G4double            /*bField*/,
			   BDSBeamPipeInfo*    beamPipeInfo,
			   BDSMagnetOuterInfo* magnetOuterInfo):
  BDSMagnet(BDSMagnetType::muonspoiler, name, length,
	    beamPipeInfo, magnetOuterInfo)
{;}
