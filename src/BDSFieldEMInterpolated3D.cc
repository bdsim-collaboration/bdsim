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
#include "BDSFieldEMInterpolated3D.hh"
#include "BDSInterpolator3D.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <utility>

BDSFieldEMInterpolated3D::BDSFieldEMInterpolated3D(BDSInterpolator3D*   eInterpolatorIn,
						   BDSInterpolator3D*   bInterpolatorIn,
						   const G4Transform3D& offset,
						   G4double             eScalingIn,
						   G4double             bScalingIn):
  BDSFieldEMInterpolated(eInterpolatorIn, bInterpolatorIn, offset, eScalingIn, bScalingIn),
  eInterpolator(eInterpolatorIn),
  bInterpolator(bInterpolatorIn),
  eFirstDimensionIndex((eInterpolatorIn->FirstDimension()).underlying()),
  eFirstTime((eInterpolatorIn->FirstDimension()).underlying() > 2),
  eSecondDimensionIndex((eInterpolatorIn->SecondDimension()).underlying()),
  eSecondTime((eInterpolatorIn->SecondDimension()).underlying() > 2),
  eThirdDimensionIndex((eInterpolatorIn->ThirdDimension()).underlying()),
  eThirdTime((eInterpolatorIn->ThirdDimension()).underlying() > 2),
  bFirstDimensionIndex((bInterpolatorIn->FirstDimension()).underlying()),
  bFirstTime((bInterpolatorIn->FirstDimension()).underlying() > 2),
  bSecondDimensionIndex((bInterpolatorIn->SecondDimension()).underlying()),
  bSecondTime((bInterpolatorIn->SecondDimension()).underlying() > 2),
  bThirdDimensionIndex((bInterpolatorIn->ThirdDimension()).underlying()),
  bThirdTime((bInterpolatorIn->ThirdDimension()).underlying() > 2)
{;}

BDSFieldEMInterpolated3D::~BDSFieldEMInterpolated3D()
{
  delete eInterpolator;
  delete bInterpolator;
}

std::pair<G4ThreeVector,G4ThreeVector> BDSFieldEMInterpolated3D::GetField(const G4ThreeVector& position,
									  const G4double       t) const
{
  G4double eFCoordinate = 0;
  if (eFirstTime)
    {eFCoordinate = t;}
  else
    {eFCoordinate = position[eFirstDimensionIndex];}
  G4double eSCoordinate = 0;
  if (eSecondTime)
    {eSCoordinate = t;}
  else
    {eSCoordinate = position[eSecondDimensionIndex];}
  G4double eTCoordinate = 0;
  if (eThirdTime)
    {eTCoordinate = t;}
  else
    {eTCoordinate = position[eThirdDimensionIndex];}
  G4double bFCoordinate = 0;
  if (bFirstTime)
    {bFCoordinate = t;}
  else
    {bFCoordinate = position[bFirstDimensionIndex];}
  G4double bSCoordinate = 0;
  if (bSecondTime)
    {bSCoordinate = t;}
  else
    {bSCoordinate = position[bSecondDimensionIndex];}
  G4double bTCoordinate = 0;
  if (bThirdTime)
    {bTCoordinate = t;}
  else
    {bTCoordinate = position[bThirdDimensionIndex];}
  
  G4ThreeVector e = eInterpolator->GetInterpolatedValue(eFCoordinate,
							eSCoordinate,
							eTCoordinate) * EScaling();
  G4ThreeVector b = bInterpolator->GetInterpolatedValue(bFCoordinate,
							bSCoordinate,
							bTCoordinate) * BScaling();
  return std::make_pair(b,e);
}
