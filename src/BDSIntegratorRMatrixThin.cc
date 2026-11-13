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
#include "BDSIntegratorRMatrixThin.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4Mag_EqRhs.hh"

BDSIntegratorRMatrixThin::BDSIntegratorRMatrixThin(BDSMagnetStrength const* strength,
                                                   G4Mag_EqRhs* eqOfMIn,
                                                   G4double maximumRadiusIn):
  BDSIntegratorMag(eqOfMIn, 6),
  maximumRadius(maximumRadiusIn)
{
  kick1   = (*strength)["kick1"];
  kick2   = (*strength)["kick2"];
  kick3   = (*strength)["kick3"];
  kick4   = (*strength)["kick4"];
  rmat11  = (*strength)["rmat11"];
  rmat12  = (*strength)["rmat12"];
  rmat13  = (*strength)["rmat13"];
  rmat14  = (*strength)["rmat14"];
  rmat21  = (*strength)["rmat21"];
  rmat22  = (*strength)["rmat22"];
  rmat23  = (*strength)["rmat23"];
  rmat24  = (*strength)["rmat24"];
  rmat31  = (*strength)["rmat31"];
  rmat32  = (*strength)["rmat32"];
  rmat33  = (*strength)["rmat33"];
  rmat34  = (*strength)["rmat34"];
  rmat41  = (*strength)["rmat41"];
  rmat42  = (*strength)["rmat42"];
  rmat43  = (*strength)["rmat43"];
  rmat44  = (*strength)["rmat44"];

#ifdef BDSDEBUG
  G4cout << "RMatrix " << rmat11 << " " << rmat12 << " " << rmat13 << " " << rmat14 << " " << kick1 << G4endl;
  G4cout << "RMatrix " << rmat21 << " " << rmat22 << " " << rmat23 << " " << rmat24 << " " << kick2 <<  G4endl;
  G4cout << "RMatrix " << rmat31 << " " << rmat32 << " " << rmat33 << " " << rmat34 << " " << kick3 <<  G4endl;
  G4cout << "RMatrix " << rmat41 << " " << rmat42 << " " << rmat43 << " " << rmat44 << " " << kick4 <<  G4endl;
#endif
}

void BDSIntegratorRMatrixThin::Stepper(const G4double yIn[],
                                       const G4double /*dydx*/[],
                                       const G4double h,
                                       G4double       yOut[],
                                       G4double       yErr[])
{
  for (G4int i = 0; i < 3; i++)
    {
      yErr[i]   = 0;
      yErr[i+3] = 0;
    }
  
  // check if beam particle, if so step as drift
  const G4double fcof = eqOfM->FCof();
  G4double lengthFraction = h / thinElementLength;

  // only apply the kick if we're taking a step longer than half the length of the item,
  // in which case, apply the full kick. This appears more robust than scaling the kick
  // by h / thinElementLength as the precise geometrical length depends on the geometry
  // ie if there's a beam pipe etc -> more length safetys.  The geometry layout should
  // prevent more than one step begin taken, but occasionally, a very small initial step
  // can be taken resulting in a double kick.
  if (lengthFraction < 0.51 || !BDS::IsFiniteStrength(fcof))
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      SetDistChord(0);
      return;
    }

  G4ThreeVector pos = G4ThreeVector( yIn[0], yIn[1], yIn[2]);
  G4ThreeVector mom = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  G4double      momMag = mom.mag();

  BDSStep       localPosMom  = ConvertToLocal(pos, mom, h, false);
  G4ThreeVector localPos     = localPosMom.PreStepPoint();
  G4ThreeVector localMom     = localPosMom.PostStepPoint();
  G4ThreeVector localMomUnit = localMom.unit();

  G4double x0 = localPos.x();
  G4double y0 = localPos.y();
  G4double z0 = localPos.z();

  G4double xp = localMomUnit.x();
  G4double yp = localMomUnit.y();
  G4double zp = localMomUnit.z();

  // only proceed with thin matrix if particle is paraxial
  // judged by forward momentum > 1-limit and |transverse| < limit (default limit=0.1)
  if (zp < (1.0-backupStepperMomLimit) || std::abs(xp) > backupStepperMomLimit || std::abs(yp) > backupStepperMomLimit)
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      SetDistChord(0);
      return;
    }

  G4double x1    = rmat11 * x0 + rmat12 * xp * CLHEP::m + rmat13 * y0 + rmat14 * yp * CLHEP::m + kick1;
  G4double xp1   = rmat21 * x0 / CLHEP::m + rmat22 * xp + rmat23 * y0 / CLHEP::m + rmat24 * yp + kick2;
  G4double y1    = rmat31 * x0 + rmat32 * xp * CLHEP::meter + rmat33 * y0 + rmat34 * yp * CLHEP::m + kick3;
  G4double yp1   = rmat41 * x0 / CLHEP::m + rmat42 * xp + rmat43 * y0 / CLHEP::m + rmat44 * yp + kick4;
  G4double z1    = z0 + h;
  G4double zp1 = std::sqrt(1 - std::pow(xp1,2) - std::pow(yp1,2));
  
  // need to check against aperture before returning
  if(x1 > maximumRadius)
    {x1 = maximumRadius;}
  else if( x1 < -maximumRadius)
    {x1 = -maximumRadius;}
  if(y1 > maximumRadius)
    {y1 = maximumRadius;}
  else if( y1 < -maximumRadius)
    {y1 = -maximumRadius;}

  G4ThreeVector localPosOut     = G4ThreeVector(x1, y1, z1);
  G4ThreeVector localMomUnitOut = G4ThreeVector(xp1, yp1, zp1);
  ConvertToGlobal(localPosOut, localMomUnitOut, yOut, yErr, momMag);
}
