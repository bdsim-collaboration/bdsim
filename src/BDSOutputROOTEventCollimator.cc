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
#include "BDSOutputROOTEventCollimator.hh"
#include "BDSOutputROOTParticleData.hh"

#ifndef __ROOTBUILD__
#include "BDSHitCollimator.hh"
#include "BDSHitEnergyDeposition.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4TwoVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <utility>
#endif

#include <set>
#include <vector>

class BDSOutputROOTParticleData;

ClassImp(BDSOutputROOTEventCollimator)

BDSOutputROOTParticleData* BDSOutputROOTEventCollimator::particleTable = nullptr;

BDSOutputROOTEventCollimator::BDSOutputROOTEventCollimator():
  primaryInteracted(false),
  primaryStopped(false),
  n(0),
  totalEnergyDeposited(0)
{;}

BDSOutputROOTEventCollimator::~BDSOutputROOTEventCollimator()
{;}

void BDSOutputROOTEventCollimator::Flush()
{
  primaryInteracted = false;
  primaryStopped    = false;
  n                 = 0;
  energy.clear();
  energyDeposited.clear();
  xIn.clear();
  yIn.clear();
  zIn.clear();
  xpIn.clear();
  ypIn.clear();
  zpIn.clear();
  T.clear();
  weight.clear();
  partID.clear();
  parentID.clear();
  turn.clear();
  firstPrimaryHitThisTurn.clear();
  impactParameterX.clear();
  impactParameterY.clear();
  isIon.clear();
  ionZ.clear();
  ionA.clear();
  turnSet.clear();
  charge.clear();
  kineticEnergy.clear();
  mass.clear();
  rigidity.clear();
  totalEnergyDeposited = 0;
}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventCollimator::Fill(const BDSHitCollimator*                 hit,
					const BDSOutputROOTEventCollimatorInfo& info,
					const std::pair<G4double, G4double>&    differences,
					G4bool                                  storeHits)
{
  if (storeHits)
    {
      n++;
      energy.push_back((float) (hit->totalEnergy / CLHEP::GeV));
      const G4ThreeVector& pos = hit->preStepPosition;
      const G4ThreeVector& mom = hit->preStepMomentum;
      xIn.push_back((float) (pos.x() / CLHEP::m));
      yIn.push_back((float) (pos.y() / CLHEP::m));
      zIn.push_back((float) (pos.z() / CLHEP::m));
      xpIn.push_back((float) (mom.x() / CLHEP::rad));
      ypIn.push_back((float) (mom.y() / CLHEP::rad));
      zpIn.push_back((float) (mom.z() / CLHEP::rad));

      // calculate impact parameters - note done in output units (as is info object)
      G4double impactX = std::abs(xIn.back() - info.offsetX);
      G4double impactY = std::abs(yIn.back() - info.offsetY);
      G4double impactZ = zIn.back();

      // interpolate aperture to that point
      G4double zFromStart = -0.5 * info.length - impactZ;
      if (zFromStart < 0)
        {zFromStart = 0;} // sometimes rounding problems
      G4double fraction = zFromStart / info.length;
      G4double xAperAtZ = info.xSizeIn + differences.first * fraction;
      G4double yAperAtZ = info.ySizeIn + differences.second * fraction;

      // impact parameter is absolute
      impactX = impactX - xAperAtZ;
      impactY = impactY - yAperAtZ;

      if (BDS::IsFinite(info.tilt))
        {
          G4TwoVector impactPos(impactX, impactY);
          impactPos.rotate(info.tilt);
          impactX = impactPos.x();
          impactY = impactPos.y();
        }
      impactParameterX.push_back((float) impactX);
      impactParameterY.push_back((float) impactY);
    }
  
  BDSHitEnergyDeposition* eHit = hit->energyDepositionHit;
  if (eHit)
    {
      G4double eDep = eHit->GetEnergy() / CLHEP::GeV;
      G4double eW   = eHit->GetEnergyWeighted() / CLHEP::GeV;
      G4double w    = eHit->GetWeight();
      
      totalEnergyDeposited += eW;
      
      primaryInteracted = primaryInteracted || eHit->GetParentID() == 0;

      if (!storeHits)
        {return;} // skip the rest of this function to avoid storing extra bits
      
      energyDeposited.push_back((float)eDep);
      T.push_back((float)(eHit->GetGlobalTime() / CLHEP::ns));
      weight.push_back((float)w);
      partID.push_back(eHit->GetPartID());
      parentID.push_back(eHit->GetParentID());
      G4int tn = eHit->GetTurnsTaken();
      turn.push_back(tn);
      // Mark if this is the first primary impact of this turn as there may
      // be multiple 'hits' inside the collimator for a primary passing through
      // but we're really interested in the first one. Judge by whether we have
      // an hits for this turn already. We assume (safely) that the first entry
      // for a given turn will be the primary.
      if (parentID.back() == 0)
	{firstPrimaryHitThisTurn.push_back(turnSet.find(tn) == turnSet.end());}
      else
	{firstPrimaryHitThisTurn.push_back(false);} // can't be true
      turnSet.insert(tn);
    }
}

void BDSOutputROOTEventCollimator::FillExtras(G4bool fillIonInfo,
					      G4bool fillLinks)
{
  if (!particleTable)
    {return;}

  if (!(fillIonInfo || fillLinks))
    {return;}
  
  for (int i = 0; i < n; ++i)
    {// loop over all existing entries in the branch vectors
      auto& pid = partID[i];
      if (particleTable->IsIon(pid))
        {
          auto& ionInfo = particleTable->GetIonInfo(pid);
	  if (fillIonInfo)
	    {
	      isIon.push_back(true);
	      ionA.push_back(ionInfo.a);
	      ionZ.push_back(ionInfo.z);
	    }
	  if (fillLinks)
	    {
	      charge.push_back(ionInfo.charge);
	      mass.push_back((float)ionInfo.mass);
	      rigidity.push_back((float)ionInfo.rigidity(energy[i], ionInfo.charge));
	      kineticEnergy.push_back((float)particleTable->KineticEnergy(pid, energy[i]));
	    }
        }
      else
        {// particle
	  auto& pInfo = particleTable->GetParticleInfo(pid);
	  if (fillIonInfo)
	    {
	      isIon.push_back(false);
	      ionA.push_back(0);
	      ionZ.push_back(0);
	    }
	  if (fillLinks)
	    {
	      charge.push_back(pInfo.charge);
	      mass.push_back((float)pInfo.mass);
	      rigidity.push_back((float)pInfo.rigidity(energy[i], pInfo.charge));
	      kineticEnergy.push_back((float)particleTable->KineticEnergy(pid, energy[i]));
	    }
        }
    }
}

#endif

void BDSOutputROOTEventCollimator::Fill(const BDSOutputROOTEventCollimator* other)
{
  if (!other)
    {return;}
  primaryInteracted = other->primaryInteracted;
  primaryStopped    = other->primaryStopped;
  n                 = other->n;
  energy            = other->energy;
  energyDeposited   = other->energyDeposited;

  xIn  = other->xIn;
  yIn  = other->yIn;
  zIn  = other->zIn;
  xpIn = other->xpIn;
  ypIn = other->ypIn;
  zpIn = other->zpIn;
  T    = other->T;
  
  weight   = other->weight;
  partID   = other->partID;
  parentID = other->parentID;
  turn     = other->turn;

  firstPrimaryHitThisTurn = other->firstPrimaryHitThisTurn;
  impactParameterX        = other->impactParameterX;
  impactParameterY        = other->impactParameterY;

  isIon   = other->isIon;
  ionA    = other->ionA;
  ionZ    = other->ionZ;
  turnSet = other->turnSet;
  charge  = other->charge;

  kineticEnergy = other->kineticEnergy;
  mass          = other->mass;
  rigidity      = other->rigidity;
}
