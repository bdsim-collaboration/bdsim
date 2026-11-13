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
#ifndef BDSINTEGRATORDIPOLEFRINGEBASE_H
#define BDSINTEGRATORDIPOLEFRINGEBASE_H

#include "BDSAuxiliaryNavigator.hh"
#include "BDSIntegratorDipoleRodrigues2.hh"
#include "BDSIntegratorDipoleQuadrupole.hh"
#include "BDSIntegratorMultipoleThin.hh"

#include "globals.hh"

namespace BDS
{
  /// Function to calculate the value of the fringe field correction term.
  G4double FringeFieldCorrection(BDSMagnetStrength const* strength, G4bool entranceOrExit);

  /// Function to calculate the value of the second fringe field correction term.
  G4double SecondFringeFieldCorrection(BDSMagnetStrength const* strength, G4bool entranceOrExit);
}

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator that ignores the field and uses the analytical solution for a dipole kick.
 *
 * @author Will Shields 
 */

class BDSIntegratorDipoleFringe: public BDSIntegratorDipoleRodrigues2, public BDSAuxiliaryNavigator
{
public:
  BDSIntegratorDipoleFringe(BDSMagnetStrength const* strength,
			    G4double                 brhoIn,
			    G4Mag_EqRhs*             eqOfMIn,
			    G4double                 minimumRadiusOfCurvature,
			    const G4double&          tiltIn = 0);
  
  virtual ~BDSIntegratorDipoleFringe();

  /// @{ Assignment and copy constructor not implemented nor used
  BDSIntegratorDipoleFringe& operator=(const BDSIntegratorDipoleFringe&) = delete;
  BDSIntegratorDipoleFringe(BDSIntegratorDipoleFringe&) = delete;
  /// @}

  /// The stepper for integration. Calls base class stepper.
  virtual void Stepper(const G4double yIn[6],
		       const G4double dydx[6],
		       const G4double h,
		       G4double       yOut[6],
		       G4double       yErr[6]);

  /// The stepper for integration. Particle charge and momentum scaling are provided as arguments
  /// as they are calculated in the derived classes. Uses BDSIntegratorDipole2::Stepper and then adds
  /// a kick in yp in curvilinear coordinates.
  void BaseStepper(const G4double  yIn[6],
                   const G4double  dydx[6],
                   const G4double& h,
                   G4double        yOut[6],
                   G4double        yErr[6],
                   const G4double& fcof,
                   const G4double& momScaling);


  /// Calculate a single step using dipole fringe field matrix.
  /// Unit momentum, momentum magnitude, and normalised bending radius are provided
  /// as arguments because they already calculated in the BaseStepper method.
  void OneStep(const G4ThreeVector& posIn,
               const G4ThreeVector& momUIn, // assumed unit momentum of momIn
               G4ThreeVector&       posOut,
               G4ThreeVector&       momOut,
               const G4double&      bendingRadius) const;

  /// Calculate the thin multipole kick to represent the dipole poleface curvature effect.
  /// Step length is passed in as it is needed by the transforms.
  void MultipoleStep(const G4double  yIn[6],
                     G4double        yMultipoleOut[6],
                     const G4double& h);

  /// Getter functions for poleface and fringe variables. Values need to be read in at least the
  /// thin kicker integrator, but should not be overwritten.
  inline G4double GetPolefaceAngle() {return polefaceAngle;}
  inline G4double GetFringeCorr() {return fringeCorr;}
  inline G4double GetSecondFringeCorr() {return secondFringeCorr;}
  inline G4double GetPolefaceCurv() {return polefaceCurvature;}

protected:
  /// Poleface rotation angle
  G4double polefaceAngle;
  /// Fringe field correction term
  G4double fringeCorr;
  /// Second fringe field correction term
  G4double secondFringeCorr;
  /// Poleface curvature
  G4double polefaceCurvature;
  /// Nominal magnet bending radius
  G4double rho;

  G4ThreeVector unitField;       ///< Cache of the unit field direction.
  const G4double fieldArcLength; ///< Cache of the field arc length.
  const G4double fieldAngle;     ///< Cache of the field angle.

  const G4double tilt;
  const G4bool   finiteTilt;
  G4double bx;
  G4double by;
  /// Whether a magnet has a finite strength or not. Can be set in the constructor for
  /// zero strength elements and then a drift routine is used before anything else.
  G4bool zeroStrength;
  
  /// Cache of thin element length from global constants. Initialised via check
  /// on unphysical -1 value as global constants doesn't exist at compile time.
  static G4double thinElementLength;

  BDSIntegratorMultipoleThin* multipoleIntegrator;

  G4bool isEntrance; ///< store if fringe is entrance or exit

  /// Fractional momentum limit above which the thin matrix fringe kick accuracy becomes questionable, so
  /// advance with the dipolerodrigues2 stepper to ensure correct dipole tracking length is applied
  G4double backupStepperMomLimit;

private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorDipoleFringe() = delete;
};

#endif
