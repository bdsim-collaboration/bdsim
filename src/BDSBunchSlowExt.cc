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
#include "BDSBunchSlowExt.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>

BDSBunchSlowExt::BDSBunchSlowExt():
  BDSBunch("slowext"),
  dTStart(0),
  dTStop(0),
  dPStart(0),
  dPStop(0),
  applyGradient(false),
  dT(0),
  dEStart(0),
  dE(0)
{;}

BDSBunchSlowExt::~BDSBunchSlowExt()
{;}

void BDSBunchSlowExt::SetOptions(const BDSParticleDefinition* beamParticle,
                                 const GMAD::Beam& beam,
                                 const BDSBunchType& distrType,
                                 G4Transform3D beamlineTransformIn,
                                 const G4double beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  dTStart = beam.dTStart * CLHEP::s;
  dTStop = beam.dTStop * CLHEP::s;
  dPStart = beam.dPStart * CLHEP::GeV;
  dPStop = beam.dPStop * CLHEP::GeV;

  dT = dTStop - dTStart;

  // bunch definition needs total energy, so we go from dP to dE
  G4double pA = P0 + dPStart;
  G4double pB = P0 + dPStop;
  auto pdCopy = BDSParticleDefinition(*particleDefinition);
  pdCopy.SetEnergies(0, 0, pA);
  G4double EStart = pdCopy.TotalEnergy();
  pdCopy.SetEnergies(0, 0, pB);
  G4double EStop = pdCopy.TotalEnergy();
  dEStart = EStart - E0;
  dE = EStop - EStart;

  // if not difference in both time or momentum then no need to do anything
  applyGradient = BDS::IsFinite(std::abs(dTStop - dTStart)) && BDS::IsFinite(std::abs(dPStop - dPStart));

  if (!applyGradient)
    {BDS::Warning(__METHOD_NAME__, "no difference in time and momentum - no action for this distribution");}
}

void BDSBunchSlowExt::CheckParameters()
{
  BDSBunch::CheckParameters();
  if (dTStop <  dTStart)
    {throw BDSException(__METHOD_NAME__, "dTStop must be greater or equal to dTStart");}
}

BDSParticleCoordsFull BDSBunchSlowExt::GetNextParticleLocal()
{
  if (!applyGradient)
    {return BDSBunch::GetNextParticleLocal();}

  G4double dtLocal, dELocal;
  GetDeltas(dtLocal, dELocal);
  G4double t = T0 + dtLocal;
  G4double E = E0 + dELocal;
  
  return BDSParticleCoordsFull(X0,Y0,Z0,Xp0,Yp0,Zp0,t,S0,E,/*weight=*/1.0);
}

void BDSBunchSlowExt::GetDeltas(G4double& dtLocal, G4double& dELocal) const
{
  G4double fraction = G4RandFlat::shoot();
  dtLocal = dTStart + dT*fraction;
  dELocal = dEStart + dE*fraction;
}