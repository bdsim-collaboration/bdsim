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
#include "BDSBunchComposite.hh"
#include "BDSBunchFactory.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSParticleCoordsFull.hh"

#include "parser/beam.h"

BDSBunchComposite::BDSBunchComposite():
BDSBunch("composite"),
  xBunch(nullptr),
  yBunch(nullptr),
  zBunch(nullptr)
{;}

BDSBunchComposite::~BDSBunchComposite()
{
  delete xBunch;
  delete yBunch;
  delete zBunch;
}

void BDSBunchComposite::SetOptions(const BDSParticleDefinition* beamParticle,
                                   const GMAD::Beam& beam,
                                   const BDSBunchType& distrType,
                                   G4Transform3D beamlineTransformIn,
                                   G4double beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn,beamlineSIn);
  
  delete xBunch;
  delete yBunch;
  delete zBunch;
  BDSBunchType xType;
  BDSBunchType yType;
  BDSBunchType zType;
  try
    {xType = BDS::DetermineBunchType(beam.xDistrType);}
  catch (BDSException& e)
    {e.AppendToMessage("Error with x dimension composite beam distribution"); throw e;}
  try
    {yType = BDS::DetermineBunchType(beam.yDistrType);}
  catch (BDSException& e)
    {e.AppendToMessage("Error with y dimension composite beam distribution"); throw e;}
  try
    {zType = BDS::DetermineBunchType(beam.zDistrType);}
  catch (BDSException& e)
    {e.AppendToMessage("Error with z dimension composite beam distribution"); throw e;}

  if (xType == BDSBunchType::composite ||
      yType == BDSBunchType::composite ||
      zType == BDSBunchType::composite)
    {throw BDSException(__METHOD_NAME__, "x,y,z distributions cannot be 'composite'");}

  // here we don't have generatePrimariesOnly bool but this will be overridden with the
  // separate call to SetGeneratePrimariesOnly in BDSBunchFactory
  xBunch = BDSBunchFactory::CreateBunch(beamParticle, xType, beam, beamlineTransformIn);
  yBunch = BDSBunchFactory::CreateBunch(beamParticle, yType, beam, beamlineTransformIn);
  zBunch = BDSBunchFactory::CreateBunch(beamParticle, zType, beam, beamlineTransformIn);
  
  name = "composite: x: " + xBunch->Name() + ", y: " + yBunch->Name() + ", z: " + zBunch->Name();
}

void BDSBunchComposite::SetGeneratePrimariesOnly(G4bool generatePrimariesOnlyIn)
{
  BDSBunch::SetGeneratePrimariesOnly(generatePrimariesOnlyIn);
  xBunch->SetGeneratePrimariesOnly(generatePrimariesOnlyIn);
  yBunch->SetGeneratePrimariesOnly(generatePrimariesOnlyIn);
  zBunch->SetGeneratePrimariesOnly(generatePrimariesOnlyIn);
}

void BDSBunchComposite::CheckParameters()
{
  xBunch->CheckParameters();
  yBunch->CheckParameters();
  zBunch->CheckParameters();
}

BDSParticleCoordsFull BDSBunchComposite::GetNextParticleLocal()
{
  auto x = xBunch->GetNextParticleLocal();
  auto y = yBunch->GetNextParticleLocal();
  auto z = zBunch->GetNextParticleLocal();

  particleDefinitionHasBeenUpdated = xBunch->ParticleDefinitionHasBeenUpdated() ||
                                     yBunch->ParticleDefinitionHasBeenUpdated() ||
                                     zBunch->ParticleDefinitionHasBeenUpdated();

  BDSParticleCoordsFull result(x.x, y.y, z.z,
                               x.xp, y.yp, z.zp,
                               z.T, z.s,
                               z.totalEnergy,
                               x.weight * y.weight * z.weight);
  return result;
}
