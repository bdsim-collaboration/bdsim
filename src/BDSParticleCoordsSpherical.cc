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
#include "BDSParticleCoordsSpherical.hh"

#include <ostream>

BDSParticleCoordsSpherical::BDSParticleCoordsSpherical():
  r(0), theta(0), phi(0),
  rp(0), thetap(0), phip(0),
  T(0)
{;}

BDSParticleCoordsSpherical::BDSParticleCoordsSpherical(G4double rIn,
						       G4double thetaIn,
						       G4double phiIn,
						       G4double rpIn,
						       G4double thetapIn,
						       G4double phipIn,
						       G4double tIn):
  r(rIn), theta(thetaIn), phi(phiIn),
  rp(rpIn), thetap(thetapIn), phip(phipIn),
  T(tIn)
{;}

std::ostream& operator<< (std::ostream& out, BDSParticleCoordsSpherical const& p)
{
  p.Print(out);
  return out;
}

void BDSParticleCoordsSpherical::Print(std::ostream& out) const
{
  out <<   "r, theta, phi: (" << r  << ", " << theta  << ", " << phi  << ")";
  out << ", rp, thetap, phip: (" << rp << ", " << thetap << ", " << phip << ")";
  out << ", t: " << T << G4endl;
}
