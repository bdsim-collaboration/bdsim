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
#ifndef BDSCRYSTALINFO_H
#define BDSCRYSTALINFO_H

#include "BDSCrystalType.hh"
#include "globals.hh"         // geant4 types / globals

#include "CLHEP/Units/SystemOfUnits.h"

class G4Material;

/**
 * @brief Holder for all information required to create a crystal.
 * 
 * Geant4 units are assumed by classes that use instances of this class.
 * This reduces the number of argument to constructors 
 * plus aggregates common tasks in the component factory.
 *
 * Note, compiler provided copy constructor is sufficient as only pointers
 * to materials owned by BDSMaterials are kept - shallow copy is required then.
 * 
 * @author Laurie Nevay
 */

class BDSCrystalInfo
{
public:
  /// No default constuctor.
  BDSCrystalInfo() = delete;
  
  /// Constructor to assign all members at once.
  BDSCrystalInfo(G4Material*     materialIn,
		 const G4String& dataIn,
		 BDSCrystalType  crystalTypeIn,
		 G4double        lengthXIn,
		 G4double        lengthYIn,
		 G4double        lengthZIn,
		 G4double        sizeAIn,
		 G4double        sizeBIn,
		 G4double        sizeCIn,
		 G4double        alphaIn,
		 G4double        betaIn,
		 G4double        gammaIn,
		 G4int           spaceGroupIn,
		 G4double        bendingAngleYAxisIn = 0,
		 G4double        bendingAngleZAxisIn = 0,
		 G4double        miscutAngleY        = 0);

  G4Material*    material; ///< Material.
  G4String       data;     ///< Potential data path.
  BDSCrystalType shape;    ///< Shape of geometry.
  G4double       lengthX;  ///< X dimension.
  G4double       lengthY;  ///< Y dimension.
  G4double       lengthZ;  ///< Z dimension.
  G4double       sizeA;
  G4double       sizeB;
  G4double       sizeC;
  G4double       alpha;
  G4double       beta;
  G4double       gamma;
  G4int          spaceGroup;
  G4double       bendingAngleYAxis; ///< Bending angle about Y axis.
  G4double       bendingAngleZAxis; ///< Bending angle about Z axis.
  G4double       miscutAngleY;

  G4double BendingRadiusHorizontal() const {return BendingRadius(lengthZ, bendingAngleYAxis);}
  G4double BendingRadiusVertical()   const {return BendingRadius(lengthY, bendingAngleZAxis);}

private:
  G4double BendingRadius(G4double length,
			 G4double angle) const;
};

#endif
