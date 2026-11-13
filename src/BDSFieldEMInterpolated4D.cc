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
#include "BDSFieldEMInterpolated4D.hh"
#include "BDSInterpolator4D.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <utility>

BDSFieldEMInterpolated4D::BDSFieldEMInterpolated4D(BDSInterpolator4D*   eInterpolatorIn,
						   BDSInterpolator4D*   bInterpolatorIn,
						   const G4Transform3D& offset,
						   G4double             eScalingIn,
						   G4double             bScalingIn):
  BDSFieldEMInterpolated(eInterpolatorIn, bInterpolatorIn, offset, eScalingIn, bScalingIn),
  eInterpolator(eInterpolatorIn),
  bInterpolator(bInterpolatorIn)
{;}

BDSFieldEMInterpolated4D::~BDSFieldEMInterpolated4D()
{
  delete eInterpolator;
  delete bInterpolator;
}

std::pair<G4ThreeVector,G4ThreeVector> BDSFieldEMInterpolated4D::GetField(const G4ThreeVector& position,
									  const G4double       t) const
{
  G4ThreeVector e = eInterpolator->GetInterpolatedValue(position[0],position[1],position[2],t) * EScaling();
  G4ThreeVector b = bInterpolator->GetInterpolatedValue(position[0],position[1],position[2],t) * BScaling();
  return std::make_pair(b,e);
}
