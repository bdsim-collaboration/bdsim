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
#ifndef BDSFIELDMAGDIPOLEOUTER_H
#define BDSFIELDMAGDIPOLEOUTER_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"

#include <vector>

class BDSMagnetStrength;

/**
 * @brief A perfect magetic dipole in 3D, normal field inside 1/2 poleTipRadius.
 *
 * Field for the far field of a pure magnetic dipole. Inside 0.5*poleTipRadius,
 * the nominal uniform field is returned and tanh is used as a sigmoid function
 * to smoothly vary between the two on a length scale of transitionLengthScale, 
 * which is 1cm by default.
 *
 * @author Laurie Nevay
 */

class BDSFieldMagDipoleOuter: public BDSFieldMag
{
public:
  BDSFieldMagDipoleOuter(const BDSMagnetStrength* strength,
			 const G4double&          poleTipRadius,
			 G4double                 arbitraryScaling = 1.0);

  virtual ~BDSFieldMagDipoleOuter(){;}

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const double         t = 0) const;

private:
  G4double      spatialLimit;   ///< Limit for getting too close to a current source.
  G4double      poleTipRadius;  ///< Used as radial limit for returning normal field.
  G4double      normalisation;  ///< Storage of the overal normalisation factor.
  G4ThreeVector localField;     ///< Nominal dipole field.
  G4ThreeVector m;              ///< Dipole moment as unit vector of field direction.
  G4double      maxField;       ///< Any field beyond this will curtailed to this value.
  G4bool        initialisationPhase; ///< Need a way to control cludge normalisation behaviour during initial normalisation calculation.
  static G4double transitionLengthScale;
};

#endif
