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
#include "BDSParticleCoordsCylindrical.hh"

#include <ostream>

BDSParticleCoordsCylindrical::BDSParticleCoordsCylindrical():
  r(0), z(0), phi(0),
  rp(0), zp(0), phip(0),
  T(0)
{;}

BDSParticleCoordsCylindrical::BDSParticleCoordsCylindrical(G4double rIn,
							   G4double zIn,
							   G4double phiIn,
							   G4double rpIn,
							   G4double zpIn,
							   G4double phipIn,
							   G4double tIn):
  r(rIn), z(zIn), phi(phiIn),
  rp(rpIn), zp(zpIn), phip(phipIn),
  T(tIn)
{;}

std::ostream& operator<< (std::ostream& out, BDSParticleCoordsCylindrical const& p)
{
  p.Print(out);
  return out;
}

void BDSParticleCoordsCylindrical::Print(std::ostream& out) const
{
  out <<   "r, z, phi: (" << r  << ", " << z  << ", " << phi  << ")";
  out << ", rp, zp, phip: (" << rp << ", " << zp << ", " << phip << ")";
  out << ", t: " << T << G4endl;
}
