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
#ifndef BDSINTEGRATORMULTIPOLETHIN_H
#define BDSINTEGRATORMULTIPOLETHIN_H

#include "BDSIntegratorMag.hh"

#include "globals.hh"
#include <list>
#include <vector>

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator that ignores the field and uses the analytical solution to a multipole.
 * 
 * @author Will Shields
 */

class BDSIntegratorMultipoleThin: public BDSIntegratorMag
{
public:
  BDSIntegratorMultipoleThin(BDSMagnetStrength const* strength,
			     G4double                 brhoIn,
			     G4Mag_EqRhs*             eqOfMIn);
  
  virtual ~BDSIntegratorMultipoleThin(){;}

  /// The stepper for integration. The stepsize is fixed, equal to h. The reason for this
  /// is so that intermediate steps can be calculated and therefore the error ascertained
  /// or distance from the chord.  Error calculation is not currently implemented.
  virtual void Stepper(const G4double yIn[],
		       const G4double dydx[],
		       const G4double h,
		       G4double       yOut[],
		       G4double       yErr[]);

  /// Calculate a single step using thin multipole matrix.
  /// Unit momentum, momentum magnitude, and normalised bending radius are provided
  /// as arguments because they already calculated in the Stepper method.
  void OneStep(const G4ThreeVector& posIn,
               const G4ThreeVector& momUIn, // assumed unit momentum of momIn
               const G4double       momIn,
               G4ThreeVector&       posOut,
               G4ThreeVector&       momOut,
               G4double             h) const;

private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorMultipoleThin() = delete;

  /// Calculate the factorial of n.
  G4int Factorial(G4int n);

  /// Magnetic rigidity for momentum scaling
  G4double brho;

  /// Dipole component
  G4double b0l;
  /// @{ Higher order components
  std::list<double>  bnl;
  std::list<double>  bsl;
  std::vector<G4int> nfact;
  /// @}
};

#endif
