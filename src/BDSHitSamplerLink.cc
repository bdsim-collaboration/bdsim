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
#include "BDSHitSamplerLink.hh"

#include "G4Allocator.hh"
#include "G4Types.hh"

G4Allocator<BDSHitSamplerLink> BDSAllocatorSamplerLink;

BDSHitSamplerLink::BDSHitSamplerLink(G4int samplerIDIn,
				     const BDSParticleCoordsFull& coordsIn,
				     G4double momentumIn,
				     G4double massIn,
				     G4int    ZIn,
				     G4int    AIn,
				     G4double chargeIn,
				     G4int    pdgIDIn,
				     G4int    parentIDIn,
				     G4int    trackIDIn,
				     G4int    nElectronsIn):
  BDSHitSampler(samplerIDIn, coordsIn, momentumIn, massIn, chargeIn,
		0, pdgIDIn,parentIDIn,trackIDIn,0,0,nElectronsIn),
  Z(ZIn),
  A(AIn),
  eventID(0),
  externalParticleID(0),
  externalParentID(0)
{;}

BDSHitSamplerLink::~BDSHitSamplerLink()
{;}
