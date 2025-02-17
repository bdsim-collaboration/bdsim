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
#ifndef BDSFIELDMAGDIPOLEENGE_H
#define BDSFIELDMAGDIPOLEENGE_H

#include "BDSFieldMag.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

class BDSMagnetStrength;

/**
 * @brief A dipole field with Enge-type fringes.
 * This follows the model and parameterisation described in:
 * https://journals.aps.org/prab/abstract/10.1103/PhysRevSTAB.18.064001
 * This class represents a dipole field with Enge fringes. To expand description.
 * The field is calculated in cartesian coordinates. A complete description to be added in the manual.
 * 
 * @author Paul Bogdan Jurj
 */

class BDSFieldMagDipoleEnge: public BDSFieldMag
{
public:
  BDSFieldMagDipoleEnge() = delete;

  /// This constructor uses the "field" and "length" parameters
  /// from the BDSMagnetStrength instance and forwards to the next constructor.
  ///BDSFieldMagDipoleEnge(BDSMagnetStrength const* strength,
  ///                      G4double apertureIn ,G4double engeIn = 1.0, G4double toleranceIn = 0.0);
  
  /// More reasonable constructor for the internal parameterisation. 'strength' represents B0
  BDSFieldMagDipoleEnge(G4double strength,
                        G4double apertureRadius,
                        G4double coilLength,
                        G4double engeCoefficient,
                        G4double toleranceIn = 0.0
                        );

  virtual ~BDSFieldMagDipoleEnge(){;}

  /// Calculate the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				                 const G4double       t = 0) const;

  /// @{ Accessor.
  inline G4double GetB0() const {return B0;}
  /// @}

private:
  /// Private default constructor to ensure use of supplied constructor
  
  G4ThreeVector QueryField(G4double y, G4double z) const;

  G4double D;
  G4double halfLength;
  G4double B0;
  G4double engeCoeff;
  G4double coilTolerance;
};

#endif
