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
#ifndef BDSINTEGRATORKICKERTHIN_H
#define BDSINTEGRATORKICKERTHIN_H

#include "BDSIntegratorMag.hh"
#include "BDSIntegratorDipoleFringe.hh"

#include "globals.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator for thin h or v kick.
 * 
 * This adds a fraction of the total momentum to the horizontal
 * or vertical (in curvilinear coordinates) as defined by hkick
 * and vkick respectively. The delta is subtracted from the z
 * component to conserve momentum.
 *
 * @author Laurie Nevay
 */

class BDSIntegratorKickerThin: public BDSIntegratorMag
{
public:
  BDSIntegratorKickerThin(BDSMagnetStrength const* strength,
			  G4double                 brhoIn,
			  G4Mag_EqRhs*             eqOfMIn,
			  G4double                 minimumRadiusOfCurvatureIn);
  
  virtual ~BDSIntegratorKickerThin(){;}
  /// @{ Assignment and copy constructor not implemented nor used
  BDSIntegratorKickerThin& operator=(const BDSIntegratorKickerThin&) = delete;
  BDSIntegratorKickerThin(BDSIntegratorKickerThin&) = delete;
  /// @}

  /// The stepper for integration.
  virtual void Stepper(const G4double yIn[],
		       const G4double dydx[],
		       const G4double h,
		       G4double       yOut[],
		       G4double       yErr[]);

  virtual void OneStep(const G4ThreeVector& localPos,
                       const G4ThreeVector& localMomUnit,
                       const G4ThreeVector& localMom,
                       const G4double&      h,
                       const G4double&      fcof,
                       G4ThreeVector&       localPosOut,
                       G4ThreeVector&       localMomOut) const;

  /// Separate fringe field integrators for entrance and exit fringes
  BDSIntegratorDipoleFringe* fringeIntEntr;
  BDSIntegratorDipoleFringe* fringeIntExit;

private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorKickerThin() = delete;

  /// @{ Cache of variable.
  const G4double hkick;
  const G4double vkick;
  const G4double brho;
  G4double rho;
  G4double tiltAngle;
  /// @}

  /// Cache if the fringe or pole face effects are to be applied
  G4bool hasEntranceFringe;
  G4bool hasExitFringe;
};

#endif
