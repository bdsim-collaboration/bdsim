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
#ifndef BDSFIELDMAGSOLENOIDSHEET_H
#define BDSFIELDMAGSOLENOIDSHEET_H

#include "BDSFieldMag.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic field due to a cylinder of current.
 *
 * This follows the parameterisation and uses the algorithm for the generalised complete
 * elliptical integral as described in:
 *
 * Cylindrical Magnets and ideal Solenoids, N. Derby and S. Olbert, American Journal of
 * Physics **78**, 229 (2010); https://doi.org/10.1119/1.3256157 and also at
 * https://arxiv.org/abs/0909.3880.
 *
 * The field is calculated in cylindrical coordinates. A complete description is in the manual.
 * 
 * @author Laurie Nevay
 */

class BDSFieldMagSolenoidSheet: public BDSFieldMag
{
public:
  BDSFieldMagSolenoidSheet() = delete;
  /// This constructor uses the "field" and "length" parameters
  /// from the BDSMagnetStrength instance and forwards to the next constructor.
  BDSFieldMagSolenoidSheet(BDSMagnetStrength const* strength,
                           G4double radiusIn ,G4double toleranceIn = 0.0);
  /// More reasonable constructor for the internal parameterisation. 'strength'
  /// can be either B0 or I. This is interpreted via 'strengthIsCurrent'. Have
  /// to do this as the signature would be the same for either case.
  BDSFieldMagSolenoidSheet(G4double strength,
                           G4bool   strengthIsCurrent,
                           G4double sheetRadius,
                           G4double fullLength,
                           G4double toleranceIn = 0.0);
  virtual ~BDSFieldMagSolenoidSheet(){;}

  /// Calculate the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
                                 const G4double       t = 0) const;
  
  /// @{ Accessor.
  inline G4double GetB0() const {return B0;}
  inline G4double GetI()  const {return I;}
  /// @}

private:
  /// Approximation for rho=0 Bz field. Brho=0 by definition. zp and zm are z+halfLength
  /// and z-halfLength. Returns Bz.
  G4double OnAxisBz(G4double zp, G4double zm) const;
  
  G4double a;
  G4double halfLength;
  G4double B0;
  G4double I;
  G4double spatialLimit;
  G4double normalisation;
  G4double coilTolerance;
};

#endif
