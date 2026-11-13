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
#include "BDSFieldEInterpolated2D.hh"
#include "BDSInterpolator2D.hh"

#include "G4ThreeVector.hh"

BDSFieldEInterpolated2D::BDSFieldEInterpolated2D(BDSInterpolator2D*   interpolatorIn,
						 const G4Transform3D& offset,
						 G4double             eScalingIn):
  BDSFieldEInterpolated(interpolatorIn, offset, eScalingIn),
  interpolator(interpolatorIn),
  firstDimensionIndex((interpolatorIn->FirstDimension()).underlying()),
  firstTime((interpolatorIn->FirstDimension()).underlying() > 2),
  secondDimensionIndex((interpolatorIn->SecondDimension()).underlying()),
  secondTime((interpolatorIn->SecondDimension()).underlying() > 2)
{;}

BDSFieldEInterpolated2D::~BDSFieldEInterpolated2D()
{
  delete interpolator;
}

G4ThreeVector BDSFieldEInterpolated2D::GetField(const G4ThreeVector& position,
						const G4double       t) const
{
  G4double fCoordinate = 0;
  if (firstTime)
    {fCoordinate = t;}
  else
    {fCoordinate = position[firstDimensionIndex];}
  G4double sCoordinate = 0;
  if (secondTime)
    {sCoordinate = t;}
  else
    {sCoordinate = position[secondDimensionIndex];}
  return interpolator->GetInterpolatedValue(fCoordinate, sCoordinate) * EScaling();
}
