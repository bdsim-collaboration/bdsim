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
#include "BDSIntegratorDipoleFringeScaling.hh"

#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

BDSIntegratorDipoleFringeScaling::BDSIntegratorDipoleFringeScaling(BDSMagnetStrength const* strengthIn,
								   G4double                 brhoIn,
								   G4Mag_EqRhs*             eqOfMIn,
								   G4double                 minimumRadiusOfCurvatureIn,
								   const G4double&          tiltIn):
  BDSIntegratorDipoleFringe(strengthIn, brhoIn, eqOfMIn, minimumRadiusOfCurvatureIn, tiltIn),
  bRho(brhoIn*(*strengthIn)["scaling"])  // scale here otherwise calculation of rho in base class will be unscaled
{;}

void BDSIntegratorDipoleFringeScaling::Stepper(const G4double yIn[],
                                        const G4double dydx[],
                                        const G4double h,
                                        G4double       yOut[],
                                        G4double       yErr[])
{
  // momentum magnitude for scaling, doesn't matter if global or local as only the magnitude is needed
  G4ThreeVector mom = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  G4double momInMag = mom.mag();

  // unit normalisation
  const G4double fcof = eqOfM->FCof();

  // normalise to momentum only, charge normalisation in base stepper
  G4double momScaling = std::abs(momInMag / (fcof * bRho));

  BaseStepper(yIn, dydx, h, yOut, yErr, fcof, momScaling);
}

