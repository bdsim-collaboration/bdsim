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
#include "BDSOutputROOTEventLossWorld.hh"

#ifndef __ROOTBUILD__
#include "CLHEP/Units/SystemOfUnits.h"
#include "BDSHitEnergyDepositionGlobal.hh"
#endif

ClassImp(BDSOutputROOTEventLossWorld)

BDSOutputROOTEventLossWorld::BDSOutputROOTEventLossWorld()
{
  Flush();
}

BDSOutputROOTEventLossWorld::~BDSOutputROOTEventLossWorld()
{;}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventLossWorld::Fill(const BDSHitEnergyDepositionGlobal* hit)
{
  n++;
  totalEnergy.push_back( (float) (hit->totalEnergy / CLHEP::GeV));
  preStepKineticEnergy.push_back( (float) (hit->preStepKineticEnergy / CLHEP::GeV));
  postStepKineticEnergy.push_back( (float) (hit->postStepKineticEnergy / CLHEP::GeV));
  stepLength.push_back( (float) (hit->stepLength / CLHEP::m));
  X.push_back( (float) (hit->X / CLHEP::m));
  Y.push_back( (float) (hit->Y / CLHEP::m));
  Z.push_back( (float) (hit->Z / CLHEP::m));
  T.push_back( (float) (hit->T / CLHEP::ns));
  partID.push_back(hit->pdgID);
  trackID.push_back(hit->trackID);
  parentID.push_back(hit->parentID);
  weight.push_back((float)hit->weight);
  turn.push_back(hit->turnsTaken);
}
#endif

void BDSOutputROOTEventLossWorld::Fill(const BDSOutputROOTEventLossWorld* other)
{
  if (!other)
    {return;}

  n = other->n;
  totalEnergy           = other->totalEnergy;
  preStepKineticEnergy  = other->preStepKineticEnergy;
  postStepKineticEnergy = other->postStepKineticEnergy;
  stepLength            = other->stepLength;
  X = other->X;
  Y = other->Y;
  Z = other->Z;
  T = other->T;
  partID   = other->partID;
  trackID  = other->trackID;
  parentID = other->parentID;
  weight   = other->weight;
  turn     = other->turn;
}

void BDSOutputROOTEventLossWorld::Flush()
{
  n = 0;
  totalEnergy.clear();
  preStepKineticEnergy.clear();
  postStepKineticEnergy.clear();
  stepLength.clear();
  X.clear();
  Y.clear();
  Z.clear();
  T.clear();
  partID.clear();  
  trackID.clear();
  parentID.clear();
  weight.clear();
  turn.clear();
}
