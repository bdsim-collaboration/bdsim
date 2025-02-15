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
#ifndef BDSFIELDMAGDIPOLEHARDEDGE_H
#define BDSFIELDMAGDIPOLEHARDEDGE_H

#include "BDSFieldMag.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

class BDSMagnetStrength;

/**
 * @brief This class represents a hard-edge dipole field.
 * The field is calculated in cartesian coordinates.
 * 
 * @author Paul Bogdan Jurj
 */

class BDSFieldMagDipoleHardEdgeMuonCooler: public BDSFieldMag
{
public:
  BDSFieldMagDipoleHardEdgeMuonCooler() = delete;


  BDSFieldMagDipoleHardEdgeMuonCooler(G4double strength,
                        G4double apertureRadius,
                        G4double coilLength
                        );

  virtual ~BDSFieldMagDipoleHardEdgeMuonCooler(){;}

  /// Calculate the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				                 const G4double       t = 0) const;

  /// @{ Accessor.
  inline G4double GetB0() const {return B0;}
  /// @}

private:
  /// Private default constructor to ensure use of supplied constructor
  
  G4double D;
  G4double halfLength;
  G4double B0;
};

#endif
