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
#include "BDSInterpolator3D.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

BDSInterpolator3D::BDSInterpolator3D(BDSArray3DCoords* arrayIn):
  BDSInterpolator(arrayIn),
  array(arrayIn)
{;}

G4ThreeVector BDSInterpolator3D::GetInterpolatedValue(G4double x, G4double y, G4double z) const
{
  BDSFieldValue r = GetInterpolatedValueT(x,y,z);
  return G4ThreeVector(r.x(), r.y(), r.z());
}
