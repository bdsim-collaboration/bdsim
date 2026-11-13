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
#include "BDSArray4DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator4DLinear.hh"
#include "BDSInterpolatorRoutines.hh"

#include "G4Types.hh"

BDSInterpolator4DLinear::BDSInterpolator4DLinear(BDSArray4DCoords* arrayIn):
  BDSInterpolator4D(arrayIn)
{;}

BDSInterpolator4DLinear::~BDSInterpolator4DLinear()
{;}

BDSFieldValue BDSInterpolator4DLinear::GetInterpolatedValueT(G4double x,
							     G4double y,
							     G4double z,
							     G4double t) const
{
  BDSFieldValue localData[2][2][2][2];
  G4double xFrac, yFrac, zFrac, tFrac;
  array->ExtractSection2x2x2x2(x, y, z, t, localData, xFrac, yFrac, zFrac, tFrac);
  return BDS::Linear4D(localData, xFrac, yFrac, zFrac, tFrac);
}
