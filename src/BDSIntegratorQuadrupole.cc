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
#include "BDSIntegratorQuadrupole.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "G4Mag_EqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <limits>

BDSIntegratorQuadrupole::BDSIntegratorQuadrupole(BDSMagnetStrength const* strength,
						 G4double                 brho,
						 G4Mag_EqRhs*             eqOfMIn,
						 G4double minimumRadiusOfCurvatureIn):
  BDSIntegratorMag(eqOfMIn, 6),
  minimumRadiusOfCurvature(minimumRadiusOfCurvatureIn)
{
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  // we take |Brho| as it depends on charge and so does the eqOfM->FCof()
  // so they'd both cancel out.
  bPrime = std::abs(brho) * (*strength)["k1"] / CLHEP::m2;

  zeroStrength = !BDS::IsFiniteStrength(bPrime);
}

void BDSIntegratorQuadrupole::Stepper(const G4double yIn[],
				      const G4double dydx[],
				      const G4double h,
				      G4double       yOut[],
				      G4double       yErr[])
{
  // in case of zero field or neutral particles do a linear step
  const G4double fcof = eqOfM->FCof();
  if (zeroStrength || !BDS::IsFiniteStrength(fcof))
    {
      AdvanceDriftMag(yIn,h,yOut,yErr);
      SetDistChord(0);
      return;
    }
  
  G4ThreeVector mom    = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  G4double      momMag = mom.mag();

  // quad strength k normalised to charge and momentum of this particle
  // note bPrime was calculated w.r.t. the nominal rigidity.
  // eqOfM->FCof() gives us conversion to MeV,mm and rigidity in Tm correctly
  // as well as charge of the given particle
  G4double kappa = fcof*bPrime/momMag;
  
  // neutral particle or no strength - advance as a drift.
  if(std::abs(kappa) < 1e-20)
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      SetDistChord(0);
      return;
    }

  G4ThreeVector pos          = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector momUnit      = mom.unit();
  BDSStep       localPosMom  = GlobalToCurvilinear(pos, momUnit, h, true);
  G4ThreeVector localPos     = localPosMom.PreStepPoint();
  G4ThreeVector localMomUnit = localPosMom.PostStepPoint();

  G4double xp  = localMomUnit.x();
  G4double yp  = localMomUnit.y();
  G4double zp  = localMomUnit.z();

  // only proceed with thick matrix if particle is paraxial
  // judged by forward momentum > 1-limit and |transverse| < limit (default limit=0.1)
  if (zp < (1.0-backupStepperMomLimit) || std::abs(xp) > backupStepperMomLimit || std::abs(yp) > backupStepperMomLimit)
    {
      backupStepper->Stepper(yIn, dydx, h, yOut, yErr);
      SetDistChord(backupStepper->DistChord());
      return;
    }
  
  G4double x0  = localPos.x();
  G4double y0  = localPos.y();
  G4double z0  = localPos.z();

  // local r'' (for curvature)
  G4ThreeVector localA;
  localA.setX(-zp*x0); // can this be replaced by a cross product?
  localA.setY( zp*y0); // G4ThreeVector has a cross method
  localA.setZ( x0*xp - y0*yp);
  localA *= kappa;
  // determine effective curvature 
  G4double localAMag         = localA.mag();
  G4double radiusOfCurvature = std::numeric_limits<double>::max();
  if (BDS::IsFiniteStrength(localAMag))
    {radiusOfCurvature = 1./localAMag;} // avoid division by 0

  // if we have a low energy particle that makes it into the paraxial cuts
  // it could cause problems later in paraxial algorithm so use backup integrator
  if (std::abs(radiusOfCurvature) < minimumRadiusOfCurvature)
    {
      backupStepper->Stepper(yIn, dydx, h, yOut, yErr);
      SetDistChord(backupStepper->DistChord());
      return;
    }

  G4double h2  = h*h;
  // initialise output variables with input position as default
  G4double x1  = x0;
  G4double y1  = y0;
  // z1 calculated below
  G4double xp1 = xp;
  G4double yp1 = yp;
  G4double zp1 = zp;
      
  // chord distance (simple quadratic approx)
  G4double dc = h2/(8*radiusOfCurvature);
  if (std::isnan(dc))
    {SetDistChord(0);}
  else
    {SetDistChord(dc);}
  
  G4double rootK  = std::sqrt(std::abs(kappa*zp)); // direction independent
  if (std::isnan(rootK))
    {rootK = 0;}
  G4double rootKh = rootK*h*zp;
  G4double X11=0,X12=0,X21=0,X22=0;
  G4double Y11=0,Y12=0,Y21=0,Y22=0;
  
  if (kappa >= 0)
    {//focussing
      X11 = std::cos(rootKh);
      X12 = std::sin(rootKh)/rootK;
      X21 =-std::abs(kappa)*X12;
      X22 = X11;
      
      Y11 = std::cosh(rootKh);
      Y12 = std::sinh(rootKh)/rootK;
      Y21 = std::abs(kappa)*Y12;
      Y22 = Y11;
    }
  else
    {// defocussing
      X11 = cosh(rootKh);
      X12 = sinh(rootKh)/rootK;
      X21 = std::abs(kappa)*X12;
      X22 = X11;
      
      Y11 = std::cos(rootKh);
      Y12 = std::sin(rootKh)/rootK;
      Y21 = -std::abs(kappa)*Y12;
      Y22 = Y11;
    }
      
  x1  = X11*x0 + X12*xp;    
  xp1 = X21*x0 + X22*xp;
  
  y1  = Y11*y0 + Y12*yp;    
  yp1 = Y21*y0 + Y22*yp;
  
  // relies on normalised momenta otherwise this will be nan.
  zp1 = std::sqrt(1 - xp1*xp1 - yp1*yp1);
  if (std::isnan(zp1))
    {zp1 = zp;} // ensure not nan

  // new z position will be projection of h onto the z axis
  G4double z1 = z0 + std::sqrt(h2 - std::pow(x1-x0,2) - std::pow(y1-y0,2));

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


