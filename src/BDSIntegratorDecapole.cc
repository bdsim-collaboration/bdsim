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
#include "BDSIntegratorDecapole.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Mag_EqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSIntegratorDecapole::BDSIntegratorDecapole(BDSMagnetStrength const* strength,
					     G4double                 brho,
					     G4Mag_EqRhs*             eqOfMIn):
  BDSIntegratorEulerOld(eqOfMIn)
{
  // B'''' = d^4By/dx^4 = Brho * (1/Brho d^4By/dx^4) = Brho * k4
  bQuadruplePrime = brho * (*strength)["k4"] / (CLHEP::m3*CLHEP::m2);
  zeroStrength = !BDS::IsFinite(bQuadruplePrime);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B'''' = " << bQuadruplePrime << G4endl;
#endif
}

void BDSIntegratorDecapole::AdvanceHelix(const G4double  yIn[],
					 G4double        h,
					 G4double        yOut[],
                     G4double        yErr[])
{
  G4ThreeVector mom = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  G4double momMag   = mom.mag();
  G4double kappa    = -eqOfM->FCof()*bQuadruplePrime / momMag;

  if(std::abs(kappa)<1.e-20)
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      SetDistChord(0);
      return;
    }

  G4ThreeVector pos          = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  BDSStep       localPosMom  = ConvertToLocal(pos, mom, h, false);
  G4ThreeVector localPos     = localPosMom.PreStepPoint();
  G4ThreeVector localMom     = localPosMom.PostStepPoint();
  G4ThreeVector localMomUnit = localMom.unit();
  
  G4double x0 = localPos.x();
  G4double y0 = localPos.y();

  // Evaluate field at the approximate midpoint of the step.
  const G4double halfH = 0.5*h;
  x0 = x0 + localMomUnit.x()*halfH;
  y0 = y0 + localMomUnit.y()*halfH;
  
  G4double xp = localMomUnit.x();
  G4double yp = localMomUnit.y();
  G4double zp = localMomUnit.z();

  G4double xsq = std::pow(x0, 2);
  G4double ysq = std::pow(y0, 2);
  G4double x02My02 = (xsq - ysq); 
  G4double Bx = 4.0*x0*y0 * (x02My02);
  G4double By = std::pow(x0,4) - 6.0 * xsq * ysq + std::pow(y0,4);
  
  // local r'' (for curvature)
  G4ThreeVector localA;
  localA.setX( zp*By);
  localA.setY(-zp*Bx);
  localA.setZ( xp*By - yp*Bx);
  
  localA *= kappa / 24; // 24 is actually a 4! factor.;
  
  AdvanceChord(h,localPos,localMomUnit,localA);
  ConvertToGlobal(localPos, localMomUnit, yOut, yErr, momMag);
}
