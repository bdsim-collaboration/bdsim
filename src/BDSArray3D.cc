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
#include "BDSArray3D.hh"

#include "G4Types.hh"

#include <ostream>

BDSArray3D::BDSArray3D(G4int nXIn, G4int nYIn, G4int nZIn):
  BDSArray4D(nXIn,nYIn,nZIn,1)
{;}

std::ostream& operator<< (std::ostream& out, BDSArray3D const &a)
{
  return a.Print(out);
}
