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
#include "BDSMagnetOuterFactoryNone.hh"

#include "BDSDebug.hh"
#include "BDSMagnetOuter.hh"

#include "globals.hh"                      // geant4 globals / types
#include "G4LogicalVolume.hh"

#include <cmath>
#include <utility>                         // for std::pair
#include <algorithm>                       // for std::max

BDSMagnetOuterFactoryNone::BDSMagnetOuterFactoryNone()
{;}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateSectorBend(G4String           /*name*/,
							    G4double           /*length*/,
							    const BDSBeamPipe* /*beamPipe*/,
							    G4double           /*containerLength*/,
							    const BDSMagnetOuterInfo* /*recipe*/)
{
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateRectangularBend(G4String           /*name*/,
								 G4double           /*length*/,
								 const BDSBeamPipe* /*beamPipe*/,
								 G4double           /*containerLength*/,
								 const BDSMagnetOuterInfo* /*recipe*/)
{
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateQuadrupole(G4String      /*name*/,
							    G4double      /*length*/,
							    BDSBeamPipe*  /*beamPipe*/,
							    G4double      /*containerLength*/,
							    const BDSMagnetOuterInfo* /*recipe*/)
{
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateSextupole(G4String      /*name*/,
							   G4double      /*length*/,
							   BDSBeamPipe*  /*beamPipe*/,
							   G4double      /*containerLength*/,
							   const BDSMagnetOuterInfo* /*recipe*/)
{
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateOctupole(G4String      /*name*/,
							  G4double      /*length*/,
							  BDSBeamPipe*  /*beamPipe*/,
							  G4double      /*containerLength*/,
							  const BDSMagnetOuterInfo* /*recipe*/)
{
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateDecapole(G4String      /*name*/,
							  G4double      /*length*/,
							  BDSBeamPipe*  /*beamPipe*/,
							  G4double      /*containerLength*/,
							  const BDSMagnetOuterInfo* /*recipe*/)
{
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateSolenoid(G4String      /*name*/,
							  G4double      /*length*/,
							  BDSBeamPipe*  /*beamPipe*/,
							  G4double      /*containerLength*/,
							  const BDSMagnetOuterInfo* /*recipe*/)
{
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateMultipole(G4String      /*name*/,
							   G4double      /*length*/,
							   BDSBeamPipe*  /*beamPipe*/,
							   G4double      /*containerLength*/,
							   const BDSMagnetOuterInfo* /*recipe*/)
{
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateRfCavity(G4String      /*name*/,
							  G4double      /*length*/,
							  BDSBeamPipe*  /*beamPipe*/,
							  G4double      /*containerLength*/,
							  const BDSMagnetOuterInfo* /*recipe*/)
{
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateMuonSpoiler(G4String      /*name*/,
							     G4double      /*length*/,
							     BDSBeamPipe*  /*beamPipe*/,
							     G4double      /*containerLength*/,
							     const BDSMagnetOuterInfo* /*recipe*/)
{
  return nullptr;
}

BDSMagnetOuter* BDSMagnetOuterFactoryNone::CreateKicker(G4String                  /*name*/,
							G4double                  /*length*/,
							const BDSBeamPipe*        /*beamPipe*/,
							G4double                  /*containerLength*/,
							const BDSMagnetOuterInfo* /*recipe*/,
							G4bool                    /*vertical*/)
{
  return nullptr;
}
