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
#include "BDSBunch.hh"
#include "BDSBunchCompositeSDE.hh"
#include "BDSBunchFactory.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSParticleCoordsFull.hh"

#include "parser/beam.h"

BDSBunchCompositeSDE::BDSBunchCompositeSDE():
BDSBunch("compositespacedirectionenergy"),
  sBunch(nullptr),
  dBunch(nullptr),
  eBunch(nullptr)
{;}

BDSBunchCompositeSDE::~BDSBunchCompositeSDE()
{
  delete sBunch;
  delete dBunch;
  delete eBunch;
}

void BDSBunchCompositeSDE::SetOptions(const BDSParticleDefinition* beamParticle,
                                     const GMAD::Beam& beam,
                                     const BDSBunchType& distrType,
                                     G4Transform3D beamlineTransformIn,
                                     G4double beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn,beamlineSIn);
  
  delete sBunch;
  delete dBunch;
  delete eBunch;
  BDSBunchType sType;
  BDSBunchType dType;
  BDSBunchType eType;
  try
    {sType = BDS::DetermineBunchType(beam.spaceDistrType);}
  catch (BDSException& e)
    {e.AppendToMessage("Error with space dimension in composite beam distribution"); throw e;}
  try
    {dType = BDS::DetermineBunchType(beam.directionDistrType);}
  catch (BDSException& e)
    {e.AppendToMessage("Error with direction dimension in composite beam distribution"); throw e;}
  try
    {eType = BDS::DetermineBunchType(beam.energyDistrType);}
  catch (BDSException& e)
    {e.AppendToMessage("Error with energy dimension in composite beam distribution"); throw e;}

  std::vector<BDSBunchType> dims = {sType, dType, eType};
  for (auto d : dims)
    {
      if (d == BDSBunchType::composite || d == BDSBunchType::compositesde)
        {throw BDSException(__METHOD_NAME__, "s,y,z distributions cannot be 'composite'");}
    }

  // here we don't have generatePrimariesOnly bool but this will be overridden with the
  // separate call to SetGeneratePrimariesOnly in BDSBunchFactory
  sBunch = BDSBunchFactory::CreateBunch(beamParticle, sType, beam, beamlineTransformIn);
  dBunch = BDSBunchFactory::CreateBunch(beamParticle, dType, beam, beamlineTransformIn);
  eBunch = BDSBunchFactory::CreateBunch(beamParticle, eType, beam, beamlineTransformIn);
  
  name = "composite: space: " + sBunch->Name() + ", direction: " + dBunch->Name() + ", energy: " + eBunch->Name();
}

void BDSBunchCompositeSDE::SetGeneratePrimariesOnly(G4bool generatePrimariesOnlyIn)
{
  BDSBunch::SetGeneratePrimariesOnly(generatePrimariesOnlyIn);
  sBunch->SetGeneratePrimariesOnly(generatePrimariesOnlyIn);
  dBunch->SetGeneratePrimariesOnly(generatePrimariesOnlyIn);
  eBunch->SetGeneratePrimariesOnly(generatePrimariesOnlyIn);
}

void BDSBunchCompositeSDE::CheckParameters()
{
  sBunch->CheckParameters();
  dBunch->CheckParameters();
  eBunch->CheckParameters();
}

BDSParticleCoordsFull BDSBunchCompositeSDE::GetNextParticleLocal()
{
  auto s = sBunch->GetNextParticleLocal();
  auto d = dBunch->GetNextParticleLocal();
  auto e = eBunch->GetNextParticleLocal();

  particleDefinitionHasBeenUpdated = sBunch->ParticleDefinitionHasBeenUpdated() ||
                                     dBunch->ParticleDefinitionHasBeenUpdated() ||
                                     eBunch->ParticleDefinitionHasBeenUpdated();

  // TODO - the weight only comes from the e distribution here... should it be product of all?
  BDSParticleCoordsFull result(s.x,  s.y,  s.z,
                               d.xp, d.yp, d.zp,
                               e.T, s.s,
                               e.totalEnergy,
                               s.weight * d.weight * e.weight);
  return result;
}
