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
#ifndef BDSFIELDMAGDIPOLEQUADRUPOLE_H
#define BDSFIELDMAGDIPOLEQUADRUPOLE_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSMagnetStrength;
class BDSFieldMagDipole;
class BDSFieldMagDipoleQuadrupole;
class BDSFieldMagQuadrupole;

/**
 * @brief Class that provides the magnetic strength in a mixed dipole / quadrupole.
 * 
 * This is the linear sum of a dipole and quadrupolar field (as defined by k1).
 *
 * @author Laurie Nevay
 */

class BDSFieldMagDipoleQuadrupole: public BDSFieldMag
{
public:
  BDSFieldMagDipoleQuadrupole(BDSMagnetStrength const* strength,
			      G4double          const  brho);
  
  virtual ~BDSFieldMagDipoleQuadrupole();

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector &position,
				 const G4double       t = 0) const;

  virtual void SetTransform(const G4Transform3D& transformIn);
  
private:
  /// Private default constructor to force use of supplied constructor.
  BDSFieldMagDipoleQuadrupole();
  
  BDSFieldMagQuadrupole* quad;
  BDSFieldMagDipole*      dipole;
};

#endif
