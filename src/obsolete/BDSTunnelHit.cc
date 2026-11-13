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
#include "BDSTunnelHit.hh"

#include "globals.hh" // geant4 types / globals

G4Allocator<BDSTunnelHit> BDSTunnelHitAllocator;

BDSTunnelHit::BDSTunnelHit(G4int    nCopyIn, 
			   G4double energyIn, 
			   G4double XIn, 
			   G4double YIn, 
			   G4double ZIn,
			   G4double SIn,
			   G4double xIn,
			   G4double yIn,
			   G4double zIn,
			   G4double rIn,
			   G4double thetaIn,
			   G4int    partIDIn, 
			   G4double weightIn, 
			   G4bool   precisionRegionIn, 
			   G4int    turnsTakenIn,
			   G4int    eventNoIn):
  energy(energyIn),
  X(XIn),
  Y(YIn),
  Z(ZIn),
  S(SIn),
  x(xIn),
  y(yIn),
  z(zIn),
  r(rIn),
  theta(thetaIn),
  weight(weightIn),
  copyNumber(nCopyIn),
  partID(partIDIn),
  precisionRegion(precisionRegionIn),
  turnsTaken(turnsTakenIn),
  eventNo(eventNoIn)
{;}

BDSTunnelHit::~BDSTunnelHit()
{;}
