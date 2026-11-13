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
#include "BDSDimensionType.hh"
#include "BDSFieldMagInterpolated1D.hh"
#include "BDSInterpolator1D.hh"

#include "G4ThreeVector.hh"

BDSFieldMagInterpolated1D::BDSFieldMagInterpolated1D(BDSInterpolator1D*   interpolatorIn,
						     const G4Transform3D& offset,
						     G4double             scalingIn):
  BDSFieldMagInterpolated(interpolatorIn, offset, scalingIn),
  interpolator(interpolatorIn),
  dimensionIndex((interpolatorIn->FirstDimension()).underlying()),
  time((interpolatorIn->FirstDimension()).underlying() > 2)
{;}

BDSFieldMagInterpolated1D::~BDSFieldMagInterpolated1D()
{
  delete interpolator;
}

G4ThreeVector BDSFieldMagInterpolated1D::GetField(const G4ThreeVector& position,
						  const G4double       t) const
{
  G4double coordinate = 0;
  if (time)
    {coordinate = t;}
  else
    {coordinate = position[dimensionIndex];}
  return interpolator->GetInterpolatedValue(coordinate) * Scaling();
}
