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
#include "BDSOutputROOTEventModel.hh"

#ifndef __ROOTBUILD__
#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetStrength.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSSamplerRegistry.hh"

#include "G4RotationMatrix.hh"
#include "G4String.hh"
#include "G4Types.hh"

#include <algorithm>
#include <iterator>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#endif

ClassImp(BDSOutputROOTEventModel)

BDSOutputROOTEventModel::BDSOutputROOTEventModel():
  n(0),
  storeCollimatorInfo(false),
  storeCavityInfo(false),
  nCavities(0),
  nCollimators(0)
{
  Flush();
}

BDSOutputROOTEventModel::~BDSOutputROOTEventModel()
{;}

int BDSOutputROOTEventModel::findNearestElement(const TVector3& point) const
{
  // we make a vector of pairs of the distance between the mid of each element
  // and the specified point along with the index. We then sort this vector by
  // the distance (first part of pair only). Could use a map as it's sorted but
  // can't guarantee access with a double or float as a key due to binary representation.
  std::vector<std::pair<float, int> > distanceAndIndex;
  distanceAndIndex.reserve(midRefPos.size());
  for (int i = 0; i < (int)midRefPos.size(); i++)
    {distanceAndIndex.emplace_back(std::make_pair((midRefPos[i]-point).Mag(), i));}
  
  struct customLess
  {bool operator()(std::pair<float, int>& a, std::pair<float, int>& b) const {return a.first < b.first;};};
  std::sort(distanceAndIndex.begin(), distanceAndIndex.end(), customLess());
  return distanceAndIndex[0].second;
}

void BDSOutputROOTEventModel::Flush()
{
  n = 0;
  componentName.clear();
  placementName.clear();
  componentType.clear();
  length.clear();
  angle.clear();
  staPos.clear();
  midPos.clear();
  endPos.clear();
  staRot.clear();
  midRot.clear();
  endRot.clear();
  staRefPos.clear();
  midRefPos.clear();
  endRefPos.clear();
  staRefRot.clear();
  midRefRot.clear();
  endRefRot.clear();
  tilt.clear();
  offsetX.clear();
  offsetY.clear();
  staS.clear();
  midS.clear();
  endS.clear();
  beamPipeType.clear();
  beamPipeAper1.clear();
  beamPipeAper2.clear();
  beamPipeAper3.clear();
  beamPipeAper4.clear();
  material.clear();
  k1.clear();
  k2.clear();
  k3.clear();
  k4.clear();
  k5.clear();
  k6.clear();
  k7.clear();
  k8.clear();
  k9.clear();
  k10.clear();
  k11.clear();
  k12.clear();
  k1s.clear();
  k2s.clear();
  k3s.clear();
  k4s.clear();
  k5s.clear();
  k6s.clear();
  k7s.clear();
  k8s.clear();
  k9s.clear();
  k10s.clear();
  k11s.clear();
  k12s.clear();
  ks.clear();
  hkick.clear();
  vkick.clear();
  bField.clear();
  eField.clear();
  e1.clear();
  e2.clear();
  fint.clear();
  fintx.clear();
  fintk2.clear();
  fintxk2.clear();
  pvName.clear();
  pvNameWPointer.clear();
  midT.clear();
  staP.clear();
  staEk.clear();

  storeCavityInfo = false;
  cavityIndices.clear();
  cavityIndicesByName.clear();
  nCavities = 0;
  cavityInfo.clear();
  cavityBranchNamesUnique.clear();
  storeCollimatorInfo = false;
  collimatorIndices.clear();
  collimatorIndicesByName.clear();
  nCollimators = 0;
  collimatorInfo.clear();
  collimatorBranchNamesUnique.clear();
  scoringMeshTranslation.clear();
  scoringMeshRotation.clear();
  scoringMeshName.clear();

  materialIDToName.clear();
  materialNameToID.clear();
  subElementIDs.clear();
  subElementNames.clear();
  
  samplerNamesUnique.clear();
  samplerSPosition.clear();
  samplerCNamesUnique.clear();
  samplerSNamesUnique.clear();
  samplerCRadius.clear();
  samplerSRadius.clear();
}

#ifndef __ROOTBUILD__
BDSOutputROOTEventModel::BDSOutputROOTEventModel(G4bool storeCollimatorInfoIn,
                                                 G4bool storeCavityInfoIn):
  n(0),
  storeCollimatorInfo(storeCollimatorInfoIn),
  storeCavityInfo(storeCavityInfoIn),
  nCavities(0),
  nCollimators(0)
{;}

TRotation BDSOutputROOTEventModel::ConvertToROOT(const G4RotationMatrix* rm) const
{
  TRotation rr = TRotation();
  rr.SetToIdentity();
  if (!rm)
    {return rr;}
  
  double rotAngle;
  CLHEP::Hep3Vector axis;
  
  rm->getAngleAxis(rotAngle, axis);
  rr.Rotate(rotAngle, TVector3(axis.x(), axis.y(), axis.z()));
  return rr;
}

TRotation BDSOutputROOTEventModel::ConvertToROOT(const G4RotationMatrix& rm) const
{
  return ConvertToROOT(&rm);
}

TVector3 BDSOutputROOTEventModel::ConvertToROOT(const G4ThreeVector& v) const
{
  return TVector3(v.x() / CLHEP::m, v.y() / CLHEP::m, v.z() / CLHEP::m);
}

void BDSOutputROOTEventModel::Fill(const std::vector<G4int>&                collimatorIndicesIn,
                                   const std::map<G4String, G4int>&         collimatorIndicesByNameIn,
                                   const std::vector<BDSOutputROOTEventCollimatorInfo>& collimatorInfoIn,
                                   const std::vector<G4String>&             collimatorBranchNamesIn,
                                   const std::vector<G4int>&                cavityIndicesIn,
                                   const std::map<G4String, G4int>&         cavityIndicesByNameIn,
                                   const std::vector<BDSOutputROOTEventCavityInfo>& cavityInfoIn,
                                   const std::vector<G4String>&             cavityBranchNamesIn,
                                   const std::map<G4String, G4Transform3D>* scorerMeshPlacements,
                                   const std::map<short int, G4String>*     materialIDToNameUnique,
                                   G4bool storeTrajectory)
{
  auto sr = BDSSamplerRegistry::Instance();
  for (const auto& nameSPos : sr->GetUniquePlaneNamesAndSPosition())
    {
      samplerNamesUnique.push_back(std::string(nameSPos.first) + ".");
      samplerSPosition.push_back((double) nameSPos.second / CLHEP::m);
    }
  for (const auto& name : sr->GetUniqueNamesCylinder())
    {samplerCNamesUnique.push_back(std::string(name) + ".");}
  for (const auto& name : sr->GetUniqueNamesSphere())
    {samplerSNamesUnique.push_back(std::string(name) + ".");}
  samplerCRadius = sr->GetUniqueNameToRadiusCylinder();
  samplerSRadius = sr->GetUniqueNameToRadiusSphere();

  for (const auto& name : collimatorBranchNamesIn)
    {collimatorBranchNamesUnique.push_back(std::string(name) + ".");}
  for (const auto& name : cavityBranchNamesIn)
    {cavityBranchNamesUnique.push_back(std::string(name) + ".");}

  if (scorerMeshPlacements)
    {
      for (const auto& kv : *scorerMeshPlacements)
        {
          const G4String& name = kv.first;
          scoringMeshTranslation[name] = ConvertToROOT(kv.second.getTranslation());
          scoringMeshRotation[name]    = ConvertToROOT(kv.second.getRotation());
          scoringMeshName.emplace_back(name);
        }
    }
  
  const BDSBeamline* beamline = BDSAcceleratorModel::Instance()->BeamlineMain();
  if (!beamline)
    {return;} // in case of generatePrimariesOnly there is no model - return

  if (storeCollimatorInfo)
    {
      for (const auto value : collimatorIndicesIn)
        {collimatorIndices.push_back((int)value);}
      
      nCollimators = (int)collimatorIndices.size();
      
      for (const auto& kv : collimatorIndicesByNameIn)
        {collimatorIndicesByName[(std::string)kv.first] = (int)kv.second;}

      collimatorInfo = collimatorInfoIn;
    }

  if (storeCavityInfo)
    {
      for (const auto value : cavityIndicesIn)
        {cavityIndices.push_back((int)value);}

      nCavities = (int)cavityIndices.size();

      for (const auto& kv : cavityIndicesByNameIn)
        {cavityIndicesByName[(std::string)kv.first] = (int)kv.second;}

      cavityInfo = cavityInfoIn;
    }

  if (materialIDToNameUnique && storeTrajectory)
    {
      for (const auto& kv : *materialIDToNameUnique)
        {
          materialIDToName[kv.first] = (std::string)kv.second;
          materialNameToID[(std::string)kv.second] = kv.first;
        }
    }

  n = (int)beamline->size();
  
  for (auto i = beamline->begin(); i != beamline->end(); ++i)
    {
      componentName.push_back((*i)->GetName());
      placementName.push_back((*i)->GetPlacementName());
      componentType.push_back((*i)->GetType());
      length.push_back((float)((*i)->GetAcceleratorComponent()->GetArcLength() / CLHEP::m));
      angle.push_back((float)((*i)->GetAcceleratorComponent()->GetAngle() / CLHEP::radian));
      staPos.push_back(ConvertToROOT((*i)->GetPositionStart()));
      midPos.push_back(ConvertToROOT((*i)->GetPositionMiddle()));
      endPos.push_back(ConvertToROOT((*i)->GetPositionEnd()));
      staRot.push_back(ConvertToROOT((*i)->GetRotationStart()));
      midRot.push_back(ConvertToROOT((*i)->GetRotationMiddle()));
      endRot.push_back(ConvertToROOT((*i)->GetRotationEnd()));
      staRefPos.emplace_back(ConvertToROOT((*i)->GetReferencePositionStart()));
      midRefPos.emplace_back(ConvertToROOT((*i)->GetReferencePositionMiddle()));
      endRefPos.emplace_back(ConvertToROOT((*i)->GetReferencePositionEnd()));
      staRefRot.push_back(ConvertToROOT((*i)->GetReferenceRotationStart()));
      midRefRot.push_back(ConvertToROOT((*i)->GetReferenceRotationMiddle()));
      endRefRot.push_back(ConvertToROOT((*i)->GetReferenceRotationEnd()));
      
      // tilt and offset
      BDSTiltOffset* to = (*i)->GetTiltOffset();
      if (to)
        {
          tilt.push_back((float)(to->GetTilt() / CLHEP::rad));
          offsetX.push_back((float)(to->GetXOffset() / CLHEP::m));
          offsetY.push_back((float)(to->GetYOffset() / CLHEP::m));
        }
      else
        {
          tilt.push_back(0);
          offsetX.push_back(0);
          offsetY.push_back(0);
        }
      
      // S positions
      staS.push_back((float)((*i)->GetSPositionStart()  / CLHEP::m));
      midS.push_back((float)((*i)->GetSPositionMiddle() / CLHEP::m));
      endS.push_back((float)((*i)->GetSPositionEnd()    / CLHEP::m));
      
      // beam pipe
      BDSBeamPipeInfo* beampipeinfo = (*i)->GetBeamPipeInfo();
      beamPipeType.push_back(beampipeinfo  ? beampipeinfo->beamPipeType.ToString() : "");
      beamPipeAper1.push_back(beampipeinfo ? beampipeinfo->aper1 / CLHEP::m : 0);
      beamPipeAper2.push_back(beampipeinfo ? beampipeinfo->aper2 / CLHEP::m : 0);
      beamPipeAper3.push_back(beampipeinfo ? beampipeinfo->aper3 / CLHEP::m : 0);
      beamPipeAper4.push_back(beampipeinfo ? beampipeinfo->aper4 / CLHEP::m : 0);
      
      // associated material if any
      const auto accComp = (*i)->GetAcceleratorComponent();
      material.push_back(accComp->Material());
      
      // helper shortcuts to all the member vectors
      std::vector<std::vector<float>*> localNorm = {&k1,&k2,&k3,&k4,&k5,&k6,&k7,&k8,&k9,&k10,&k11,&k12};
      std::vector<std::vector<float>*> localSkew = {&k1s,&k2s,&k3s,&k4s,&k5s,&k6s,&k7s,&k8s,&k9s,&k10s,&k11s,&k12s};
      
      // helper lambda to avoid duplication
      auto fillzero = [&]
                      {
                        for (int j = 0; j < (int)localNorm.size(); j++)
                          {localNorm[j]->push_back(0);}
                        for (int j = 0; j < (int)localSkew.size(); j++)
                          {localSkew[j]->push_back(0);}
                        ks.push_back(0);
                        hkick.push_back(0);
                        vkick.push_back(0);
                        bField.push_back(0);
                        eField.push_back(0);
                        e1.push_back(0);
                        e2.push_back(0);
                        hgap.push_back(0);
                        fint.push_back(0);
                        fintx.push_back(0);
                        fintk2.push_back(0);
                        fintxk2.push_back(0);
                      };
  
      // do this bit first as we test for magnet strengths later and then do a 'continue' in the for loop
      auto setOfPVs = BDSPhysicalVolumeInfoRegistry::Instance()->PVsForBeamlineElement(*i);
      std::vector<std::string> localPVNames;
      std::vector<std::string> localPVNamesWPointer;
      if (setOfPVs)
        {
          auto name = [](G4VPhysicalVolume* pv){return pv->GetName();};
          std::transform(setOfPVs->begin(), setOfPVs->end(), std::back_inserter(localPVNames), name);
          auto fullName = [](G4VPhysicalVolume* pv)
                          {
                            std::stringstream ss;
                            ss << static_cast<const void*>(pv);
                            std::string addressName = ss.str();
                            return pv->GetName() + addressName;
                          };
          std::transform(setOfPVs->begin(), setOfPVs->end(), std::back_inserter(localPVNamesWPointer), fullName);
        }
      // always push it back even if an empty vector
      pvName.push_back(localPVNames);
      pvNameWPointer.push_back(localPVNamesWPointer);

      midT.push_back((float)(*i)->GetSynchronousTMiddle()/CLHEP::ns);
      staP.push_back((float)(*i)->GetStartMomentum()/CLHEP::GeV);
      staEk.push_back((float)(*i)->GetStartKineticEnergy()/CLHEP::GeV);
      
      // fill magnet strength data
      // NOTE - has a 'continue'
      if (BDSMagnet* mag = dynamic_cast<BDSMagnet*>(accComp))
        {
          const BDSMagnetStrength* ms = mag->MagnetStrength();
          if (!ms)
            {
              fillzero();
              continue;
            }
          // assume localNorm and normComponents are same size
          std::vector<G4double> normComponents = ms->NormalComponents();
          for (int j = 0; j < (int)localNorm.size(); j++)
            {localNorm[j]->push_back((float)normComponents[j]);}
          std::vector<G4double> skewComponents = ms->SkewComponents();
          for (int j = 0; j < (int)localSkew.size(); j++)
            {localSkew[j]->push_back((float)skewComponents[j]);}
          
          ks.push_back((float)((*ms)["ks"]/BDSMagnetStrength::Unit("ks")));
          hkick.push_back( (float)((*ms)["hkick"]/BDSMagnetStrength::Unit("hkick")));
          vkick.push_back( (float)((*ms)["vkick"]/BDSMagnetStrength::Unit("vkick")));
          bField.push_back((float)((*ms)["field"]/BDSMagnetStrength::Unit("field")));
          eField.push_back((float)((*ms)["efield"]/BDSMagnetStrength::Unit("efield")));
          e1.push_back((float)((*ms)["e1"]/BDSMagnetStrength::Unit("e1")));
          e2.push_back((float)((*ms)["e2"]/BDSMagnetStrength::Unit("e2")));
          hgap.push_back((float)((*ms)["hgap"]/BDSMagnetStrength::Unit("hgap")));
          fint.push_back((float)(*ms)["fint"]);
          fintx.push_back((float)(*ms)["fintx"]);
          fintk2.push_back((float)(*ms)["fintk2"]);
          fintxk2.push_back((float)(*ms)["fintxk2"]);
        }
      else
        {// not a magnet
          fillzero();
        }
    }
  const auto& idToName = BDSPhysicalVolumeInfoRegistry::Instance()->GetSubElementIDToNameMap();
  for (const auto& entry : idToName)
  {
    subElementIDs.push_back(entry.first);
    subElementNames.push_back(entry.second);
  }
}
#endif
