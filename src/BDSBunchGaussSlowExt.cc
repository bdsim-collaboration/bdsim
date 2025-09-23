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
#include "BDSBunchGaussSlowExt.hh"
#include "BDSBunchSlowExt.hh"
#include "BDSDebug.hh"
#include "BDSWarning.hh"

#include "parser/beam.h"


BDSBunchGaussSlowExt::BDSBunchGaussSlowExt():
  BDSBunch("gaussslowext"),
  gauss(new BDSBunchGauss()),
  slowext(new BDSBunchSlowExt())
{;}

BDSBunchGaussSlowExt::~BDSBunchGaussSlowExt()
{
  delete gauss;
  delete slowext;
}

void BDSBunchGaussSlowExt::SetOptions(const BDSParticleDefinition* beamParticle,
                                         const GMAD::Beam& beam,
                                         const BDSBunchType& distrType,
                                         G4Transform3D beamlineTransformIn,
                                         const G4double beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);

  gauss->SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  slowext->SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);

  if (gauss->OffsetSampleMean())
    {BDS::Warning(__METHOD_NAME__, "offsetSampleMean will only work for the gauss component of this slow extracted distribution");}
}

BDSParticleCoordsFull BDSBunchGaussSlowExt::GetNextParticleLocal()
{
  auto pGauss = gauss->GetNextParticleLocal();
  G4double dtLocal, dELocal;
  slowext->GetDeltas(dtLocal, dELocal);
  pGauss.totalEnergy += dELocal;
  pGauss.T += dtLocal;
  return pGauss;
}

void BDSBunchGaussSlowExt::BeginOfRunAction(G4int numberOfEvents,
                                               G4bool batchMode)
{
  gauss->BeginOfRunAction(numberOfEvents, batchMode);
}
