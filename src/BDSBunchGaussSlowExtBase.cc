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
#include "BDSBunchGauss.hh"
#include "BDSBunchGaussSlowExtBase.hh"
#include "BDSBunchSlowExt.hh"
#include "BDSDebug.hh"
#include "BDSWarning.hh"

#include "parser/beam.h"

BDSBunchGaussSlowExtBase::BDSBunchGaussSlowExtBase(const G4String& nameIn):
  BDSBunch(nameIn),
  gauss(nullptr),
  slowext(new BDSBunchSlowExt())
{;}

BDSBunchGaussSlowExtBase::~BDSBunchGaussSlowExtBase()
{
  delete gauss;
  delete slowext;
}

void BDSBunchGaussSlowExtBase::SetOptions(const BDSParticleDefinition* beamParticle,
                                          const GMAD::Beam& beam,
                                          const BDSBunchType& distrType,
                                          G4Transform3D beamlineTransformIn,
                                          const G4double beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);

  gauss->SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  slowext->SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);

  if (gauss->OffsetSampleMean())
    {BDS::Warning(__METHOD_NAME__, "offsetSampleMean will only work for the Gaussian component of this slow extracted distribution");}
}

void BDSBunchGaussSlowExtBase::CheckParameters()
{
  gauss->CheckParameters();
  slowext->CheckParameters();
}

BDSParticleCoordsFull BDSBunchGaussSlowExtBase::GetNextParticleLocal()
{
  auto pGauss = gauss->GetNextParticleLocal();
  G4double dtLocal, dELocal;
  slowext->GetDeltas(dtLocal, dELocal);
  pGauss.totalEnergy += dELocal;
  pGauss.T += dtLocal;
  return pGauss;
}

void BDSBunchGaussSlowExtBase::BeginOfRunAction(G4int numberOfEvents,
                                                G4bool batchMode)
{
  gauss->BeginOfRunAction(numberOfEvents, batchMode);
}
