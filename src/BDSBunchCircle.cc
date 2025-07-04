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
#include "BDSBunchCircle.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSBunchCircle::BDSBunchCircle():
  BDSBunch("circle"),
  envelopeR(0.0),
  envelopeRp(0.0),
  envelopeT(0.0),
  envelopeE(0.0)
{;}

BDSBunchCircle::~BDSBunchCircle() 
{;}

void BDSBunchCircle::SetOptions(const BDSParticleDefinition* beamParticle,
                                const GMAD::Beam& beam,
                                const BDSBunchType& distrType,
                                G4Transform3D beamlineTransformIn,
                                const G4double beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  envelopeR  = beam.envelopeR  * CLHEP::m; 
  envelopeRp = beam.envelopeRp * CLHEP::rad;
  envelopeT  = beam.envelopeT  * CLHEP::s;
  envelopeE  = beam.envelopeE  * CLHEP::GeV; 
}

void BDSBunchCircle::CheckParameters()
{
  BDSBunch::CheckParameters();
  if (envelopeR <= 0)
    {throw BDSException(__METHOD_NAME__, "envelopeR <= 0");}
  if (envelopeRp < 0)
    {throw BDSException(__METHOD_NAME__, "envelopeRp <= 0");}
  if (envelopeT < 0)
    {throw BDSException(__METHOD_NAME__, "envelopeT < 0");}
  if (envelopeE < 0)
    {throw BDSException(__METHOD_NAME__, "envelopeE < 0");}
}

BDSParticleCoordsFull BDSBunchCircle::GetNextParticleLocal()
{
  G4double dt    = envelopeT * (1.-2.*G4RandFlat::shoot());
  G4double dz    = dt * CLHEP::c_light;

  G4double t     = T0 + dt;
  G4double z     = Z0 + dz;
  G4double phiR  = G4RandFlat::shoot() * CLHEP::twopi;
  G4double phiRp = G4RandFlat::shoot() * CLHEP::twopi;
  G4double r     = std::sqrt(G4RandFlat::shoot()) * envelopeR;
  G4double rp    = std::sqrt(G4RandFlat::shoot()) * envelopeRp;

  G4double x  = X0  + std::cos(phiR)  * r;
  G4double y  = Y0  + std::sin(phiR)  * r;
  G4double xp = Xp0 + std::cos(phiRp) * rp;
  G4double yp = Yp0 + std::sin(phiRp) * rp; 
  G4double zp = CalculateZp(xp,yp,Zp0);
  G4double E  = E0 + envelopeE * (1-2*G4RandFlat::shoot());
  
  return BDSParticleCoordsFull(x,y,z,xp,yp,zp,t,S0-dz,E,/*weight=*/1.0);
}
