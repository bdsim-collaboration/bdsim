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
#include "BDSIntegratorKickerThin.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

#include <cmath>

BDSIntegratorKickerThin::BDSIntegratorKickerThin(BDSMagnetStrength const* strength,
						 G4double                 brhoIn,
						 G4Mag_EqRhs*             eqOfMIn,
						 G4double                 minimumRadiusOfCurvatureIn):
  BDSIntegratorMag(eqOfMIn, 6),
  hkick((*strength)["hkick"]),
  vkick((*strength)["vkick"]),
  brho(brhoIn)
{
  // set base class member
  zeroStrength = (!BDS::IsFiniteStrength(hkick) && !BDS::IsFiniteStrength(vkick));

  // duplicate magnetstrength for fringe field integrators as all its physical parameters are set for the magnet
  // as a whole, including fringes. Only isentrance bool determines if the object is for an entrance or exit
  // fringe, therefore set as appropriate (required as BDSIntegratorDipoleFringe members are set upon
  // instantiation according to isentrance).
  BDSMagnetStrength* fringeStEntr = new BDSMagnetStrength(*strength);
  BDSMagnetStrength* fringeStExit = new BDSMagnetStrength(*strength);
  (*fringeStEntr)["isentrance"] = true;
  (*fringeStExit)["isentrance"] = false;
  // tilt is zero as only onestep function in local coords will be called in this integrator
  fringeIntEntr = new BDSIntegratorDipoleFringe(fringeStEntr, brhoIn, eqOfMIn, minimumRadiusOfCurvatureIn, 0);
  fringeIntExit = new BDSIntegratorDipoleFringe(fringeStExit, brhoIn, eqOfMIn, minimumRadiusOfCurvatureIn, 0);

  // check if the fringe effect is finite
  G4bool finiteEntrFringe = false;
  G4bool finiteExitFringe = false;
  if (BDS::IsFiniteStrength(fringeIntEntr->GetFringeCorr()) || BDS::IsFiniteStrength(fringeIntEntr->GetSecondFringeCorr()))
    {finiteEntrFringe = true;}
  if (BDS::IsFiniteStrength(fringeIntExit->GetFringeCorr()) || BDS::IsFiniteStrength(fringeIntExit->GetSecondFringeCorr()))
    {finiteExitFringe = true;}

  // only call fringe tracking if the poleface rotation or fringe field correction terms are finite
  hasEntranceFringe = false;
  hasExitFringe     = false;
  if (BDS::IsFinite(fringeIntEntr->GetPolefaceAngle()) || finiteEntrFringe)
    {hasEntranceFringe = true;}
  if (BDS::IsFinite(fringeIntExit->GetPolefaceAngle()) || finiteExitFringe)
    {hasExitFringe = true;}

  // effective bending radius needed for fringe field calculations
  if (BDS::IsFinite((*strength)["field"]))
    {rho = brhoIn/(*strength)["field"];}
  else
    {
      // zero field means rho cannot be calculated, therefore do not allow fringe kicks.
      rho = 1; // for safety
      hasEntranceFringe = false;
      hasExitFringe = false;
    }

  // tilt for vertical kickers. Poleface rotations are assumed to be about the vertical axis,
  // so effect should be applied to rotated axes.
  tiltAngle = 0;
  if (!BDS::IsFinite((*strength)["by"]) && ((*strength)["bx"] == 1.0))
    {tiltAngle = -CLHEP::pi/2.0;}

  delete fringeStEntr;
  delete fringeStExit;
}

void BDSIntegratorKickerThin::Stepper(const G4double   yIn[],
				      const G4double[] /*dydx[]*/,
				      const G4double   h,
				      G4double         yOut[],
				      G4double         yErr[])
{
  const G4double fcof = eqOfM->FCof();

  // only apply the kick if we're taking a step longer than half the length of the item,
  // in which case, apply the full kick. This appears more robust than scaling the kick
  // by h / thinElementLength as the precise geometrical length depends on the geometry
  // ie if there's a beam pipe etc -> more length safetys.  The geometry layout should
  // prevent more than one step begin taken, but occasionally, a very small initial step
  // can be taken resulting in a double kick.
  G4double lengthFraction = h / thinElementLength;
  
  if (!BDS::IsFiniteStrength(fcof) || zeroStrength || lengthFraction < 0.51)
    {// neutral particle or zero strength - drift through
      AdvanceDriftMag(yIn, h, yOut, yErr);
      SetDistChord(0);
      return;
    }

  G4ThreeVector pos = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector mom = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  
  // global to local (thinElementLength is static base class member)
  // purposively control step length as we're dealing with a very short element
  BDSStep       localPosMom  = ConvertToLocal(pos, mom, h, false, thinElementLength);
  G4ThreeVector localPos     = localPosMom.PreStepPoint();
  G4ThreeVector localMom     = localPosMom.PostStepPoint();
  G4ThreeVector localMomUnit = localMom.unit();
  G4double      localMomMag  = localMom.mag();

  // only proceed with thin kick if particle is paraxial
  // judged by forward momentum > 1-limit and |transverse| < limit (default limit=0.1)
  if (std::abs(localMomUnit.z()) < (1.0 - backupStepperMomLimit))
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      SetDistChord(0);
      return;
    }

  // normalise to particle charge for fringe/poleface effects
  G4double charge = fcof / std::abs(fcof);
  G4double bendingRad = rho * charge;

  // set as local coords in case no entrance fringe effect
  G4ThreeVector fringeEntrPosOut = localPos;
  G4ThreeVector fringeEntrMomOut = localMomUnit;

  if (hasEntranceFringe)
    {
      if (BDS::IsFinite(tiltAngle))
        {
          localPos.rotateZ(tiltAngle);
          localMomUnit.rotateZ(tiltAngle);
          fringeIntEntr->OneStep(localPos, localMomUnit, fringeEntrPosOut, fringeEntrMomOut, bendingRad);
          fringeEntrPosOut.rotateZ(-tiltAngle);
          fringeEntrMomOut.rotateZ(-tiltAngle);
        }
      else
        {fringeIntEntr->OneStep(localPos, localMomUnit, fringeEntrPosOut, fringeEntrMomOut, bendingRad);}
    }
  fringeEntrMomOut *= localMomMag; // fringe returns unit momentum m

  G4ThreeVector localPosOut;
  G4ThreeVector localMomOut;

  // do thin kicker step
  OneStep(fringeEntrPosOut, fringeEntrMomOut, localMom, h, fcof, localPosOut, localMomOut);

  // set as coords after kick in case no exit fringe effect
  G4ThreeVector fringeExitPosOut = localPosOut;
  G4ThreeVector fringeExitMomOut = localMomOut;

  if (hasExitFringe)
    {
      if (BDS::IsFinite(tiltAngle))
        {
          localPosOut.rotateZ(tiltAngle);
          localMomOut.rotateZ(tiltAngle);
          fringeIntExit->OneStep(localPosOut, localMomOut.unit(), fringeExitPosOut, fringeExitMomOut, bendingRad);
          fringeExitPosOut.rotateZ(-tiltAngle);
          fringeExitMomOut.rotateZ(-tiltAngle);
        }
      else
        {fringeIntExit->OneStep(localPosOut, localMomOut, fringeExitPosOut, fringeExitMomOut, bendingRad);}
    }
  fringeExitMomOut *= localMomMag; // fringe returns unit momentum

  // convert back to global
  ConvertToGlobal(fringeExitPosOut, fringeExitMomOut, yOut, yErr);
}

void BDSIntegratorKickerThin::OneStep(const G4ThreeVector& localPos,
                                      const G4ThreeVector& localMomUnit,
                                      const G4ThreeVector& localMom,
                                      const G4double&      h,
                                      const G4double&      fcof,
                                      G4ThreeVector&       localPosOut,
                                      G4ThreeVector&       localMomOut) const
{
  // Transverse position stays the same and the particle is advanced by h along local z.
  G4double x1 = localPos.x();
  G4double y1 = localPos.y();
  G4double z1 = localPos.z();

  G4double factor = 1; // for forwards / backwards action
  if (localMomUnit.z() < 0)
    {// backwards travelling
      z1 -= h;
      factor = -1;
    }
  else
    {z1 += h;} // forwards travelling

  G4double px = localMom.x();
  G4double py = localMom.y();
  G4double pz = localMom.z();
  
  // Scale the action for the current particle momentum w.r.t.
  // the design momentum. Even though a thin kicker, it represents
  // a thin dipole, which would affect different particles differently.
  G4double localMomMag = localMom.mag();
  G4double ratio = (fcof * brho ) / localMomMag;    
  
  G4double dpx = ratio * hkick * localMomMag * factor;
  G4double dpy = ratio * vkick * localMomMag * factor;
  
  px += dpx;
  py += dpy;
  pz -= std::abs(dpx) + std::abs(dpy); // conservation of momentum

  localPosOut = G4ThreeVector(x1, y1, z1);
  localMomOut = G4ThreeVector(px, py, pz);
}
