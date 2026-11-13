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
#ifndef BDSINTEGRATORDIPOLEQUADRUPOLE_H
#define BDSINTEGRATORDIPOLEQUADRUPOLE_H

#include "BDSIntegratorMag.hh"
#include "BDSMagUsualEqRhs.hh"

#include "globals.hh"

class BDSIntegratorDipoleRodrigues2;
class BDSMagnetStrength;
class BDSParticleDefinition;
class BDSStep;
class G4Mag_EqRhs;

/**
 * @brief Integrator for combined dipole and quadrupolar field.
 * 
 * @author Laurie Nevay
 */

class BDSIntegratorDipoleQuadrupole: public BDSIntegratorMag
{
public:
  BDSIntegratorDipoleQuadrupole(BDSMagnetStrength const* strength,
				G4double                 brhoIn,
				G4Mag_EqRhs*             eqOfMIn,
				G4double minimumRadiusOfCurvatureIn,
				const BDSParticleDefinition* designParticle,
				const G4double&     tiltIn);
  
  virtual ~BDSIntegratorDipoleQuadrupole();

  /// @{ Assignment and copy constructor not implemented nor used
  BDSIntegratorDipoleQuadrupole& operator=(const BDSIntegratorDipoleQuadrupole&) = delete;
  BDSIntegratorDipoleQuadrupole(BDSIntegratorDipoleQuadrupole&) = delete;
  /// @}

  /// Check if the quadrupole has finite strength and use drift if not. If finite strength,
  /// convert to local curvilinear coordiantes and check for paraxial approximation. If paraxial,
  /// use thick quadrupole matrix for transport, else use the G4ClassicalRK4 backup stepper.
  virtual void Stepper(const G4double y[6],
		       const G4double dydx[6],
		       const G4double h,
		       G4double       yOut[6],
		       G4double       yErr[6]);

protected:

  /// Calculate a single step in curvilinear coordinates using dipole quadrupole matrix.
  /// Unit momentum is provided as an argument becuase it is already calculated in the
  /// Stepper method.
  void OneStep(const G4ThreeVector& posIn,
	       const G4ThreeVector& momIn,
	       const G4ThreeVector& momUIn, // assumed unit momentum of momIn
	       const G4double&      h,
	       const G4double&      fcof,
	       G4ThreeVector&       posOut,
	       G4ThreeVector&       momOut,
	       const G4double	    rho,
	       const G4double	    beta,
	       const G4double       deltaEnergy) const;



private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorDipoleQuadrupole() = delete;
  
  const G4double    nominalBRho;  ///< Cached magnet property, nominal magnetic rigidity
  BDSMagUsualEqRhs* eq;    ///< BDSIM's eqRhs class to give access to particle properties
  const G4double    bPrime;///< Cached magnet property, B field gradient for calculating K1
  const G4double    nominalBeta;    ///< Cached nominal relativistic beta of the nominal beam particle.
  G4double          nominalRho;     ///< Cached magnet property, nominal bending radius.
  const G4double    nominalField;   ///< Cached magnet property, nominal field strength.
  G4double          fieldRatio;     ///< Ratio of supplied field to nominal field. Needed for over/underpowered magnets.
  const G4double    nominalEnergy;  ///< Nominal beam energy
  const	G4double    nominalMass;    ///< Primary particle mass. Needed for recalculating nominal energy with scaling.
  const G4double    nominalCharge;  ///< Nominal beam charge.
  G4ThreeVector     unitField;      ///< Cache of the unit field direction.
  const G4double    fieldArcLength; ///< Cache of the field arc length.
  const G4double    nominalAngle;   ///< Cache of the field angle.
  G4double          angleForCL;     ///< Angle used for curvilinear transforms.
  G4double          tilt;           ///< Tilt offset transform for field.
  const G4double    scaling;        ///< Cache field scaling factor
  G4bool            isScaled;       ///< Cache if field is scaled

  BDSIntegratorDipoleRodrigues2* dipole;///< Backup integrator
};

#endif
