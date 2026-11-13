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
#include "BDSArray2DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator2DCubic.hh"
#include "BDSInterpolatorRoutines.hh"

#include "G4Types.hh"

BDSInterpolator2DCubic::BDSInterpolator2DCubic(BDSArray2DCoords* arrayIn):
  BDSInterpolator2D(arrayIn)
{;}

BDSInterpolator2DCubic::~BDSInterpolator2DCubic()
{;}

BDSFieldValue BDSInterpolator2DCubic::GetInterpolatedValueT(G4double x, G4double y) const
{
  BDSFieldValue localData[4][4];
  G4double xFrac, yFrac;
  array->ExtractSection4x4(x, y, localData, xFrac, yFrac);
  return BDS::Cubic2D(localData, xFrac, yFrac);
}
