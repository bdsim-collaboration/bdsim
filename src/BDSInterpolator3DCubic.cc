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
#include "BDSArray3DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator3DCubic.hh"
#include "BDSInterpolatorRoutines.hh"

#include "G4Types.hh"

BDSInterpolator3DCubic::BDSInterpolator3DCubic(BDSArray3DCoords* arrayIn):
  BDSInterpolator3D(arrayIn)
{;}

BDSInterpolator3DCubic::~BDSInterpolator3DCubic()
{;}

BDSFieldValue BDSInterpolator3DCubic::GetInterpolatedValueT(G4double x,
                                                            G4double y,
                                                            G4double z) const
{
  BDSFieldValue localData[4][4][4];
  G4double xFrac, yFrac, zFrac;
  array->ExtractSection4x4x4(x, y, z, localData, xFrac, yFrac, zFrac);
  return BDS::Cubic3D(localData, xFrac, yFrac, zFrac);
}
