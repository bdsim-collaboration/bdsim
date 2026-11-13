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
#include "BDSCavityInfo.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSFieldEMRFCavity.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "globals.hh"
#include "G4ThreeVector.hh"

#include "TMath.h"

#include <cmath>
#include <utility>

const G4double BDSFieldEMRFCavity::j0FirstZero = 2.404825557695772768622;

const G4double BDSFieldEMRFCavity::Z0 = CLHEP::mu0 * CLHEP::c_light;

BDSFieldEMRFCavity::BDSFieldEMRFCavity(BDSMagnetStrength const* strength):
  BDSFieldEMRFCavity((*strength)["efield"],
                     (*strength)["frequency"],
                     (*strength)["phase"],
                     (*strength)["equatorradius"],
                     (*strength)["synchronousT0"])
{;}

BDSFieldEMRFCavity::BDSFieldEMRFCavity(G4double eFieldAmplitude,
                                       G4double frequencyIn,
                                       G4double phaseOffset,
                                       G4double cavityRadiusIn,
                                       G4double synchronousTIn):
  eFieldMax(eFieldAmplitude),
  phase(phaseOffset),
  cavityRadius(cavityRadiusIn),
  synchronousT(synchronousTIn),
  normalisedCavityRadius(j0FirstZero/cavityRadius),
  angularFrequency(CLHEP::twopi * frequencyIn)
{
  // this would cause NANs to be propagated into tracking which is really bad
  if (!BDS::IsFinite(cavityRadiusIn) || std::isnan(normalisedCavityRadius) || std::isinf(normalisedCavityRadius))
    {throw BDSException(__METHOD_NAME__, "no cavity radius supplied - required for pill box model");}
}

std::pair<G4ThreeVector, G4ThreeVector> BDSFieldEMRFCavity::GetField(const G4ThreeVector& position,
                                                                     const G4double       t) const
{
  // Converting from Local Cartesian to Local Cylindrical
  G4double phi = std::atan2(position.y(),position.x());
  G4double r   = std::hypot(position.x(),position.y());

  G4double rNormalised = normalisedCavityRadius * r;

  // In case a point outside the cavity is queried, ensure the bessel will return 0
  if (rNormalised > j0FirstZero)
    {rNormalised = j0FirstZero - 1e-6;}

  G4double J0r = TMath::BesselJ0(rNormalised);
  G4double J1r = TMath::BesselJ1(rNormalised);

  // Calculating free-space impedance and scale factor for Bphi:
  G4double hMax = -eFieldMax/Z0;
  G4double Bmax = hMax * CLHEP::mu0;

  // Calculating field components.
  G4double arg = angularFrequency*(t - synchronousT) + phase;
  G4double Ez   = eFieldMax * J0r * std::cos(arg);
  G4double Bphi = Bmax * J1r * std::sin(arg);

  // Converting Bphi into cartesian coordinates:
  G4TwoVector bxby(0,Bphi); // this is equivalent to a pi/2 rotation of (1,0)
  bxby.rotate(phi);
  G4double Bx = bxby.x();
  G4double By = bxby.y();
  
  // Local B and E fields:
  G4ThreeVector LocalB = G4ThreeVector(Bx, By, 0);
  G4ThreeVector LocalE = G4ThreeVector(0,  0,  Ez);
  
  auto result = std::make_pair(LocalB, LocalE);
  return result;
}

G4double BDSFieldEMRFCavity::TransitTimeFactor(G4double frequency,
                                               G4double phase,
                                               G4double zLength,
                                               G4double beta)
{
  G4double rfWavelength = CLHEP::c_light / frequency;
  G4double piGOverBetaLambda = (CLHEP::pi * zLength) / (beta * rfWavelength);
  G4double transitTimeFactor = std::sin(piGOverBetaLambda + phase) / piGOverBetaLambda;
  return transitTimeFactor;
}