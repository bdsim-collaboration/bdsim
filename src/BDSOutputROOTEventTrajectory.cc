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
#include <algorithm>
#include <bitset>
#include <iostream>
#include <iomanip>

#include "BDSOutputROOTEventTrajectory.hh"

#ifndef __ROOTBUILD__
#include "G4VPhysicalVolume.hh"

#include "BDSHitEnergyDeposition.hh"
#include "BDSAuxiliaryNavigator.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSTrajectory.hh"
#include "BDSTrajectoryOptions.hh"

#include <cmath>
#include <map>
#endif

ClassImp(BDSOutputROOTEventTrajectory)
BDSOutputROOTEventTrajectory::BDSOutputROOTEventTrajectory():
  auxNavigator(nullptr),
  n(0)
{
  FlushLocal();
}

BDSOutputROOTEventTrajectory::~BDSOutputROOTEventTrajectory()
{
#ifndef __ROOTBUILD__
  delete auxNavigator;
#endif
}

#ifndef __ROOTBUILD__
int findPrimaryStepIndex(BDSTrajectory* traj)
{
  if (!traj->GetParent())
    {return -1;}

  if (traj->GetParent()->GetTrackID() == 1)
    {return traj->GetParentStepIndex();}
  else
    {return findPrimaryStepIndex(traj->GetParent());}
}

void BDSOutputROOTEventTrajectory::Fill(const BDSTrajectoriesToStore* trajectories,
                                        int  storeStepPointsN,
                                        bool storeStepPointLast,
                                        const BDS::TrajectoryOptions& storageOptions,
                                        const std::map<G4Material*, short int>& materialToID)
{
  if (!auxNavigator)
    {// navigator for converting coordinates to curvilinear coordinate system
      auxNavigator = new BDSAuxiliaryNavigator();
    }

  G4bool stEK = storageOptions.storeLinks || storageOptions.storeKineticEnergy;
  G4bool stMo = storageOptions.storeMomentumVector;
  G4bool stPr = storageOptions.storeProcesses;
  G4bool stTi = storageOptions.storeTime;
  G4bool stMa = storageOptions.storeMaterial;
  
  // assign trajectory indices
  int idx = 0;
  for (auto trajFlag : trajectories->trajectories)
    {
      BDSTrajectory* traj = trajFlag.first;
      if (trajFlag.second) // ie we want to save this trajectory
        {
          traj->SetTrajIndex(idx);
          idx++;
        }
      else // we don't want to save it
        {traj->SetTrajIndex(-1);}
    }

  // assign parent (and step) indices
  for (auto trajFlag : trajectories->trajectories)
    {
      BDSTrajectory* traj   = trajFlag.first;
      BDSTrajectory* parent = traj->GetParent();
      if (trajFlag.second && parent)
        { // to store and not primary
          traj->SetParentIndex(parent->GetTrajIndex());

          // search for parent step index
          if (parent->GetTrajIndex() != -1)
            {
              auto trajStartPos = traj->GetPoint(0)->GetPosition();
              traj->SetParentStepIndex(-1);
              for (int i = 0; i < parent->GetPointEntries(); ++i)
                {
                  if(parent->GetPoint(i)->GetPosition() == trajStartPos)
                    {
                      traj->SetParentStepIndex(i);
                      break;
                    }
                }
            }
          else
            {parent->SetParentStepIndex(-1);}
        }
      else
        {traj->SetParentIndex(-1);}
    }

  n = 0;
  for (auto trajFlag : trajectories->trajectories)
    {
      BDSTrajectory* traj = trajFlag.first;

      // check if the trajectory is to be stored
      if (!trajFlag.second) // ie false, then continue and don't store
        {continue;}

      partID.push_back((int) traj->GetPDGEncoding());
      trackID.push_back((unsigned int) std::abs(traj->GetTrackID()));
      parentID.push_back((unsigned int) std::abs(traj->GetParentID()));
      parentIndex.push_back((unsigned int) std::abs(traj->GetParentIndex()));
      parentStepIndex.push_back((unsigned int) std::abs(traj->GetParentStepIndex()));
      depth.push_back((int) traj->GetDepth());

      // now we convert the geant4 type based BDSTrajectory information into
      // basic C++ and ROOT types for the output
      IndividualTrajectory itj;
      if (storeStepPointsN > 0)
        {// store specific number of step points along the trajectory
          G4int nSteps = traj->GetPointEntries();
          G4int nPoints = std::min(nSteps, storeStepPointsN);
          for (int i = 0; i < nPoints; ++i)
            {FillIndividualTrajectory(itj, traj, i, materialToID);}
          // optionally include the last point if required and not already stored
          if (storeStepPointLast && (nPoints < nSteps))
            {FillIndividualTrajectory(itj, traj, nSteps-1, materialToID);}
        }
      else
        {// store all points as usual
          for (int i = 0; i < traj->GetPointEntries(); ++i)
            {FillIndividualTrajectory(itj, traj, i, materialToID);}
        }
      
      // record the filters that were matched for this trajectory
      filters.push_back(trajectories->filtersMatched.at(traj));
      
      XYZ.push_back(itj.XYZ);
      modelIndicies.push_back(itj.modelIndex);
      
      if (stMo)
        {PXPYPZ.push_back(itj.PXPYPZ);}
      
      S.push_back(itj.S);
      
      if (stPr)
        {
          preProcessTypes.push_back(itj.preProcessType);
          preProcessSubTypes.push_back(itj.preProcessSubType);
          postProcessTypes.push_back(itj.postProcessType);
          postProcessSubTypes.push_back(itj.postProcessSubType);
        }
      
      preWeights.push_back(itj.preWeight);
      postWeights.push_back(itj.postWeight);
      energyDeposit.push_back(itj.energyDeposit);
      
      if (stTi)
        {T.push_back(itj.T);}
      
      if (stEK)
        {kineticEnergy.push_back(itj.kineticEnergy);}

      if (stMa)
        {materialID.push_back(itj.materialID);}

      if (!itj.xyz.empty())
        {
          xyz.push_back(itj.xyz);
          pxpypz.push_back(itj.pxpypz);
        }

      if (!itj.charge.empty())
        {
          charge.push_back(itj.charge);
          turnsTaken.push_back(itj.turn);
          mass.push_back(itj.mass);
          rigidity.push_back(itj.rigidity);
        }
      
      if (!itj.isIon.empty())
        {
          isIon.push_back(itj.isIon);
          ionA.push_back(itj.ionA);
          ionZ.push_back(itj.ionZ);
          nElectrons.push_back(itj.nElectrons);
        }
      
      // recursively search for primary interaction step
      primaryStepIndex.push_back(findPrimaryStepIndex(traj));

      // geant4 trackID to trackIndex in this table
      trackID_trackIndex.insert(std::pair<int,int>(traj->GetTrackID(),n));
      
      n++;
    }
  
#if 0
  // Fill maps for later analysis
  int trackIndex = 0;
  for (auto iT = trajVec.begin(); iT != trajVec.end(); ++iT)
    {
      BDSTrajectory* traj = *iT;

      // map of trackID to trackIndex
      trackID_trackIndex.insert(std::pair<int, int>(traj->GetTrackID(),trackIndex));

      std::cout << trajVec.size() << " " << parentID.size() << " " << parentIndex.size() << " "
                << traj->GetTrackID() << " " << traj->GetParentID() << " " << trackIndex << std::endl;

      // map of trackIndex to trackProcess
      auto processPair = findParentProcess(trackIndex);
      trackIndex_trackProcess.insert(std::pair<int,std::pair<int,int>>(trackIndex,processPair));

      // map of modelIndex to trackProcess
      if(processPair.first != -1)
        {
          int mi = modelIndicies[processPair.first][processPair.second];
          trackIndex_modelIndex.insert(std::pair<int,int>(trackIndex, mi));
          try
            {modelIndex_trackIndex.at(mi).push_back(trackIndex);}
          catch(const std::exception&)
            {
              modelIndex_trackIndex.insert(std::pair<int,std::vector<int>>(mi,std::vector<int>()));
              modelIndex_trackIndex.at(mi).push_back(trackIndex);
            }
        }

      ++trackIndex;
    }
#endif
}

void BDSOutputROOTEventTrajectory::FillIndividualTrajectory(IndividualTrajectory& itj,
                                                            BDSTrajectory*        traj,
                                                            int                   i,
                                                            const std::map<G4Material*, short int>& materialToID) const
{
  BDSTrajectoryPoint* point = dynamic_cast<BDSTrajectoryPoint*>(traj->GetPoint(i));
  if (!point)
    {return;}
  
  // Position
  G4ThreeVector pos = point->GetPosition();
  itj.XYZ.emplace_back(pos.getX() / CLHEP::m, pos.getY() / CLHEP::m, pos.getZ() / CLHEP::m);
  
  G4VPhysicalVolume* vol = auxNavigator->LocateGlobalPointAndSetup(pos,nullptr,true,true,true);
  BDSPhysicalVolumeInfo* theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(vol);
  if (theInfo)
    {itj.modelIndex.push_back(theInfo->GetBeamlineIndex());}
  else
    {itj.modelIndex.push_back(-1);}
  
  // Process types
  itj.preProcessType.push_back(point->GetPreProcessType());
  itj.preProcessSubType.push_back(point->GetPreProcessSubType());
  itj.postProcessType.push_back(point->GetPostProcessType());
  itj.postProcessSubType.push_back(point->GetPostProcessSubType());
  
  itj.preWeight.push_back(point->GetPreWeight());
  itj.postWeight.push_back(point->GetPostWeight());
  itj.energyDeposit.push_back(point->GetEnergyDeposit() / CLHEP::GeV);
  G4ThreeVector mom = point->GetPreMomentum() / CLHEP::GeV;
  itj.PXPYPZ.emplace_back(mom.getX(), mom.getY(), mom.getZ());
  itj.S.push_back(point->GetPreS() / CLHEP::m);
  itj.T.push_back(point->GetPreGlobalTime() / CLHEP::ns);
  itj.kineticEnergy.push_back(point->GetKineticEnergy() / CLHEP::GeV);
  
  itj.materialID.push_back(materialToID.at(point->GetMaterial()));
  
  if (point->extraLocal)
    {
      G4ThreeVector localPos = point->GetPositionLocal() / CLHEP::m;
      G4ThreeVector localMom = point->GetMomentumLocal() / CLHEP::GeV;
      itj.xyz.emplace_back(localPos.getX(), localPos.getY(), localPos.getZ());
      itj.pxpypz.emplace_back(localMom.getX(), localMom.getY(), localMom.getZ());
    }
  
  if (point->extraLink)
    {
      itj.charge.push_back((int) (point->GetCharge() / (G4double)CLHEP::eplus));
      itj.turn.push_back(point->GetTurnsTaken());
      itj.mass.push_back(point->GetMass() / CLHEP::GeV);
      itj.rigidity.push_back(point->GetRigidity() / (CLHEP::tesla*CLHEP::m));
    }
  
  if (point->extraIon)
    {
      itj.isIon.push_back(point->GetIsIon());
      itj.ionA.push_back(point->GetIonA());
      itj.ionZ.push_back(point->GetIonZ());
      itj.nElectrons.push_back(point->GetNElectrons());
    }     
}
  
void BDSOutputROOTEventTrajectory::Fill(const BDSHitsCollectionEnergyDeposition* phc)
{
  G4cout << phc->GetSize() << G4endl;
}

#endif

void BDSOutputROOTEventTrajectory::Flush()
{
  FlushLocal();
}

void BDSOutputROOTEventTrajectory::FlushLocal()
{
  n = 0;
  filters.clear();
  partID.clear();
  trackID.clear();
  parentID.clear();
  parentIndex.clear();
  parentStepIndex.clear();
  primaryStepIndex.clear();
  depth.clear();
  
  preProcessTypes.clear();
  preProcessSubTypes.clear();
  postProcessTypes.clear();
  postProcessSubTypes.clear();
  
  preWeights.clear();
  postWeights.clear();
  energyDeposit.clear();
  
  XYZ.clear();
  S.clear();
  PXPYPZ.clear();
  T.clear();
  
  xyz.clear();
  pxpypz.clear();

  charge.clear();
  kineticEnergy.clear();
  turnsTaken.clear();
  mass.clear();
  rigidity.clear();

  isIon.clear();
  ionA.clear();
  ionZ.clear();
  nElectrons.clear();

  materialID.clear();
  modelIndicies.clear();
  trackID_trackIndex.clear();

  // trackIndex_trackProcess.clear();
  // trackIndex_modelIndex.clear();
  // modelIndex_trackIndex.clear();
}

void BDSOutputROOTEventTrajectory::Fill(const BDSOutputROOTEventTrajectory* other)
{
  if (!other)
    {return;}

  n = 0;
  filters             = other->filters;
  partID              = other->partID;
  trackID             = other->trackID;
  parentID            = other->parentID;
  parentIndex         = other->parentIndex;
  parentStepIndex     = other->parentStepIndex;
  primaryStepIndex    = other->primaryStepIndex;
  depth               = other->depth;
  
  preProcessTypes     = other->preProcessTypes;
  preProcessSubTypes  = other->preProcessSubTypes;
  postProcessTypes    = other->postProcessTypes;
  postProcessSubTypes = other->postProcessSubTypes;

  preWeights          = other->preWeights;
  postWeights         = other->postWeights;
  energyDeposit       = other->energyDeposit;

  XYZ                 = other->XYZ;
  S                   = other->S;
  PXPYPZ              = other->PXPYPZ;
  T                   = other->T;

  xyz                 = other->xyz;
  pxpypz              = other->pxpypz;
  charge              = other->charge;
  kineticEnergy       = other->kineticEnergy;
  turnsTaken          = other->turnsTaken;
  mass                = other->mass;
  rigidity            = other->rigidity;
  
  isIon               = other->isIon;
  ionA                = other->ionA;
  ionZ                = other->ionZ;
  nElectrons          = other->nElectrons;

  materialID          = other->materialID;
  modelIndicies       = other->modelIndicies;
  trackID_trackIndex  = other->trackID_trackIndex;

}

#if 0
std::pair<int,int> BDSOutputROOTEventTrajectory::findParentProcess(int trackIndex)
{
  std::cout << "BDSOutputROOTEventTrajectory::findParentProcess> "
            << trackIndex << " " << parentID.size() << " " << parentIndex.size() << std::endl;
  int tid = trackIndex;
  int pid = parentID.at(tid);
  std::cout << pid << std::endl;
  int pin = parentIndex.at(tid);
  std::cout << pin << std::endl;

  if (pin == -1)
    {return std::pair<int,int>(-1,-1);}
  int sin = parentStepIndex.at(tid);
  std::cout << sin << std::endl;

  while (pid > 0)
    {
      if(pin == 0)
        {break;}
      tid = pin;
      pid = parentID.at(tid);
      pin = parentIndex.at(tid);
      sin = parentStepIndex.at(tid);

      std::cout << tid << " " << pid << " " << pin << " " << sin << " " << std::endl;
    }

  return std::pair<int,int>(pin,sin);
}
#endif

std::vector<BDSOutputROOTEventTrajectoryPoint> BDSOutputROOTEventTrajectory::trackInteractions(int trackIDIn) const
{
  // prevent a bad access
  if (trackID_trackIndex.find(trackIDIn) == trackID_trackIndex.end())
    {
      std::cout << "No such track ID" << std::endl;
      return std::vector<BDSOutputROOTEventTrajectoryPoint>();
    }
  
  int ti = trackID_trackIndex.at(trackIDIn);  // get track index

  std::vector<BDSOutputROOTEventTrajectoryPoint> tpv; // trajectory point vector - result

  int nstep = (int)XYZ[ti].size();
  if (nstep == 0) // no points or it wasn't stored
    {return std::vector<BDSOutputROOTEventTrajectoryPoint>();}

  if (postProcessTypes[ti].empty()) // and implicitly nstep>0
    {// we require processes for this function
      std::cout << "Processes not stored for this file - not possible." << std::endl;
      return std::vector<BDSOutputROOTEventTrajectoryPoint>();
    }

  bool usePXPYPZ = !PXPYPZ[ti].empty();
  bool useT      = !T.empty();
  bool useIon    = !isIon.empty();
  bool useLocal  = !xyz.empty();
  bool useLinks  = !charge.empty();
  bool useEK     = !kineticEnergy.empty();
  bool useMat    = !materialID.empty();
  
  for (int i = 0; i < nstep; ++i)
    {
      int ppt  = postProcessTypes[ti][i];
      int ppst = postProcessSubTypes[ti][i];
      // this is a hard coded version of BDSTrajectoryPoint::IsScatteringPoint which is
      // only available if we link to Geant4 for the enums (we don't here).
      // -1 = undefined, 1 = G4ProcessType::fTransportation, 10 = G4ProcessTypes::fParallel
      // 0 = G4ProcessType::fNotDefined for crystal channeling (exclude the thousands of points)
      // 401 = G4TransportationProcessSubType::STEP_LIMITER which is categorised under G4ProcessType::fGeneral
      bool notGeneral = ppt != 7 && ppst != 401;
      bool changeInEnergy = energyDeposit[ti][i] > 1e-9;
      if ( (ppt != -1 && ppt != 1 && ppt != 10 && ppt != 0 && notGeneral) || changeInEnergy)
        {
          BDSOutputROOTEventTrajectoryPoint p(partID[ti],
                                              trackID[ti],
                                              parentID[ti],
                                              parentIndex[ti],
                                              postProcessTypes[ti][i],
                                              postProcessSubTypes[ti][i],
                                              postWeights[ti][i],
                                              energyDeposit[ti][i],
                                              XYZ[ti][i],
                                              usePXPYPZ ? PXPYPZ[ti][i] : TVector3(),
                                              modelIndicies[ti][i],
                                              useT ? T[ti][i]      : 0,
                                              useLocal ? xyz[ti][i] : TVector3(),
                                              useLocal ? pxpypz[ti][i] : TVector3(),
                                              useLinks ? charge[ti][i] : 0,
                                              useEK ? kineticEnergy[ti][i] : 0,
                                              useLinks ? turnsTaken[ti][i] : 0,
                                              useLinks ? rigidity[ti][i] : 0,
                                              useLinks ? mass[ti][i] : 0,
                                              useIon ? isIon[ti][i] : false,
                                              useIon ? ionA[ti][i] : 0,
                                              useIon ? ionZ[ti][i] : 0,
                                              useIon ? nElectrons[ti][i] : 0,
                                              useMat ? materialID[ti][i] : -1,
                                              i);
          tpv.push_back(p);
        }
    }
  return tpv;
}

BDSOutputROOTEventTrajectoryPoint BDSOutputROOTEventTrajectory::primaryProcessPoint(int trackIDIn) const
{
  // prevent a bad access
  if (trackID_trackIndex.find(trackIDIn) == trackID_trackIndex.end())
    {
      std::cout << "No such track ID" << std::endl;
      return BDSOutputROOTEventTrajectoryPoint();
    }
  int ti = trackID_trackIndex.at(trackIDIn);  // get track index
  int pid = (int)parentID[ti];                   // parent trackID
  int chosenTrackID = trackIDIn;
  while (pid != 0)
    {
      if (parentID[trackID_trackIndex.at(pid)] > 0)
        {chosenTrackID = pid;}
      ti = trackID_trackIndex.at(pid);
      pid = (int)parentID[ti];
    }
  return parentProcessPoint(chosenTrackID);
}

BDSOutputROOTEventTrajectoryPoint BDSOutputROOTEventTrajectory::parentProcessPoint(int trackIDIn) const
{
  // prevent a bad access
  if (trackID_trackIndex.find(trackIDIn) == trackID_trackIndex.end())
    {
      std::cout << "No such track ID" << std::endl;
      return BDSOutputROOTEventTrajectoryPoint();
    }
  
  int ti  = trackID_trackIndex.at(trackIDIn);  // get track index
  int pti = (int)parentID[ti]; // parent trackID
  
  if (pti == 0)
    {
      std::cout << "Track is a parent" << std::endl;
      return BDSOutputROOTEventTrajectoryPoint();
    }
  
  int si  = (int)parentStepIndex.at(ti);          // get primary index
  int pi = 0;
  if (pti > 0)
    {pi  = trackID_trackIndex.at(pti);}      // parent track storage index
  else
    {pi = ti;}
  
  if (si > (int)XYZ[pi].size())
    {// evidently not all step points are stored
      std::cout << "Not all step points are stored. Parent step index is outside points stored." << std::endl;
      return BDSOutputROOTEventTrajectoryPoint();
    }

  bool usePXPYPZ = !PXPYPZ[ti].empty();
  bool useT      = !T.empty();
  bool useIon    = !isIon.empty();
  bool useLocal  = !xyz.empty();
  bool useLinks  = !charge.empty();
  bool useEK     = !kineticEnergy.empty();
  bool useMat    = !materialID.empty();

  BDSOutputROOTEventTrajectoryPoint p(partID[pi],
                                      trackID[pi],
                                      parentID[pi],
                                      parentIndex[pi],
                                      postProcessTypes[pi][si],
                                      postProcessSubTypes[pi][si],
                                      postWeights[pi][si],
                                      energyDeposit[pi][si],
                                      XYZ[pi][si],
                                      usePXPYPZ ? PXPYPZ[pi][si] : TVector3(),
                                      modelIndicies[pi][si],
                                      useT ? T[pi][si]      : 0,
                                      useLocal ? xyz[pi][si] : TVector3(),
                                      useLocal ? pxpypz[pi][si] : TVector3(),
                                      useLinks ? charge[pi][si] : 0,
                                      useEK ? kineticEnergy[pi][si] : 0,
                                      useLinks ? turnsTaken[pi][si] : 0,
                                      useLinks ? rigidity[pi][si] : 0,
                                      useLinks ? mass[pi][si] : 0,
                                      useIon ? isIon[pi][si] : false,
                                      useIon ? ionA[pi][si] : 0,
                                      useIon ? ionZ[pi][si] : 0,
                                      useIon ? nElectrons[pi][si] : 0,
                                      useMat ? materialID[pi][si] : -1,
                                      si);
  return p;
}

std::vector<BDSOutputROOTEventTrajectoryPoint> BDSOutputROOTEventTrajectory::processHistory(int trackIDIn) const
{
  // prevent a bad access
  if (trackID_trackIndex.find(trackIDIn) == trackID_trackIndex.end())
    {
      std::cout << "No such track ID" << std::endl;
      return std::vector<BDSOutputROOTEventTrajectoryPoint>();
    }

  if (postProcessTypes.empty())
    {
      std::cout << "Processes not stored for this file - not possible." << std::endl;
      return std::vector<BDSOutputROOTEventTrajectoryPoint>();
    }
  
  int ti = trackID_trackIndex.at(trackIDIn);

  bool usePXPYPZ = !PXPYPZ.empty();
  bool useT      = !T.empty();
  bool useIon    = !isIon.empty();
  bool useLocal  = !xyz.empty();
  bool useLinks  = !charge.empty();
  bool useEK     = !kineticEnergy.empty();
  bool useMat    = !materialID.empty();

  std::vector<BDSOutputROOTEventTrajectoryPoint> tpv;      // trajectory point vector
  while (ti != 0)
    {
      unsigned int pi  = parentIndex.at(ti);
      unsigned int psi = parentStepIndex.at(ti);
      if (psi > (unsigned int)XYZ[pi].size())
        {
          std::cout << "Not all points stored - defaulting to creation point." << std::endl;
          psi = 0;
        }
      
      BDSOutputROOTEventTrajectoryPoint p(partID[pi],
                                          trackID[pi],
                                          parentID[pi],
                                          parentIndex[pi],
                                          postProcessTypes[pi][psi],
                                          postProcessSubTypes[pi][psi],
                                          postWeights[pi][psi],
                                          energyDeposit[pi][psi],
                                          XYZ[pi][psi],
                                          usePXPYPZ ? PXPYPZ[ti][psi] : TVector3(),
                                          modelIndicies[ti][psi],
                                          useT ? T[ti][psi]      : 0,
                                          useLocal ? xyz[ti][psi] : TVector3(),
                                          useLocal ? pxpypz[ti][psi] : TVector3(),
                                          useLinks ? charge[ti][psi] : 0,
                                          useEK ? kineticEnergy[ti][psi] : 0,
                                          useLinks ? turnsTaken[ti][psi] : 0,
                                          useLinks ? rigidity[ti][psi] : 0,
                                          useLinks ? mass[ti][psi] : 0,
                                          useIon ? isIon[ti][psi] : false,
                                          useIon ? ionA[ti][psi] : 0,
                                          useIon ? ionZ[ti][psi] : 0,
                                          useIon ? nElectrons[ti][psi] : 0,
                                          useMat ? materialID[ti][psi] : -1,
                                          (int)psi);
      tpv.push_back(p);
      ti = (int)pi;
    }
  std::reverse(tpv.begin(),tpv.end());
  return tpv;
}

void BDSOutputROOTEventTrajectory::printTrajectoryInfoByTrackID(int trackIDIn) const
{
  // prevent a bad access
  int storageIndex = 0;
  auto search = trackID_trackIndex.find(trackIDIn);
  if (search == trackID_trackIndex.end())
    {
      std::cout << "No such track ID" << std::endl;
      return;
    }
  else
    {storageIndex = search->second;}
  printTrajectoryInfo(storageIndex);
}

void BDSOutputROOTEventTrajectory::printTrajectoryInfo(int storageIndex) const
{
  int i = storageIndex; // shortcut
  int wdt = 11; // width of columns for print out

  if (i > (int)partID.size() || i < 0)
    {std::cout << "Invalid index" << std::endl; return;}
  
  if (i+1 > n) // safety
    {std::cout << "Index chosen is greater than maximum index of: " << n-1 << std::endl; return;}
  
  // print out regarding the trajectory generally
  std::cout << "Storage index "    << std::setw(wdt) << i
            << ", PDG ID "         << std::setw(wdt) << partID[i]
            << ", Track ID "       << std::setw(wdt) << trackID[i]
            << ", Parent ID "      << std::setw(wdt) << parentID[i] << std::endl;
  std::cout << "Created by Track ID " << parentID[i] << ", with storage index " << parentIndex[i] << ", and at step index " << parentStepIndex[i] << std::endl;
  
  // print out regarding each step of the trajectory
  std::cout << std::setw(wdt) << "step ind"  << " "
            << std::setw(wdt) << "prePrcT"   << " " << std::setw(wdt) << "prePrcST" << " "
            << std::setw(wdt) << "pstPrcT"   << " " << std::setw(wdt) << "pstPrcST" << " "
            << std::setw(wdt) << "X"         << " " << std::setw(wdt) << "Y"        << " "
            << std::setw(wdt) << "Z"         << " " << std::setw(wdt) << "E"        << " "
            << std::setw(wdt) << "p"         << " " << std::setw(wdt) << "p_x"      << " "
            << std::setw(wdt) << "p_y"       << " " << std::setw(wdt) << "p_z"      << " "
            << std::setw(wdt) << "t"         << std::endl;

  for (size_t j=0; j<XYZ[i].size(); ++j)
    {
      std::cout << std::setw(wdt) << j << " "
      << std::setw(wdt) << preProcessTypes[i][j]  << " " << std::setw(wdt) << preProcessSubTypes[i][j]  << " "
                  << std::setw(wdt) << postProcessTypes[i][j] << " " << std::setw(wdt) << postProcessSubTypes[i][j] << " "
                  << std::setw(wdt) << XYZ[i][j].X()          << " " << std::setw(wdt) << XYZ[i][j].Y()             << " "
                  << std::setw(wdt) << XYZ[i][j].Z()          << " " << std::setw(wdt) << energyDeposit[i][j]       << " "
                  << std::setw(wdt) << PXPYPZ[i][j].Mag()     << " " << std::setw(wdt) << PXPYPZ[i][j].X()          << " "
                  << std::setw(wdt) << PXPYPZ[i][j].Y()       << " " << std::setw(wdt) << PXPYPZ[i][j].Z()          << " "
                  << std::setw(wdt) << T[i][j]                << std::endl;
    }
}

bool BDSOutputROOTEventTrajectory::parentIsPrimary(int trackIDIn) const
{
  // prevent a bad access
  if (trackID_trackIndex.find(trackIDIn) == trackID_trackIndex.end())
    {
      std::cout << "No such track ID" << std::endl;
      return false;
    }
  
  unsigned int storageIndex = (unsigned int)trackID_trackIndex.at(trackIDIn);
  unsigned int parentStorageIndex = parentIndex[storageIndex];
  return parentID[parentStorageIndex] == 0;
}

std::ostream& operator<< (std::ostream& out, BDSOutputROOTEventTrajectory const &t)
{
  for (int i=0; i< (int)t.preProcessTypes.size();++i)
    {
      for (int j=0; j< (int)t.preProcessTypes[i].size(); ++j)
        {
          //if(t.preProcessTypes[i][j] != 1 && t.preProcessTypes[i][j] != 7)
          //{
          out << i << " " << j
              << " " << t.preProcessTypes[i][j]   << " " << t.preProcessSubTypes[i][j]
              << " " << t.postProcessTypes[i][j]  << " " << t.postProcessSubTypes[i][j]
              << " " << t.preWeights[i][j]        << " " << t.postWeights[i][j]
              << " " << t.energyDeposit[i][j]     << " " << t.T[i][j]
              << std::endl;
          //}
        }
    }
  return out;
}
