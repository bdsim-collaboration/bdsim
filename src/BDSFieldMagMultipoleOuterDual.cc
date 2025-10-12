/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSFieldMagMultipoleOuterDual.hh"
#include "BDSFieldMagMultipoleOuter.hh"

#include "G4ThreeVector.hh"

BDSFieldMagMultipoleOuterDual::BDSFieldMagMultipoleOuterDual(G4int              orderIn,
							     G4double           poleTipRadiusIn,
							     const BDSFieldMag* innerFieldIn,
							     G4bool             kPositive,
							     G4double           brho,
							     G4double           separation,
							     G4bool             secondFieldOnLeft,
                                 G4bool             secondFieldInverted,
							     G4double           arbitraryScaling):
  fieldBase(nullptr)
{
  fieldBase = new BDSFieldMagMultipoleOuter(orderIn, poleTipRadiusIn, innerFieldIn,
					    kPositive, brho, arbitraryScaling);
  G4double offsetX = secondFieldOnLeft ? -separation : separation;
  offset = G4ThreeVector(offsetX,0,0);
  invertSecondField = secondFieldInverted;
}

BDSFieldMagMultipoleOuterDual::~BDSFieldMagMultipoleOuterDual()
{
  delete fieldBase;
}

G4ThreeVector BDSFieldMagMultipoleOuterDual::GetField(const G4ThreeVector& position,
						      const G4double       t) const
{
  G4ThreeVector aSide      = fieldBase->GetField(position, t);
  G4ThreeVector shiftedPos = position + offset;
  G4ThreeVector bSide      = fieldBase->GetField(shiftedPos,t);
  if (invertSecondField)
  {
      bSide *= -1.0; // opposite to whatever 'a' side
  }
  G4ThreeVector result = aSide + bSide;
  return result;
}
