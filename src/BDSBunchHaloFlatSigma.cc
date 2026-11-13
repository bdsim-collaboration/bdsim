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
#include "BDSBunchHaloFlatSigma.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSParticleCoordsFull.hh"

#include "parser/beam.h"

#include "globals.hh" // geant4 types / globals

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "Randomize.hh"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <utility>
#include <vector>


namespace BDS
{
  PhaseSpaceCoord PhaseSpaceCoordFromActionAngle(ActionAngleCoord aa, const TwissPair& tp)
  {
    G4double action = aa.action;
    G4double angle = aa.angle;
    G4double alpha = tp.alpha;
    G4double beta = tp.beta;
    G4double x = std::sqrt(2 * action * beta) * std::cos(angle);
    G4double xp = (-std::sqrt(2 * action / beta) * (std::sin(angle) + alpha * std::cos(angle)));
    PhaseSpaceCoord result = {x, xp};
    return result;
  }

  EllipsePointGenerator::EllipsePointGenerator(G4double actionIn,
                                               const TwissPair& tp):
    action(actionIn),
    twisspair(tp)
  {
    // Here: populate angles vector from 0 to 2pi and the corresponding arc
    // lengths from angle=0 to each angle.
    G4int npoints = 100;
    std::vector<PhaseSpaceCoord> points;
    points.reserve(npoints);
    // Sample points on the ellipse equidistantly.
    // leq (<=) so that can can interpolate up to 2pi.
    for (G4int i = 0; i <= npoints; ++i)
      {
        G4double angle = i * CLHEP::twopi / npoints;
        angles.push_back(angle);
        ActionAngleCoord aa = {action, angle};
        points.push_back(PhaseSpaceCoordFromActionAngle(aa, twisspair));
      }
    // Get the cumulative distances from angle=0 to each angle.  The distance
    // between the first and second point is of course non-zero.  So we have to
    // consider the case where we roll a pathLength between 0 and the first
    // distance.  To do this we insert 0 at the start.
    std::vector<G4double> cumulativeDistances = CumulativeDistances(points);
    cumulativeDistances.insert(std::begin(cumulativeDistances), 0);
    pathLengths = std::move(cumulativeDistances);
  }

  PhaseSpaceCoord EllipsePointGenerator::GetRandomPointOnEllipse() const
  {
    // Select random point on the perimeter
    G4double pathLength = EllipsePerimeter() * G4RandFlat::shoot();
    // Invert to get the angle and then use that to get the (x, xp) pair.
    G4double angle = PathLengthToAngle(pathLength);
    ActionAngleCoord aa = {action, angle};
    return PhaseSpaceCoordFromActionAngle(aa, twisspair);
  }
  
  G4double EllipsePointGenerator::PathLengthToAngle(G4double pathLength) const
  {
    // Find position of pathLength in pathLengths
    auto it = std::lower_bound(std::begin(pathLengths), std::end(pathLengths), pathLength);
    auto n = std::distance(std::begin(pathLengths), it);

    // Get path lengths and angles either side of the point we are trying to
    // evaluate.
    G4double s1 = *it;
    G4double s0 = *(--it);
    G4double angle0 = *std::next(std::begin(angles), n - 1);
    G4double angle1 = *std::next(std::begin(angles), n);

    // Do the linear interpolation
    G4double angle = angle0 + (pathLength - s0) * ((angle1 - angle0) / (s1 - s0));
    return angle;
  }
}

BDSBunchHaloFlatSigma::BDSBunchHaloFlatSigma():
  BDSBunch("halosigma"),
  alphaX(0.0), alphaY(0.0),
  betaX(0.0),  betaY(0.0),
  emitX(0.0), emitY(0.0),
  sigmaX(0.0),  sigmaY(0.0),
  haloNSigmaXInner(0.0), haloNSigmaXOuter(0.0),
  haloNSigmaYInner(0.0), haloNSigmaYOuter(0.0)
{;}

void BDSBunchHaloFlatSigma::SetOptions(const BDSParticleDefinition* beamParticle,
                                       const GMAD::Beam&   beam,
                                       const BDSBunchType& distrType,
                                       G4Transform3D       beamlineTransformIn,
                                       const G4double      beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  alphaX = G4double(beam.alfx);
  alphaY = G4double(beam.alfy);
  betaX  = G4double(beam.betx);
  betaY  = G4double(beam.bety);
  haloNSigmaXInner = G4double(beam.haloNSigmaXInner);
  haloNSigmaXOuter = G4double(beam.haloNSigmaXOuter);
  haloNSigmaYInner = G4double(beam.haloNSigmaYInner);
  haloNSigmaYOuter = G4double(beam.haloNSigmaYOuter);

  G4double ex, ey; // dummy variables we don't need
  SetEmittances(beamParticle, beam, emitX, emitY, ex, ey);

  sigmaX = std::sqrt(emitX * betaX);
  sigmaY = std::sqrt(emitY * betaY);

  CheckParameters();
}

BDSParticleCoordsFull BDSBunchHaloFlatSigma::GetNextParticleLocal()
{
  // Sampler flat in nsigma between lower and upper.
  G4double xsigrange = (haloNSigmaXOuter - haloNSigmaXInner);
  G4double ysigrange = (haloNSigmaYOuter - haloNSigmaYInner);
  G4double xnsig = haloNSigmaXInner + G4RandFlat::shoot() * xsigrange;
  G4double ynsig = haloNSigmaYInner + G4RandFlat::shoot() * ysigrange;

  // Courant-snyder invariants for the particle, which are actually twice the
  // particle actions...
  G4double csix = std::pow(xnsig * sigmaX, 2) / betaX;
  G4double csiy = std::pow(ynsig * sigmaY, 2) / betaY;
  // The particle actions, which we use here.
  G4double jx = csix / 2.0;
  G4double jy = csiy / 2.0;
  
  BDS::TwissPair tx = {alphaX, betaX};
  BDS::TwissPair ty = {alphaY, betaY};
  
  BDS::EllipsePointGenerator epgx = BDS::EllipsePointGenerator(jx, tx);
  BDS::EllipsePointGenerator epgy = BDS::EllipsePointGenerator(jy, ty);
  auto xps = epgx.GetRandomPointOnEllipse();
  auto yps = epgy.GetRandomPointOnEllipse();
  
  G4double x  = (xps.position + X0)  * CLHEP::m;
  G4double xp = (xps.momentum + Xp0) * CLHEP::rad;
  G4double y  = (yps.position + Y0)  * CLHEP::m;
  G4double yp = (yps.momentum + Yp0) * CLHEP::rad;
  G4double z  = 0.;
  G4double zp = CalculateZp(xp, yp, Zp0);
  G4double weight = 1.0;
  BDSParticleCoordsFull result(x, y, z, xp, yp, zp, T0, S0 + z, E0, weight);
  return result;
}

void BDSBunchHaloFlatSigma::CheckParameters()
{
  BDSBunch::CheckParameters();

  if (emitX <= 0)
    {throw BDSException(__METHOD_NAME__, "emitx must be > 0!");}
  if (emitY <= 0)
    {throw BDSException(__METHOD_NAME__, "emity must be > 0!");}

  if (haloNSigmaXInner <= 0)
    {throw BDSException(__METHOD_NAME__, "haloNSigmaXInner <= 0");}

  if (haloNSigmaYInner <= 0)
    {throw BDSException(__METHOD_NAME__, "haloNSigmaYInner <= 0");}

  if (haloNSigmaXInner > haloNSigmaXOuter)
    {throw BDSException(__METHOD_NAME__, "haloNSigmaXInner cannot be less than haloNSigmaXOuter");}

  if (haloNSigmaYInner > haloNSigmaYOuter)
    {throw BDSException(__METHOD_NAME__, "haloNSigmaYInner cannot be less than haloNSigmaYOuter");}
}
