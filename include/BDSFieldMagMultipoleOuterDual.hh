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
#ifndef BDSFIELDMAGMULTIPOLEOUTERDUAL_H
#define BDSFIELDMAGMULTIPOLEOUTERDUAL_H
#include "BDSFieldMag.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

class BDSFieldMagMultipoleOuter;
class BDSMagnetStrength;

/**
 * @brief Sum of two multipole fields spaced by a distance in x.
 *
 * @author Laurie Nevay
 */

class BDSFieldMagMultipoleOuterDual: public BDSFieldMag
{
public:
  BDSFieldMagMultipoleOuterDual(G4int              orderIn,
				G4double           poleTipRadius,
				const BDSFieldMag* innerFieldIn,
				G4bool             kPositive,
				G4double           brho,
				G4double           separation,
				G4bool             secondFieldOnLeft = true,
                G4bool             secondFieldInverted = true,
				G4double           arbitraryScaling  = 1.0);
  
  virtual ~BDSFieldMagMultipoleOuterDual();

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const double         t = 0) const;

private:
  BDSFieldMagMultipoleOuter* fieldBase;
  G4ThreeVector offset;
  G4bool invertSecondField;
};

#endif
