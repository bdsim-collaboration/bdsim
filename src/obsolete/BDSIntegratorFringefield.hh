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
#ifndef BDSINTEGRATORFRINGEFIELD_H
#define BDSINTEGRATORFRINGEFIELD_H

#include "BDSIntegratorDipoleRodrigues.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator that ignores the field and uses the analytical solution for a dipole kick.
 *
 * @author Will Shields 
 */

class BDSIntegratorFringefield: public BDSIntegratorDipoleRodrigues
{
public:
  BDSIntegratorFringefield(BDSMagnetStrength const* strength,
			   G4double                 brho,
			   G4Mag_EqRhs*             eqOfMIn);
  
  virtual ~BDSIntegratorFringefield(){;}

  /// The stepper for integration. The stepsize is fixed, equal to h. The reason for this
  /// is so that intermediate steps can be calculated and therefore the error ascertained
  /// or distance from the chord.  Error calculation is not currently implemented.
  virtual void Stepper(const G4double y[],
		       const G4double dydx[],
		       const G4double h,
		       G4double       yOut[],
		       G4double       yErr[]);

protected:
  /// Calculate the new particle coordinates for a given step length h.
  virtual void AdvanceHelix(const G4double yIn[],
		    const G4double dydx[],
		    const G4double h,
		    G4double       yOut[],
		    G4double       yErr[]);
private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorFringefield();
  
  /// Poleface rotation angle
  G4double polefaceAngle;
  /// Fringe field correction term
  G4double fringeCorr;
  /// Data stored in order to find the chord.
  G4ThreeVector yInitial, yMidPoint, yFinal;
};

#endif
