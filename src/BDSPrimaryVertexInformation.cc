/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSParticleCoordsFullGlobal.hh"
#include "BDSParticleDefinition.hh"
#include "BDSPrimaryVertexInformation.hh"

#include "globals.hh"
#include "G4ParticleDefinition.hh"

#include "CLHEP/Units/PhysicalConstants.h"

BDSPrimaryVertexInformation:: BDSPrimaryVertexInformation(const BDSParticleCoordsFullGlobal& primaryVertexIn,
                                                          const BDSParticleDefinition*       particle):
  primaryVertex(primaryVertexIn),
  momentum(0),
  charge(particle->Charge()),
  rigidity(0),
  mass(particle->Mass()),
  pdgID(particle->ParticleDefinition()->GetPDGEncoding()),
  nElectrons(particle->NElectrons())
{
  // temporarily construct one of our particle definitions to calculate momentum and rigidity
  // for this particular particle. Note, "particle" is the general bunch particle, not the individual coords.
  auto thisParticle = new BDSParticleDefinition(particle->ParticleDefinition(),
						primaryVertexIn.local.totalEnergy, 0, 0,
						particle->FFact(),
						particle->IonDefinition());
  momentum = thisParticle->Momentum();
  rigidity = thisParticle->BRho();
  delete thisParticle;
}

BDSPrimaryVertexInformation::BDSPrimaryVertexInformation(const BDSParticleCoordsFullGlobal& primaryVertexIn,
							 G4double momentumIn,
							 G4double chargeIn,
							 G4double rigidityIn,
							 G4double massIn,
							 G4int    pdgIDIn,
							 G4int    nElectronsIn):
  primaryVertex(primaryVertexIn),
  momentum(momentumIn),
  charge(chargeIn),
  rigidity(rigidityIn),
  mass(massIn),
  pdgID(pdgIDIn),
  nElectrons(nElectronsIn)
{;}

void BDSPrimaryVertexInformation::Print() const
{
  G4cout << primaryVertex << G4endl;
  G4cout << "Primary particle charge " << charge / CLHEP::eplus << G4endl;
}
