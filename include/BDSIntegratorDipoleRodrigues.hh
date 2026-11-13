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
#ifndef BDSINTEGRATORDIPOLERODRIGUES_H
#define BDSINTEGRATORDIPOLERODRIGUES_H

#include "BDSIntegratorMag.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include <utility>

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Stepper that calculates trajectory through uniform magnetic field.
 *
 * This calculates the coordinates of a particle moving through a dipole field.
 * This ignores the field value supplied from Geant4 and uses its own field and
 * quadrupole gradient from the BDSMagnetStrength instance supplied w.r.t. the 
 * rigidity also supplied.
 * 
 * Originally part of BDSIM by many authors.
 */

class BDSIntegratorDipoleRodrigues: public BDSIntegratorMag
{
public:
  BDSIntegratorDipoleRodrigues(BDSMagnetStrength const* strength,
			       G4double                 brho,
			       G4Mag_EqRhs*             eqOfMIn);
    
  virtual ~BDSIntegratorDipoleRodrigues(){;}

  /// Stepper for this integrator.  Calculates the new coordinates of a particle
  /// through a uniform magnetic field.
  virtual void Stepper(const G4double yIn[],
		       const G4double dydx[],
		       const G4double h,
		       G4double       yOut[],
		       G4double       yErr[]);
  
protected:
  /// Calculate the new particle coordinates.
  void AdvanceHelix(const G4double yIn[],
		    const G4double dydx[],
		    G4double       h,
		    G4double       yOut[],
		    G4double       yErr[]);

  /// Scaling factor in brho calculation.
  G4double cOverGeV;

  /// Angle that the dipole induces in the reference trajectory.
  const G4double angle;

  /// Arc length of the magnetic field.
  const G4double length;

  /// Uniform magnetic field in global Y direction.
  G4double bField;

  /// Magnet strength object needed by curvilinear transforms.
  BDSMagnetStrength const* strength;

  std::pair<G4ThreeVector,G4ThreeVector> UpdatePandR(G4double rho,
						     G4double h,
						     G4ThreeVector localPos,
						     G4ThreeVector localMomUnit);
  
private:
  /// Data stored in order to find the chord.
  G4ThreeVector yInitial, yMidPoint, yFinal;

  /// Minimum tolerable radius of curvature - used to prevent spiraling particles.
  const G4double minimumRadiusOfCurvature;
};

#endif 
