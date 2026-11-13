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
#include "BDSFieldMagMultipoleOuterDualOld.hh"
#include "BDSFieldMagMultipoleOuterOld.hh"

#include "G4ThreeVector.hh"

BDSFieldMagMultipoleOuterDualOld::BDSFieldMagMultipoleOuterDualOld(G4int              orderIn,
							   G4double           poleTipRadiusIn,
							   const BDSFieldMag* innerFieldIn,
							   G4bool             kPositive,
							   G4double           separation,
							   G4bool             secondFieldOnLeft,
                                                                   G4double           arbitraryScaling):
  fieldBase(nullptr)
{
  fieldBase = new BDSFieldMagMultipoleOuterOld(orderIn, poleTipRadiusIn, innerFieldIn, kPositive, arbitraryScaling);
  G4double offsetX = secondFieldOnLeft ? -separation : separation;
  offset = G4ThreeVector(offsetX,0,0);
}

BDSFieldMagMultipoleOuterDualOld::~BDSFieldMagMultipoleOuterDualOld()
{
  delete fieldBase;
}

G4ThreeVector BDSFieldMagMultipoleOuterDualOld::GetField(const G4ThreeVector& position,
						     const G4double       t) const
{
  G4ThreeVector aSide      = fieldBase->GetField(position, t);
  G4ThreeVector shiftedPos = position + offset;
  G4ThreeVector bSide      = fieldBase->GetField(shiftedPos,t);
  bSide *= -1.0; // opposite to whatever 'a' side is
  G4ThreeVector result = aSide + bSide;
  return result;
}
