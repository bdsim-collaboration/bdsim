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
#include "BDSOutputROOTEventSamplerC.hh"
#include "BDSOutputROOTParticleData.hh"

#include "TTree.h"

class BDSOutputROOTParticleData;

#ifndef __ROOTBUILD__
#include "BDSHitSamplerCylinder.hh"
#include "BDSParticleCoordsCylindrical.hh"
#include "BDSPhysicalConstants.hh"
#include "BDSPrimaryVertexInformationV.hh"

#include "globals.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include <cmath>
#endif

BDSOutputROOTParticleData* BDSOutputROOTEventSamplerC::particleTable = nullptr;

ClassImp(BDSOutputROOTEventSamplerC)

BDSOutputROOTEventSamplerC::BDSOutputROOTEventSamplerC():
  samplerName("samplercylinder")
{
  Flush();
}

BDSOutputROOTEventSamplerC::BDSOutputROOTEventSamplerC(const std::string& samplerNameIn):
  samplerName(samplerNameIn)
{
  Flush();
}

BDSOutputROOTEventSamplerC::~BDSOutputROOTEventSamplerC()
{;}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventSamplerC::Fill(const BDSHitSamplerCylinder* hit,
				      G4bool storeMass,
				      G4bool storeCharge,
				      G4bool storeElectrons,
				      G4bool storeRigidity,
				      G4bool storeKineticEnergy)
{
  n++;
  
  totalEnergy.push_back(static_cast<float>(hit->totalEnergy / CLHEP::GeV));
  z.push_back(static_cast<float>(hit->coords.z / CLHEP::m));
  phi.push_back(static_cast<float>(hit->coords.phi / CLHEP::radian));
  rp.push_back(static_cast<float>(hit->coords.rp));
  zp.push_back(static_cast<float>(hit->coords.zp));
  phip.push_back(static_cast<float>(hit->coords.phip));
  p.push_back(static_cast<float>(hit->momentum / CLHEP::GeV));
  T.push_back(static_cast<float>(hit->coords.T / CLHEP::ns));

  weight.push_back(static_cast<float>(hit->weight));
  partID.push_back(hit->pdgID);
  parentID.push_back(hit->parentID);
  trackID.push_back(hit->trackID);
  modelID = hit->beamlineIndex; 
  turnNumber.push_back(hit->turnsTaken);

  S = static_cast<float> (hit->S / CLHEP::m);

  if (storeMass)
    {mass.push_back(static_cast<float>(hit->mass / CLHEP::GeV));}

  if (storeCharge)
    {charge.push_back(static_cast<int>(hit->charge / (G4double)CLHEP::eplus));}

  if (storeKineticEnergy)
    {kineticEnergy.push_back(static_cast<float>((hit->totalEnergy - hit->mass) / CLHEP::GeV));}

  if (storeRigidity)
    {rigidity.push_back(static_cast<float>(hit->rigidity/(CLHEP::tesla*CLHEP::m)));}

  if (storeElectrons)
    {nElectrons.push_back(static_cast<int>(hit->nElectrons));}
}

#endif

void BDSOutputROOTEventSamplerC::Fill(const BDSOutputROOTEventSamplerC* other)
{
  if (!other)
    {return;}
  n      = other->n;
  totalEnergy = other->totalEnergy;
  z      = other->z;
  phi    = other->phi;
  rp     = other->rp;
  zp     = other->zp;
  phip   = other->phip;
  p      = other->p;
  T      = other->T;

  weight     = other->weight;
  partID     = other->partID;
  parentID   = other->parentID;
  trackID    = other->trackID;
  modelID    = other->modelID;
  turnNumber = other->turnNumber;
  S          = other->S;

  charge        = other->charge;
  kineticEnergy = other->kineticEnergy;
  mass          = other->mass;
  rigidity      = other->rigidity;

  isIon = other->isIon;
  ionA  = other->ionA;
  ionZ  = other->ionZ;
  nElectrons = other->nElectrons;
}

void BDSOutputROOTEventSamplerC::SetBranchAddress(TTree*)
{;}

void BDSOutputROOTEventSamplerC::Flush()
{
  n = 0;
  totalEnergy.clear();
  z.clear();
  phi.clear();
  rp.clear();
  zp.clear();
  phip.clear();
  p.clear();
  T.clear();
  weight.clear();
  partID.clear();
  parentID.clear();
  trackID.clear();
  modelID = -1;
  turnNumber.clear();

  S = 0.0;

  charge.clear();
  kineticEnergy.clear();
  mass.clear();
  rigidity.clear();
  isIon.clear();
  ionA.clear();
  ionZ.clear();
  nElectrons.clear();
}

std::vector<float> BDSOutputROOTEventSamplerC::getKineticEnergy()
{
  std::vector<float> result((unsigned long)n);
  if (!particleTable)
    {return result;}
  for (int i = 0; i < n; ++i)
    {result[i] = static_cast<float>(particleTable->KineticEnergy(partID[i], totalEnergy[i]));}
  return result;
}

std::vector<float> BDSOutputROOTEventSamplerC::getMass()
{
  std::vector<float> result((unsigned long)n);
  if (!particleTable)
    {return result;}
  for (int i = 0; i < n; ++i)
    {result[i] = static_cast<float>(particleTable->Mass(partID[i]));}
  return result;
}

std::vector<float> BDSOutputROOTEventSamplerC::getRigidity()
{
  std::vector<float> result((unsigned long)n);
  if (!particleTable)
    {return result;}
  for (int i = 0; i < n; ++i)
    {result[i] = static_cast<float>(particleTable->Rigidity(partID[i], totalEnergy[i]));}
  return result;
}

std::vector<bool> BDSOutputROOTEventSamplerC::getIsIon()
{
  bool useNElectrons = !nElectrons.empty();
  std::vector<bool> result((unsigned long)n);
  if (!particleTable)
    {return result;}
  for (int i = 0; i < n; ++i)
    {
      result[i] = particleTable->IsIon(partID[i]);
      if (useNElectrons)
        {result[i] = result[i] || nElectrons[i] > 0;}
    }
  return result;
}

std::vector<int> BDSOutputROOTEventSamplerC::getIonA()
{
  std::vector<int> result((unsigned long)n);
  if (!particleTable)
    {return result;}
  for (int i = 0; i < n; ++i)
    {result[i] = static_cast<int>(particleTable->IonA(partID[i]));}
  return result;
}

std::vector<int> BDSOutputROOTEventSamplerC::getIonZ()
{
  std::vector<int> result((unsigned long)n);
  if (!particleTable)
    {return result;}
  for (int i = 0; i < n; ++i)
    {result[i] = static_cast<int>(particleTable->IonZ(partID[i]));}
  return result;
}
