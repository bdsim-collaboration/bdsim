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
#include "BDSFieldMagDipole.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorDipoleFringe.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

#include <cmath>

G4double BDSIntegratorDipoleFringe::thinElementLength = -1; // unphysical

BDSIntegratorDipoleFringe::BDSIntegratorDipoleFringe(BDSMagnetStrength const* strengthIn,
						     G4double                 brhoIn,
						     G4Mag_EqRhs*             eqOfMIn,
						     G4double                 minimumRadiusOfCurvatureIn,
						     const G4double&          tiltIn):
  BDSIntegratorDipoleRodrigues2(eqOfMIn, minimumRadiusOfCurvatureIn),
  rho(1.0),
  fieldArcLength((*strengthIn)["length"]),
  fieldAngle((*strengthIn)["angle"]),
  tilt(tiltIn),
  finiteTilt(BDS::IsFinite(tiltIn)),
  multipoleIntegrator(nullptr)
{
  if (thinElementLength < 0)
    {thinElementLength = BDSGlobalConstants::Instance()->ThinElementLength();}

  if (G4bool((*strengthIn)["isentrance"]))
    {
      polefaceAngle = (*strengthIn)["e1"];
      polefaceCurvature = (*strengthIn)["h1"];
      fringeCorr = BDS::FringeFieldCorrection(strengthIn, true);
      secondFringeCorr = BDS::SecondFringeFieldCorrection(strengthIn, true);
    }
  else  // must be exit face
    {
      polefaceAngle = (*strengthIn)["e2"];
      polefaceCurvature = (*strengthIn)["h2"];
      fringeCorr = BDS::FringeFieldCorrection(strengthIn, false);
      secondFringeCorr = BDS::SecondFringeFieldCorrection(strengthIn, false);
    }

  // store if instance is for entrance or exit fringe - determines which element parameters are used in kick
  // and ordering of kick & dipole transport
  isEntrance = (*strengthIn)["isentrance"];

  // check if field or scaling is finite, if not then set to be zero strength to later advance as a drift,
  // otherwise then calculate rho by undo-ing field scaling so rho is truly nominal for matching mad matrices
  if (!BDS::IsFinite((*strengthIn)["field"]) || !BDS::IsFinite((*strengthIn)["scaling"]))
    {zeroStrength = true;}
  else
    {
      zeroStrength = false;
      rho = (std::abs(brhoIn)/(*strengthIn)["field"]) * (*strengthIn)["scaling"];
    }

  // thin sextupole strength for curved polefaces
  G4double thinSextStrength = (-polefaceCurvature / rho) * 1.0 / std::pow(std::cos(polefaceAngle),3);

  // prepare magnet strength object for thin sextupole needed for curved polefaces
  BDSMagnetStrength* sextStrength = new BDSMagnetStrength();
  (*sextStrength)["k2"] = thinSextStrength;
  // component strength is normalised by length, no length here to set to 1
  (*sextStrength)["length"] = 1*CLHEP::m;

  // integrator for thin sextupole
  multipoleIntegrator = new BDSIntegratorMultipoleThin(sextStrength, brhoIn, eqOfMIn);
  delete sextStrength;

  BDSFieldMagDipole* dipoleField = new BDSFieldMagDipole(strengthIn);
  unitField = (dipoleField->FieldValue()).unit();
  delete dipoleField;

  bx = (*strengthIn)["bx"];
  by = (*strengthIn)["by"];

  backupStepperMomLimit = BDSGlobalConstants::Instance()->BackupStepperMomLimit();
}

BDSIntegratorDipoleFringe::~BDSIntegratorDipoleFringe()
{
  delete multipoleIntegrator;
}

void BDSIntegratorDipoleFringe::Stepper(const G4double yIn[6],
					const G4double dydx[6],
					const G4double h,
					G4double       yOut[6],
					G4double       yErr[6])
{
  // unit normalisation
  const G4double fcof = eqOfM->FCof();

  BaseStepper(yIn, dydx, h, yOut, yErr, fcof, 1.0);
}

void BDSIntegratorDipoleFringe::BaseStepper(const G4double  yIn[6],
					    const G4double  dydx[6],
					    const G4double& h,
					    G4double        yOut[6],
					    G4double        yErr[6],
					    const G4double& fcof,
					    const G4double& momScaling)
{
  // Protect against neutral particles, and zero field: drift through.
  if (!BDS::IsFiniteStrength(fcof) || zeroStrength)
    {
      AdvanceDriftMag(yIn,h,yOut,yErr);
      FudgeDistChordToZero(); // see doxygen in header for explanation
      return;
    }

  // container for multipole kick output, used as dipole step input
  G4double yMultipoleOut[6];
  // copy input coords as initials as multipole kick method not called
  for (G4int i = 0; i < 3; i++)
    {
      yMultipoleOut[i]     = yIn[i];
      yMultipoleOut[i + 3] = yIn[i + 3];
    }
  // apply thin multipole kick if finite curvature. Does not step, stepping occurs in dipole integrator
  if (multipoleIntegrator)
    {MultipoleStep(yIn, yMultipoleOut, h);}

  // container for copying multipole kick output (entrance fringe) or dipole step output (exit fringe)
  G4double yTemp[6];

  // only do the dipole transport before the fringe kick if it's an exit fringe, otherwise copy the
  // coords and continue
  if (!isEntrance)
    {
      // do the dipole kick and step using base class
      BDSIntegratorDipoleRodrigues2::Stepper(yMultipoleOut, dydx, h, yTemp, yErr); // yErr is correct output variable
    }
  else
    {
      for (G4int i = 0; i < 6; i++)
        {yTemp[i] = yMultipoleOut[i];}
    }
  
  // only apply the kick if we're taking a step longer than half the length of the item,
  // in which case, apply the full kick. This appears more robust than scaling the kick
  // by h / thinElementLength as the precise geometrical length depends on the geometry
  // ie if there's a beam pipe etc -> more length safetys.  The geometry layout should
  // prevent more than one step begin taken, but occasionally, a very small initial step
  // can be taken resulting in a double kick.
  G4double lengthFraction = h / thinElementLength;

  // don't do fringe kick if we're sampling the field for a long step or if it's a half
  // step inside the thin element. Apply the dipole motion only if this step is in an
  // entrance fringe, it should have already been applied if is is an exit face
  if ((h > 1*CLHEP::cm) || (lengthFraction < 0.501))
    {
      if (isEntrance)
        {BDSIntegratorDipoleRodrigues2::Stepper(yTemp, dydx, h, yOut, yErr);}
      else
        {
          for (G4int i = 0; i < 3; i++)
            {
              yOut[i]     = yTemp[i];
              yOut[i + 3] = yTemp[i + 3];
            }
        }
      return;
    }

  // position and momentum post dipole kick.
  G4ThreeVector pos = G4ThreeVector(yTemp[0], yTemp[1], yTemp[2]);
  G4ThreeVector mom = G4ThreeVector(yTemp[3], yTemp[4], yTemp[5]);

  BDSStep  localPosMom    = GlobalToCurvilinear(pos, mom, h, true);
  if (finiteTilt)
    {localPosMom = localPosMom.rotateZ(-tilt);}
  G4ThreeVector localPos  = localPosMom.PreStepPoint();
  G4ThreeVector localMom  = localPosMom.PostStepPoint();
  G4ThreeVector localMomU = localMom.unit();

  // check for forward going paraxial particles - only
  // if this is an entrance face then the dipole transport hasn't been applied yet, so apply it to ensure
  // the particle advances along the step and then exit. If it's an exit face, then the dipole transport
  // has already been applied, so simply copy the output from the dipole transport above and exit.
  if (localMomU.z() < (1.0-backupStepperMomLimit))
    {
      if (isEntrance)
        {BDSIntegratorDipoleRodrigues2::Stepper(yTemp, dydx, h, yOut, yErr);}
      else
        {
          for (G4int i = 0; i < 3; i++)
            {
              yOut[i]     = yTemp[i];
              yOut[i + 3] = yTemp[i + 3];
            }
        }		
      return;
    }

  // calculate new position and momentum kick from fringe effect only
  G4ThreeVector localCLPosOut;
  G4ThreeVector localCLMomOut;

  // normalise to particle charge
  G4double charge = fcof / std::abs(fcof);
  G4double bendingRad = rho * charge;

  // normalise to momentum - passed in from derived class.
  bendingRad *= momScaling;

  // rotate by pi/2 if field is only horizontal and tilt is zero - can only be from a vertical kicker
  if (!BDS::IsFinite(tilt) && BDS::IsFinite(bx) && (!BDS::IsFinite(by)))
    {
      // rotate only by the sign of the field and not the magnitude.
      G4double sign = bx < 0 ? -1 : 1;
      G4double verticalFringeTiltAngle = 0.5*CLHEP::pi * sign;

      localPos.rotateZ(-verticalFringeTiltAngle);
      localMomU.rotateZ(-verticalFringeTiltAngle);
      OneStep(localPos, localMomU, localCLPosOut, localCLMomOut, bendingRad);
      localCLPosOut.rotateZ(verticalFringeTiltAngle);
      localCLMomOut.rotateZ(verticalFringeTiltAngle);
    }
  else // apply fringe field kick
    {OneStep(localPos, localMomU, localCLPosOut, localCLMomOut, bendingRad);}

  // convert to global coordinates for output
  if (finiteTilt)
    {
      //localCLPosOut = localCLPosOut.rotateZ(tilt); // we only need the momentum - ignore
      localCLMomOut = localCLMomOut.rotateZ(tilt);
    }
  
  G4ThreeVector globalMom = ConvertAxisToGlobal(localCLMomOut, true);
  globalMom *= mom.mag();
  // error along direction of travel really
  G4ThreeVector globalMomU = globalMom.unit();
  globalMomU *= 1e-8;

  // container if dipole step still needs to be taken if fringe is an entrance fringe
  G4double yTempOut[6];

  // copy out values and errors
  for (G4int i = 0; i < 3; i++)
    {
	  yTempOut[i]     = pos[i];
	  yTempOut[i + 3] = globalMom[i];
      yErr[i]         = globalMomU[i]*1e-10;
      yErr[i + 3]     = 1e-40;
    }

  // now only do the dipole transport after the fringe kick if it's an entrance fringe, otherwise copy the
  // coords to the output container
  if (isEntrance)
    {
      // do the dipole kick and step using base class
      BDSIntegratorDipoleRodrigues2::Stepper(yTempOut, dydx, h, yOut, yErr); // yErr is correct output variable
    }
  else
    {
      for (G4int i = 0; i < 6; i++)
        {yOut[i] = yTempOut[i];}
    }
}

void BDSIntegratorDipoleFringe::OneStep(const G4ThreeVector& posIn,
					const G4ThreeVector& momUIn,
					G4ThreeVector&       posOut,
					G4ThreeVector&       momOut,
					const G4double&      bendingRadius) const
{
  G4double x0 = posIn.x() / CLHEP::m;
  G4double y0 = posIn.y() / CLHEP::m;
  G4double s0 = posIn.z();
  G4double xp = momUIn.x();
  G4double yp = momUIn.y();
  G4double zp = momUIn.z();

  G4double x1 = x0;
  G4double y1 = y0;
  G4double s1 = s0;
  G4double xp1 = xp;
  G4double yp1 = yp;
  G4double zp1 = zp;

  G4double X11=0,X12=0,X21=0,X22 = 0;
  G4double Y11=0,Y12=0,Y21=0,Y22 = 0;

  // fringe field correction using normalised rho
  G4double fringeFieldCorrection = (fringeCorr/bendingRadius) * (1.0 - secondFringeCorr/bendingRadius);

  // calculate fringe field kick
  X11 = 1;
  X21 = std::tan(polefaceAngle) / (bendingRadius / CLHEP::m);
  X22 = 1;

  Y11 = 1;
  Y21 = -std::tan(polefaceAngle - fringeFieldCorrection) / (bendingRadius / CLHEP::m);
  Y22 = 1;

  x1  = X11*x0 + X12*xp;
  xp1 = X21*x0 + X22*xp;
  y1  = Y11*y0 + Y12*yp;
  yp1 = Y21*y0 + Y22*yp;

  // relies on normalised momenta otherwise this will be nan.
  zp1 = std::sqrt(1 - xp1*xp1 - yp1*yp1);
  if (std::isnan(zp1))
    {zp1 = zp;} // ensure not nan

  momOut = G4ThreeVector(xp1, yp1, zp1);
  posOut = G4ThreeVector(x1*CLHEP::m, y1*CLHEP::m, s1);
}

void BDSIntegratorDipoleFringe::MultipoleStep(const G4double  yIn[6],
                                              G4double        yMultipoleOut[6],
                                              const G4double& h)
{
  // convert to local curvilinear co-ordinates
  G4ThreeVector pos = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector mom = G4ThreeVector(yIn[3], yIn[4], yIn[5]);

  BDSStep localPosMom     = GlobalToCurvilinear(pos, mom, h, true);
  G4ThreeVector localPos  = localPosMom.PreStepPoint();
  G4ThreeVector localMom  = localPosMom.PostStepPoint();
  G4ThreeVector localMomU = localMom.unit();

  // initialise new position and momentum for curvature effect as input coords
  G4ThreeVector localCLPosOut = localPos;
  G4ThreeVector localCLMomOut = localMomU;

  // Zero step length as stepping has already been applied in base class stepper
  multipoleIntegrator->OneStep(localPos, localMomU, localMom.mag(), localCLPosOut, localCLMomOut, 0);

  G4ThreeVector globalMom = ConvertAxisToGlobal(localCLMomOut, true);
  globalMom *= mom.mag();

  // write out values
  for (G4int i = 0; i < 3; i++)
    {
      yMultipoleOut[i]     = pos[i];
      yMultipoleOut[i + 3] = globalMom[i];
    }
}

// fringe field correction terms are BDS namespace methods instead of class methods
// as the functions are called during BDSIntegratorDipoleFringe construction.
// The fringe field correction terms should be cached at construction rather than
// calculated every time the integrator is called.

G4double BDS::FringeFieldCorrection(BDSMagnetStrength const* strength,
                                    G4bool isEntrance)
{
  G4double fint;
  G4double pfAngle;
  if (isEntrance)
    {
      fint = (*strength)["fint"];
      pfAngle = (*strength)["e1"];
    }
  else // must be exit face
    {
      fint = (*strength)["fintx"];
      pfAngle = (*strength)["e2"];
    }
  G4double hgap = (*strength)["hgap"];
  G4double vertGap = 2 * hgap;
  G4double corrValue = fint * vertGap * (1.0 + std::pow(std::sin(pfAngle),2)) / std::cos(pfAngle);
  return corrValue;
}

G4double BDS::SecondFringeFieldCorrection(BDSMagnetStrength const* strength,
                                          G4bool isEntrance)
{
  G4double fint;
  G4double fintK2;
  G4double pfAngle;
  if (isEntrance)
    {
      fint    = (*strength)["fint"];
      fintK2  = (*strength)["fintk2"];
      pfAngle = (*strength)["e1"];
    }
  else // must be exit face
    {
      fint    = (*strength)["fintx"];
      fintK2  = (*strength)["fintxk2"];
      pfAngle = (*strength)["e2"];
    }
  G4double hgap    = (*strength)["hgap"];
  G4double vertGap = 2 * hgap;
  G4double corrValue = fint * fintK2 * vertGap * std::tan(pfAngle);
  return corrValue;
}
