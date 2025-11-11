/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSBunchRing.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSUtilities.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <string>

BDSBunchRing::BDSBunchRing():
  BDSBunch("ring"),
  rMin(0),
  rMax(0),
  rpMin(0),
  rpMax(0),
  nonZeroRp(false),
  generateRp(false)
{;}

BDSBunchRing::~BDSBunchRing()
{;}

void BDSBunchRing::SetOptions(const BDSParticleDefinition* beamParticle,
                              const GMAD::Beam& beam,
                              const BDSBunchType& distrType,
                              G4Transform3D beamlineTransformIn,
                              const G4double beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  rMin = beam.Rmin * CLHEP::m;
  rMax = beam.Rmax * CLHEP::m;
  rpMin = beam.Rpmin * CLHEP::rad;
  rpMax = beam.Rpmax * CLHEP::rad;
  nonZeroRp = rpMin > 0 || rpMax > 0;
  generateRp = rpMin != rpMax;
}

void BDSBunchRing::CheckParameters()
{
  BDSBunch::CheckParameters();
  if (rMin < 0)
    {throw BDSException(__METHOD_NAME__, "Rmin: " + std::to_string(rMin) + " < 0");}
  if (rMax < 0)
    {throw BDSException(__METHOD_NAME__, "Rmax: " + std::to_string(rMin) + " < 0");}
  if (rMax <= rMin)
    {throw BDSException(__METHOD_NAME__, "Rmax: " + std::to_string(rMax) + " < Rmin: " + std::to_string(rMin));}
  if (rpMin < 0)
    {throw BDSException(__METHOD_NAME__, "Rpmin: " + std::to_string(rpMin) + " < 0");}
  if (rpMax < 0)
    {throw BDSException(__METHOD_NAME__, "Rpmax: " + std::to_string(rpMax) + " < 0");}
  if (rpMax < rpMin)
    {throw BDSException(__METHOD_NAME__, "Rpmax: " + std::to_string(rpMin) + " < Rpmin: " + std::to_string(rpMin));}
  if (rpMin > 1)
    {throw BDSException(__METHOD_NAME__, "Rpmin: " + std::to_string(rpMin) + " > 1");}
  if (rpMax > 1)
    {throw BDSException(__METHOD_NAME__, "Rpmax: " + std::to_string(rpMin) + " > 1");}
}

BDSParticleCoordsFull BDSBunchRing::GetNextParticleLocal()
{
  G4double r   = std::sqrt(G4RandFlat::shoot(std::pow(rMin,2), std::pow(rMax,2)));
  G4double phi = 2 * CLHEP::pi * G4RandFlat::shoot();
  G4double x   = X0 + r * std::sin(phi);
  G4double y   = Y0 + r * std::cos(phi);
  G4double xp = Xp0;
  G4double yp = Yp0;
  if (generateRp)
    {
      G4double rp   = std::sqrt(G4RandFlat::shoot(std::pow(rpMin,2), std::pow(rpMax,2)));
      xp += rp * std::sin(phi);
      yp += rp * std::cos(phi);
    }
  else if (nonZeroRp)
    {
      G4double rp = BDS::IsFinite(rpMin) ? rpMin : rpMax;
      xp += rp * std::sin(phi);
      yp += rp * std::cos(phi);
    }
  G4double zp = CalculateZp(xp,yp,Zp0);
  return BDSParticleCoordsFull(x,y,Z0,xp,yp,zp,T0,S0,E0,/*weight=*/1.0);
}
