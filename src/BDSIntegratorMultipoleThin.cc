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
#include "BDSIntegratorMultipoleThin.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

#include <cmath>
#include <list>
#include <vector>
#include <include/BDSGlobalConstants.hh>


BDSIntegratorMultipoleThin::BDSIntegratorMultipoleThin(BDSMagnetStrength const* strength,
						       G4double                 brhoIn,
						       G4Mag_EqRhs*             eqOfMIn):
  BDSIntegratorMag(eqOfMIn, 6),
  brho(brhoIn)
{
  b0l = (*strength)["field"] * brho;
  G4double l = (*strength)["length"] / CLHEP::m;
  // avoid potential div by zero in strength normalisation
  if (!BDS::IsFinite(l))
    {l=1*CLHEP::m;}
  std::vector<G4String> normKeys = strength->NormalComponentKeys();
  std::vector<G4String> skewKeys = strength->SkewComponentKeys();
  std::vector<G4String>::iterator nkey = normKeys.begin();
  std::vector<G4String>::iterator skey = skewKeys.begin();
  for (G4double i = 0; i < normKeys.size(); i++, ++nkey, ++skey)
    {
      bnl.push_back((*strength)[*nkey] / (l*std::pow(CLHEP::m,i+1)));
      bsl.push_back((*strength)[*skey] / (l*std::pow(CLHEP::m,i+1)));
      nfact.push_back(Factorial(i));
    }

  G4bool finiteStrength = false;
  for (const auto& nl : bnl)
    {finiteStrength = finiteStrength || BDS::IsFiniteStrength(nl);}
  for (const auto& sl : bsl)
    {finiteStrength = finiteStrength || BDS::IsFiniteStrength(sl);}
  zeroStrength = !finiteStrength;
}

void BDSIntegratorMultipoleThin::Stepper(const G4double yIn[],
					 const G4double[] /*dydx[]*/,
					 const G4double h,
					 G4double       yOut[],
					 G4double       yErr[])
{
  const G4double fcof = eqOfM->FCof();
  G4double lengthFraction = h / thinElementLength;

  // only apply the kick if we're taking a step longer than half the length of the item,
  // in which case, apply the full kick. This appears more robust than scaling the kick
  // by h / thinElementLength as the precise geometrical length depends on the geometry
  // ie if there's a beam pipe etc -> more length safetys.  The geometry layout should
  // prevent more than one step begin taken, but occasionally, a very small initial step
  // can be taken resulting in a double kick.
  if (zeroStrength || lengthFraction < 0.51 || !BDS::IsFiniteStrength(fcof))
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      SetDistChord(0);
      return;
    }

  G4ThreeVector pos    = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector mom    = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  G4double      momMag = mom.mag();

  // global to local
  BDSStep       localPosMom  = ConvertToLocal(pos, mom, h, false, thinElementLength);
  G4ThreeVector localPos     = localPosMom.PreStepPoint();
  G4ThreeVector localMom     = localPosMom.PostStepPoint();
  G4ThreeVector localMomUnit = localMom.unit();

  // only proceed with thin multipole step if particle is paraxial
  // judged by forward momentum > 1-limit and |transverse| < limit (default limit=0.1)
  if (localMomUnit.z() < (1.0 - backupStepperMomLimit))
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      SetDistChord(0);
      return;
    }

  G4ThreeVector localPosOut;
  G4ThreeVector localMomUnitOut;

  OneStep(localPos, localMomUnit, momMag, localPosOut, localMomUnitOut, h);

  // xp1 or yp1 may be > 1, so isnan check also needed for zp1.
  G4double zp1 = localMomUnitOut.z();
  if (std::isnan(zp1) || (zp1 < backupStepperMomLimit))
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      SetDistChord(0);
      return;
    }


  ConvertToGlobal(localPosOut, localMomUnitOut, yOut, yErr, momMag);
}

void BDSIntegratorMultipoleThin::OneStep(const G4ThreeVector& posIn,
                                         const G4ThreeVector& momUIn,
                                         const G4double       momIn,
                                         G4ThreeVector&       posOut,
                                         G4ThreeVector&       momOut,
                                         G4double             h) const
{
  G4double x0  = posIn.x();
  G4double y0  = posIn.y();
  G4double z0  = posIn.z();
  G4double xp  = momUIn.x();
  G4double yp  = momUIn.y();
  G4double zp  = momUIn.z();

  // initialise output variables with input position as default
  G4double x1  = x0;
  G4double y1  = y0;
  G4double z1  = z0 + h; // new z position will be along z by step length h
  G4double xp1 = xp;
  G4double yp1 = yp;
  G4double zp1 = zp;

  // kicks come from pg 27 of mad-8 physical methods manual
  G4complex kick(0,0);
  G4complex position(x0, y0);
  G4complex result(0,0);
  // components of complex vector
  G4double knReal = 0;
  G4double knImag = 0;
  G4double momx;
  G4double momy;

  // normalise to momentum and charge
  G4double ratio = eqOfM->FCof() * std::abs(brho) / momIn;

  G4int n = 1;
  std::list<double>::const_iterator kn = bnl.begin();

  // sum higher order components into one kick
  for (; kn != bnl.end(); n++, ++kn)
    {
      momx = 0; //reset to zero
      momy = 0;
      knReal = (*kn) * ratio * std::pow(position,n).real() / nfact[n];
      knImag = (*kn) * ratio * std::pow(position,n).imag() / nfact[n];
      if (!std::isnan(knReal))
    {momx = knReal;}
      if (!std::isnan(knImag))
    {momy = knImag;}
      result = {momx,momy};
      kick += result;
    }

  // reset n for skewed kicks.
  n=1;
  G4double ksReal = 0;
  G4double ksImag = 0;
  G4complex skewkick(0,0);

  std::list<double>::const_iterator ks = bsl.begin();
  for (; ks != bsl.end(); n++, ++ks)
    {
      if (BDS::IsFinite(*ks))
        {
          //reset to zero
          momx = 0;
          momy = 0;
          ksReal = (*ks) * ratio * std::pow(position, n).real() / nfact[n];
          ksImag = (*ks) * ratio * std::pow(position, n).imag() / nfact[n];
          if (!std::isnan(ksReal))
            {momx = ksReal;}
          if (!std::isnan(ksImag))
            {momy = ksImag;}
          result = {momx,momy};
          skewkick += result;
        }
    }

  // apply normal kick
  xp1 -= kick.real();
  yp1 += kick.imag();

  //apply skewed kick
  xp1 -= skewkick.imag();
  yp1 += skewkick.real();

  zp1 = std::sqrt(1 - std::pow(xp1,2) - std::pow(yp1,2));
  if (std::isnan(zp1))
    {zp1 = zp;}

  posOut = G4ThreeVector(x1, y1, z1);
  momOut = G4ThreeVector(xp1, yp1, zp1);
}

G4int BDSIntegratorMultipoleThin::Factorial(G4int n)
{
  G4int result = 1;
  for (G4int i = 1; i <= n; i++)
    {result *= i;}
  return result;
}
