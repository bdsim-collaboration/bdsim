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
#include "BDSOutputROOTEventLoss.hh"

#ifndef __ROOTBUILD__
#include "CLHEP/Units/SystemOfUnits.h"
#include "BDSHitEnergyDeposition.hh"
#include "BDSTrajectoryPoint.hh"
#include "BDSTrajectoryPointHit.hh"
#endif

ClassImp(BDSOutputROOTEventLoss)

BDSOutputROOTEventLoss::BDSOutputROOTEventLoss():
n(0)
#ifndef __ROOTBUILD__
,
  storeTurn(false),
  storeLinks(false),
  storeModelID(false),
  storeLocal(false),
  storeGlobal(false),
  storeTime(false),
  storeStepLength(false),
  storePhysicsProcesses(false)
#endif
{
  Flush();
}

#ifndef __ROOTBUILD__
BDSOutputROOTEventLoss::BDSOutputROOTEventLoss(bool storeTurnIn,
					       bool storeLinksIn,
					       bool storeModelIDIn,
					       bool storeLocalIn,
					       bool storeGlobalIn,
					       bool storeTimeIn,
					       bool storeStepLengthIn,
					       bool storePreStepKineticEnergyIn,
					       bool storePhysicsProcessesIn):
  n(0),
  storeTurn(storeTurnIn),
  storeLinks(storeLinksIn),
  storeModelID(storeModelIDIn),
  storeLocal(storeLocalIn),
  storeGlobal(storeGlobalIn),
  storeTime(storeTimeIn),
  storeStepLength(storeStepLengthIn),
  storePreStepKineticEnergy(storePreStepKineticEnergyIn),
  storePhysicsProcesses(storePhysicsProcessesIn)
{
  Flush();
}
#endif

BDSOutputROOTEventLoss::~BDSOutputROOTEventLoss()
{;}

#ifndef __ROOTBUILD__

void BDSOutputROOTEventLoss::Fill(const BDSTrajectoryPointHit* hit)
{
  n++;
  energy.push_back( (float) (hit->point->GetEnergyDeposit() / CLHEP::GeV));
  S.push_back     ( (float) (hit->point->GetPostS()  / CLHEP::m));
  weight.push_back( (float) hit->point->GetPostWeight());
  partID.push_back( (int)   hit->pdgID);
  trackID.push_back( (int)  hit->trackID);
  if (hit->parentID !=0) // general but don't fill for primaries as obvious -> reduce data size
    {parentID.push_back((int) hit->parentID);}
  modelID.push_back( (int)  hit->point->GetBeamLineIndex());
  
  if (storeTurn)
    {turn.push_back((int) hit->point->GetTurnsTaken());}
  
  if (storeLocal)
    {
      const G4ThreeVector& pos = hit->point->GetPostPosLocal();
      x.push_back( (float) (pos.x() / CLHEP::m));
      y.push_back( (float) (pos.y() / CLHEP::m));
      z.push_back( (float) (pos.z() / CLHEP::m));
    }
  
  if (storeGlobal)
    {
      const G4ThreeVector& pos = hit->point->GetPosition();
      X.push_back( (float) (pos.x() / CLHEP::m));
      Y.push_back( (float) (pos.y() / CLHEP::m));
      Z.push_back( (float) (pos.z() / CLHEP::m));
    }
  
  if (storeTime)
    {T.push_back( (float) (hit->point->GetPostGlobalTime() / CLHEP::ns));}

  // no step length for a point
  
  if (storePreStepKineticEnergy)
    {preStepKineticEnergy.push_back( (float) (hit->point->GetKineticEnergy() / CLHEP::GeV));}
  
  if (storePhysicsProcesses)
    {
      postStepProcessType.push_back( (int) (hit->point->GetPostProcessType()));
      postStepProcessSubType.push_back( (int) (hit->point->GetPostProcessSubType()));
    }
}

void BDSOutputROOTEventLoss::Fill(const BDSHitEnergyDeposition* hit)
{
  n++;
  energy.push_back( (float) (hit->GetEnergy() / CLHEP::GeV));
  S.push_back     ( (float) (hit->GetSHit()   / CLHEP::m));
  weight.push_back( (float) hit->GetWeight());

  if (storeTurn)
    {turn.push_back( hit->GetTurnsTaken());}

  if (storeLinks)
    {
      partID.push_back(hit->GetPartID());
      trackID.push_back(hit->GetTrackID());
      parentID.push_back(hit->GetParentID());   
    }
  
  if (storeModelID)
    {modelID.push_back(hit->GetBeamlineIndex());}

  subElementID.push_back(hit->GetSubElementID());

  if (storeLocal)
    {
      x.push_back( (float) (hit->Getx() / CLHEP::m));
      y.push_back( (float) (hit->Gety() / CLHEP::m));
      z.push_back( (float) (hit->Getz() / CLHEP::m));
    }
  
  if(storeGlobal)
    {
      X.push_back( (float) (hit->GetX() / CLHEP::m));
      Y.push_back( (float) (hit->GetY() / CLHEP::m));
      Z.push_back( (float) (hit->GetZ() / CLHEP::m));
    }

  if (storeTime)
    {T.push_back( (float) (hit->GetGlobalTime() / CLHEP::ns));}

  if (storeStepLength)
    {stepLength.push_back( (float) (hit->GetStepLength() / CLHEP::m));}
  
  if (storePreStepKineticEnergy)
    {preStepKineticEnergy.push_back( (float) (hit->GetPreStepKineticEnergy() / CLHEP::GeV));}

  if (storePhysicsProcesses)
    {
      postStepProcessType.push_back( (int) hit->GetPostStepProcessType());
      postStepProcessSubType.push_back( (int) hit->GetPostStepProcessSubType());
    }
}
#endif

void BDSOutputROOTEventLoss::Fill(const BDSOutputROOTEventLoss* other)
{
  if (!other)
    {return;}
  n        = other->n;
  energy   = other->energy;
  S        = other->S;
  weight   = other->weight;
  partID   = other->partID;
  trackID  = other->trackID;
  parentID = other->parentID;
  modelID  = other->modelID;
  turn     = other->turn;
  x = other->x;
  y = other->y;
  z = other->z;
  X = other->X;
  Y = other->Y;
  Z = other->Z;
  T = other->T;  
  stepLength           = other->stepLength;
  preStepKineticEnergy = other->preStepKineticEnergy;
  postStepProcessType  = other->postStepProcessType;
  postStepProcessSubType = other->postStepProcessSubType;
  subElementID           = other->subElementID;
}

void BDSOutputROOTEventLoss::Flush()
{FlushLocal();}

void BDSOutputROOTEventLoss::FlushLocal()
{
  n = 0;
  energy.clear();
  S.clear();
  weight.clear();
  partID.clear();
  trackID.clear();
  parentID.clear();
  modelID.clear();
  turn.clear();
  x.clear();
  y.clear();
  z.clear();
  X.clear();
  Y.clear();
  Z.clear();
  T.clear();
  stepLength.clear();
  preStepKineticEnergy.clear();
  postStepProcessType.clear();
  postStepProcessSubType.clear();
  subElementID.clear();
}
