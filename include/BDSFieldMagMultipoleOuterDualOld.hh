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
#ifndef BDSFIELDMAGMULTIPOLEOUTERDUALOLD_H
#define BDSFIELDMAGMULTIPOLEOUTERDUALOLD_H
#include "BDSFieldMag.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

class BDSFieldMagMultipoleOuterOld;
class BDSMagnetStrength;

/**
 * @brief Sum of two multipole fields spaced by a distance in x.
 *
 * @author Laurie Nevay
 */

class BDSFieldMagMultipoleOuterDualOld: public BDSFieldMag
{
public:
  BDSFieldMagMultipoleOuterDualOld(G4int              orderIn,
			       G4double           poleTipRadius,
			       const BDSFieldMag* innerFieldIn,
			       G4bool             kPositive,
			       G4double           separation,
			       G4bool             secondFieldOnLeft = true,
                                   G4double           arbitraryScaling = 1.0);

  virtual ~BDSFieldMagMultipoleOuterDualOld();

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const double         t = 0) const;

private:
  BDSFieldMagMultipoleOuterOld* fieldBase;
  G4ThreeVector offset;
};

#endif
