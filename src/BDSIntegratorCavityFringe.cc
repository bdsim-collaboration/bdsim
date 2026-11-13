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

#include "BDSDebug.hh"
#include "BDSIntegratorCavityFringe.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4Mag_EqRhs.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

BDSIntegratorCavityFringe::BDSIntegratorCavityFringe(BDSMagnetStrength const* strength,
                                                   G4Mag_EqRhs* eqOfMIn,
                                                   G4double brho,
                                                   G4double maximumRadiusIn):
  BDSIntegratorRMatrixThin(strength, eqOfMIn, maximumRadiusIn),
  phase((*strength)["phase"]),
  efield((*strength)["efield"]),
  isentrance((*strength)["isentrance"]),
  eq(static_cast<BDSMagUsualEqRhs*>(eqOfM))
{
  G4int acceleratingFieldDirectionFactor = BDS::Sign(brho);
  efield /= acceleratingFieldDirectionFactor;
  rmat11  = (*strength)["rmat11"];
  rmat21  = (*strength)["rmat21"];
  rmat22  = (*strength)["rmat22"];
  rmat33  = (*strength)["rmat33"];
  rmat43  = (*strength)["rmat43"];
  rmat44  = (*strength)["rmat44"];
}

void BDSIntegratorCavityFringe::Stepper(const G4double yIn[],
                                       const G4double /*dydx*/[],
                                       const G4double h,
                                       G4double       yOut[],
                                       G4double       yErr[])
{
  // get particle charge from reverse of how it's calculated in G4Mag_EqRhs::SetChargeMomentumMass.
  G4double charge = eq->FCof()/(eplus*c_light);
  G4double gammaPrime = charge*efield*std::cos(phase) * CLHEP::m;

  // don't update matrix for chargeless particles - remains identity matrix
  if (BDS::IsFinite(charge))
    {
	  G4double sign = 1;
	  if (!(G4bool)isentrance)
	    {sign = -1;}

	  // set energy dependent matrix terms
	  rmat21 = sign * gammaPrime / (2 * eq->TotalEnergy(yIn));
	  rmat43 = sign * gammaPrime / (2 * eq->TotalEnergy(yIn));
    }

  // apply kick in thin rmatrix base class
  BDSIntegratorRMatrixThin::Stepper(yIn,0,h,yOut,yErr);

  // set matrix terms back to zero.
  rmat21 = 0;
  rmat43 = 0;
}
