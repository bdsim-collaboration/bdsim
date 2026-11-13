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
#include "BDSParticleCoords.hh"
#include "BDSParticleCoordsFull.hh"

#include <ostream>

BDSParticleCoordsFull::BDSParticleCoordsFull():
  s(0),
  totalEnergy(0),
  weight(1)
{;}

BDSParticleCoordsFull::BDSParticleCoordsFull(G4double xIn,
					     G4double yIn,
					     G4double zIn,
					     G4double xpIn,
					     G4double ypIn,
					     G4double zpIn,
					     G4double tIn,
					     G4double sIn,
					     G4double totalEnergyIn,
					     G4double weightIn):
  BDSParticleCoords(xIn, yIn, zIn, xpIn, ypIn, zpIn, tIn),
  s(sIn),
  totalEnergy(totalEnergyIn),
  weight(weightIn)
{;}

BDSParticleCoordsFull::BDSParticleCoordsFull(const BDSParticleCoords& localIn,
					     G4double                 sIn,
					     G4double                 totalEnergyIn,
					     G4double                 weightIn):
  BDSParticleCoords(localIn),
  s(sIn),
  totalEnergy(totalEnergyIn),
  weight(weightIn)
{;}

std::ostream& operator<< (std::ostream& out, BDSParticleCoordsFull const& p)
{
  p.Print(out);
  return out;
}

void BDSParticleCoordsFull::Print(std::ostream& out) const
{
  BDSParticleCoords::Print(out);
  out << "s: " << s << ", E (total): " << totalEnergy << ", weight: " << weight << G4endl;
}
