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
#include "BDSIntegratorSolenoid.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals.hh
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

#include <cmath>
#include <limits>

BDSIntegratorSolenoid::BDSIntegratorSolenoid(BDSMagnetStrength const* strength,
					     G4double                 brho,
					     G4Mag_EqRhs*             eqOfMIn):
  BDSIntegratorMag(eqOfMIn, 6)
{
  bField = brho * (*strength)["ks"];
  zeroStrength = !BDS::IsFinite(bField);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B (local) = " << bField << G4endl;
#endif
}

void BDSIntegratorSolenoid::Stepper(const G4double yIn[],
				    const G4double dydx[],
				    G4double       h,
				    G4double       yOut[],
				    G4double       yErr[])
{
  // in case of zero field or neutral particles do a linear step
  const G4double fcof = eqOfM->FCof();
  if (zeroStrength || !BDS::IsFiniteStrength(fcof) || h < 1e-9)
    {
      AdvanceDriftMag(yIn,h,yOut,yErr);
      SetDistChord(0);
      return;
    }

  G4ThreeVector mom     = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  G4double      momMag  = mom.mag();

  // kappa unit is m^-1, so scale to mm.
  G4double      kappa   = 0.5*fcof*bField/momMag / CLHEP::m;

  // neutral particle or no strength - advance as a drift.
  if (std::abs(kappa)<1e-20)
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      SetDistChord(0);
      return;
    }

  G4ThreeVector pos      = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector momUnit  = mom.unit();
  G4double      h2       = h*h;
  BDSStep    localPosMom = GlobalToCurvilinear(pos, momUnit, h, true);
  G4ThreeVector localPos = localPosMom.PreStepPoint();
  G4ThreeVector localMom = localPosMom.PostStepPoint();
  G4ThreeVector localMomUnit = localMom.unit();
  
  // finite strength - treat as a solenoid
  G4double x0  = localPos.x();
  G4double y0  = localPos.y();
  G4double z0  = localPos.z();
  G4double xp0 = localMomUnit.x();
  G4double yp0 = localMomUnit.y();
  G4double zp0 = localMomUnit.z();

  // only proceed with thick matrix if particle is paraxial
  // judged by forward momentum > 1-limit and |transverse| < limit (default limit=0.1)
  if (zp0 < (1.0-backupStepperMomLimit) || std::abs(xp0) > backupStepperMomLimit || std::abs(yp0) > backupStepperMomLimit)
    {
      backupStepper->Stepper(yIn, dydx, h, yOut, yErr);
      SetDistChord(backupStepper->DistChord());
      return;
    }
  
  // local r'' (for curvature)
  G4ThreeVector localA;
  localA.setX(-zp0*x0);
  localA.setY( zp0*y0);
  localA.setZ( x0*xp0 - y0*yp0);
  localA *= kappa;
  
  // determine effective curvature 
  G4double localAMag = localA.mag();

  // low strength - drift
  if (localAMag < 1e-15)
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      SetDistChord(0);
      return;
    }
  
  G4double radiusOfCurvature = std::numeric_limits<double>::max();
  if (BDS::IsFiniteStrength(localAMag))
    {radiusOfCurvature = 1./localAMag;} // avoid division by 0
  
  // chord distance (simple quadratic approx)
  G4double dc = h2/(8*radiusOfCurvature);
  if (std::isnan(dc))
    {SetDistChord(0);}
  else
    {SetDistChord(dc);}

  // From USPAS lecture (http://uspas.fnal.gov/materials/13Duke/SCL_Chap3.pdf)
  // (1 , S / 2K      , 0 , (1-C) / 2K  ) (x )
  // (0 , C           , 0 , S           ) (x')
  // (0 , -(1-C) / 2K , 1 , S / 2K      ) (y )
  // (0 , -S          , 0 , C           ) (y')
  //
  // K = B_0 / 2*Brho
  // B_0 is field in solenoid
  // Brho is momentum of central trajectory
  // C = cos( 2KL )
  // S = sin( 2KL )

  G4double C      = std::cos(2 * kappa * h);
  G4double S      = std::sin(2 * kappa * h);
  G4double S2oK   = S / kappa;
  G4double OmC2oK = (1.0 - C) / kappa;

  G4double X11 = 1, X12 = 0.5*S2oK,    X13 = 0, X14 = 0.5*OmC2oK;
  G4double X21 = 0, X22 = C,           X23 = 0, X24 = S;
  G4double X31 = 0, X32 = -0.5*OmC2oK, X33 = 1, X34 = 0.5*S2oK;
  G4double X41 = 0, X42 = -S,          X43 = 0, X44 = C ;

  G4double x1  = x0*X11 + xp0*X12 + y0*X13 + yp0*X14;
  G4double xp1 = x0*X21 + xp0*X22 + y0*X23 + yp0*X24;
  G4double y1  = x0*X31 + xp0*X32 + y0*X33 + yp0*X34;
  G4double yp1 = x0*X41 + xp0*X42 + y0*X43 + yp0*X44;
  
  G4double z1 = z0 + h;
  // ensure normalisation for vector
  G4double zp1 = std::sqrt(1 - xp1*xp1 - yp1*yp1);
  if (std::isnan(zp1))
    {zp1 = zp0;} 
  
  localPos.setX(x1);
  localPos.setY(y1);
  localPos.setZ(z1);
  localMomUnit.setX(xp1);
  localMomUnit.setY(yp1);
  localMomUnit.setZ(zp1);

  // normalise from unit momentum to absolute momentum
  G4ThreeVector localMomOut = localMomUnit * momMag;

  // convert to global coordinates
  BDSStep globalPosMom = CurvilinearToGlobal(localPos, localMomOut, true);
  G4ThreeVector globalPosOut = globalPosMom.PreStepPoint();
  G4ThreeVector globalMomOut = globalPosMom.PostStepPoint();
  
  // error along direction of travel really
  G4ThreeVector globalMomOutU = globalMomOut.unit();
  globalMomOutU *= 1e-8;
  
  // write out coordinates and errors to arrays
  for (G4int i = 0; i < 3; i++)
    {
      yOut[i]   = globalPosOut[i];
      yOut[i+3] = globalMomOut[i];
      yErr[i]   = globalMomOutU[i]*1e-10; // empirically this has to be very small
      yErr[i+3] = 1e-40;
    }
}
