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
#include "BDSIntegratorDipoleRodrigues2.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4ClassicalRK4.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSIntegratorDipoleRodrigues2::BDSIntegratorDipoleRodrigues2(G4Mag_EqRhs* eqOfMIn,
							     G4double     minimumRadiusOfCurvatureIn):
  G4MagHelicalStepper(eqOfMIn),
  eqOfM(eqOfMIn),
  minimumRadiusOfCurvature(minimumRadiusOfCurvatureIn)
{;}

void BDSIntegratorDipoleRodrigues2::DumbStepper(const G4double yIn[6],
						G4ThreeVector  field,
						G4double       stepLength,
						G4double       yOut[6])
{
  AdvanceHelix(yIn, field, stepLength, yOut);
}

void BDSIntegratorDipoleRodrigues2::SingleStep(const G4double  yIn[6],
					       const G4double& h,
					       G4double        yOut[6])
{
  G4double bO[6]; // original location field value - must be 6 long
  eqOfM->GetFieldValue(yIn, bO);
  G4ThreeVector bOriginal = G4ThreeVector(bO[0],bO[1],bO[2]);

  // Do a full step using G4MagHelicalStepper
  AdvanceHelix(yIn, bOriginal, h, yOut);
}

void BDSIntegratorDipoleRodrigues2::Stepper(const G4double   yIn[6],
					    const G4double[] /*dydx*/,
					    G4double         h,
					    G4double         yOut[6],
					    G4double         yErr[6])
{
  // overwrite any previous rad and helix to avoid contaminating this step
  FudgeDistChordToZero();
  
  // Protect against very small steps or neutral particles drift through.
  if (h < 1e-12 || !BDS::IsFiniteStrength(eqOfM->FCof()))
    {
      AdvanceDriftMag(yIn,h,yOut,yErr);
      FudgeDistChordToZero(); // see doxygen in header for explanation
      return;
    }
  
  // Arrays for field querying (g4 interface)
  G4double bO[6]; // original location field value - must be 6 long
  eqOfM->GetFieldValue(yIn, bO);
  G4ThreeVector bOriginal = G4ThreeVector(bO[0],bO[1],bO[2]);

  // protect against zero field as G4's Advance helix gives wrong ang and rad
  if (!BDS::IsFiniteStrength(bOriginal.mag()))
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      FudgeDistChordToZero();
      return;
    }

  // Do a full step - the result we use
  AdvanceHelix(yIn, bOriginal, h, yOut);
  // cache some calculated parameters as there's no way to cache the distchord
  // directly as the G4MagHelicalStepper class doesn't make DistChord() virtual.
  G4double ang = GetAngCurve();
  G4double rad = GetRadHelix();
  // the G4MagHelicalStepper can sometimes produce nans for rad helix! These
  // go into distchord and then ruin the tracking across events.
  if (std::isnan(rad))
    {
      rad = h / ang;
      SetRadHelix(rad);
    }
  
  // Now we have the radius of curvature to check on.
  // If it's smaller than limit, we artificially advance the particle
  // along its helix axis (parallel to the field) so it'll hit something
  // and finish in a timely manner.
  const G4double radiusOfCurvature = rad;
  if (std::abs(radiusOfCurvature) < minimumRadiusOfCurvature)
    {
      AdvanceHelixForSpiralling(yIn, bOriginal, h, yOut, yErr);
      // Update parameters that distchord will be calcualted from from full step info.
      SetAngCurve(ang);
      SetRadHelix(rad);
      return;
    }

  // error estimation
  // if it's a very small step - no error - safer as differences between large positions
  // for very small steps can have numerical precision issues.
  if (h < 1e-9) // 1e-9 is 1pm (in g4 units) - the minimum length scale of bdsim
    {
      // set error to be low - required
      for (G4int i = 0; i < 3; i++)
	{
	  yErr[i]   = 1e-20;
	  yErr[i+3] = 1e-40;
	}
      SetAngCurve(ang);
      SetRadHelix(rad); // must update these for distchord
      return;
    }

  // normal error estimation - do two half steps and compare difference to
  // the result from one full step
  G4double yTemp[7];
  AdvanceHelix(yIn, bOriginal, h*0.5, yTemp); // first step

  // resample field at midway point (although if pure dipole, this is
  // unnecessary) - could go outside the range of the field though
  G4double bM[6]; // mid point location field value - must be 6 long
  GetEquationOfMotion()->GetFieldValue(yTemp, bM);
  G4ThreeVector bMid = G4ThreeVector(bM[0],bM[1],bM[2]);

  G4double yTemp2[7];
  AdvanceHelix(yTemp, bMid, h*0.5, yTemp2); // second step
  
  // Error estimation
  for (G4int i = 0; i < 6; i++)
    {yErr[i] = std::abs(yOut[i] - yTemp2[i]);}

  // Update parameters that distchord will be calcualted from from full step info.
  SetAngCurve(ang);
  SetRadHelix(rad);
}

void BDSIntegratorDipoleRodrigues2::AdvanceHelixForSpiralling(const G4double        yIn[6],
							      const G4ThreeVector&  field,
							      const G4double&       h,
							      G4double              yOut[6],
							      G4double              yErr[6])
{
  G4ThreeVector fieldUnit = field.unit();

  // protect against zero field as this algorithm would be wrong
  if (!BDS::IsFinite(fieldUnit.mag()))
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      FudgeDistChordToZero();
      return;
    }

  G4double momMag = G4ThreeVector(yIn[3], yIn[4], yIn[5]).mag();
  
  // Artificially change momentum to be along field direction.
  G4ThreeVector    momNew = fieldUnit*momMag;

  // Create updated array for tracking.
  G4double yModified[7];
  for (G4int i = 0; i < 3; i++)
    {
      yModified[i]     = yIn[i];    // position stays the same
      yModified[i + 3] = momNew[i]; // update momentum
    }

  // Simply advance as a drift along this vector.
  AdvanceDriftMag(yModified, h, yOut, yErr);

  // set error to be low - required
  for (G4int i = 0; i < 3; i++)
    {
      yErr[i]   = 1e-20;
      yErr[i+3] = 1e-40;
    }
}

void BDSIntegratorDipoleRodrigues2::FudgeDistChordToZero()
{
  // Large angle condition in G4MagHelicalStepper DistChord() method is faster, so set angle to 10.
  SetAngCurve(10);
  SetRadHelix(0);
}
