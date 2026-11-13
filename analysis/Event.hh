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
#ifndef EVENT_H
#define EVENT_H

#include <map>
#include <vector>

#include "Rtypes.h" // for classdef
#include "TChain.h"

#include "BDSOutputROOTEventSampler.hh"

#include "RebdsimTypes.hh"

class BDSOutputROOTEventAperture;
class BDSOutputROOTEventCollimator;
class BDSOutputROOTEventCoords;
class BDSOutputROOTEventHistograms;
class BDSOutputROOTEventInfo;
class BDSOutputROOTEventLoss;
class BDSOutputROOTEventLossWorld;
class BDSOutputROOTEventSamplerC;
class BDSOutputROOTEventSamplerS;
class BDSOutputROOTEventTrajectory;

/**
 * @brief Event loader.
 * 
 * @author Stewart Boogert
 */

class Event
{
public:
  Event();
  Event(bool debugIn,
        bool processSamplersIn = false,
        int  dataVersionIn     = 0);
  void CommonCtor();
  virtual ~Event();

  /// @{ Accessor
#ifdef __ROOTDOUBLE__  
  BDSOutputROOTEventSampler<double>* GetPrimaries() {return Primary;}
#else
  BDSOutputROOTEventSampler<float>*  GetPrimaries() {return Primary;}
#endif
  BDSOutputROOTEventLoss*            GetLoss()             {return Eloss;}
  BDSOutputROOTEventLoss*            GetLossVacuum()       {return ElossVacuum;}
  BDSOutputROOTEventLoss*            GetLossTunnel()       {return ElossTunnel;}
  BDSOutputROOTEventLossWorld*       GetLossWorld()        {return ElossWorld;}
  BDSOutputROOTEventLossWorld*       GetLossWorldContents(){return ElossWorldContents;}
  BDSOutputROOTEventLossWorld*       GetLossWorldExit()    {return ElossWorldExit;}
  BDSOutputROOTEventLoss*            GetPrimaryFirstHit()  {return PrimaryFirstHit;}
  BDSOutputROOTEventLoss*            GetPrimaryLastHit()   {return PrimaryLastHit;}
  BDSOutputROOTEventLoss*            GetTunnelHit()        {return TunnelHit;}
  BDSOutputROOTEventTrajectory*      GetTrajectory()       {return Trajectory;}
  BDSOutputROOTEventHistograms*      GetHistograms()       {return Histos;}
  BDSOutputROOTEventInfo*            GetSummary()          {return Summary;}
  BDSOutputROOTEventInfo*            GetInfo()             {return Info;}
#ifdef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<double>* GetSampler(const std::string& name);
#else
  BDSOutputROOTEventSampler<float>*  GetSampler(const std::string& name);
#endif
#ifdef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<double>* GetSampler(int index);
#else
  BDSOutputROOTEventSampler<float>*  GetSampler(int index);
#endif
  BDSOutputROOTEventSamplerC*        GetSamplerC(const std::string& name);
  BDSOutputROOTEventSamplerC*        GetSamplerC(int index);
  BDSOutputROOTEventSamplerS*        GetSamplerS(const std::string& name);
  BDSOutputROOTEventSamplerS*        GetSamplerS(int index);
  BDSOutputROOTEventAperture*        GetAperture() {return ApertureImpacts;}
  BDSOutputROOTEventCollimator*      GetCollimator(const std::string& name);
  BDSOutputROOTEventCollimator*      GetCollimator(int index);
  int                                DataVersion() const {return dataVersion;}
  const std::vector<std::string>&    GetSamplerNames() const {return samplerNames;}
  const std::vector<std::string>&    GetSamplerCylinderNames() const {return samplerCNames;}
  const std::vector<std::string>&    GetSamplerSphereNames() const {return samplerSNames;}
  const std::vector<std::string>&    GetCollimatorNames() const {return collimatorNames;}
  /// @}

  /// Copy data from another event into this event.
  void Fill(Event* other);

  /// Whether there is primary data in the output file.
  inline bool UsePrimaries() const {return usePrimaries;}

  /// Allow setting of data version if default constructor is used. If the default
  /// constructor was used, this function should be used before SetBranchAddress().
  inline void SetDataVersion(int dataVersionIn) {dataVersion = dataVersionIn;}

  /// Set the branch addresses to address the contents of the file. The vector
  /// of sampler names is used to turn only the samplers required. 
  void SetBranchAddress(TTree* t,
                        const RBDS::VectorString* samplerNames      = nullptr,
                        bool                      allBranchesOn     = false,
                        const RBDS::VectorString* branchesToTurnOn  = nullptr,
                        const RBDS::VectorString* collimatorNamesIn = nullptr,
                        const RBDS::VectorString* samplerCNamesIn  = nullptr,
                        const RBDS::VectorString* samplerSNamesIn  = nullptr);

  /// @{ Local variable ROOT data is mapped to.
#ifdef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<double>* Primary;
#else
  BDSOutputROOTEventSampler<float>* Primary;
#endif
  BDSOutputROOTEventCoords*     PrimaryGlobal;
  BDSOutputROOTEventLoss*       Eloss;
  BDSOutputROOTEventLoss*       ElossVacuum;
  BDSOutputROOTEventLoss*       ElossTunnel;
  BDSOutputROOTEventLossWorld*  ElossWorld;
  BDSOutputROOTEventLossWorld*  ElossWorldContents;
  BDSOutputROOTEventLossWorld*  ElossWorldExit;
  BDSOutputROOTEventLoss*       PrimaryFirstHit;
  BDSOutputROOTEventLoss*       PrimaryLastHit;
  BDSOutputROOTEventLoss*       TunnelHit;
  BDSOutputROOTEventTrajectory* Trajectory;
#ifdef __ROOTDOUBLE__
  std::vector<BDSOutputROOTEventSampler<double>*> Samplers;
#else
  std::vector<BDSOutputROOTEventSampler<float>*>  Samplers;
#endif
  std::vector<BDSOutputROOTEventSamplerC*> SamplersC;
  std::vector<BDSOutputROOTEventSamplerS*> SamplersS;
  BDSOutputROOTEventHistograms* Histos;
  BDSOutputROOTEventInfo*       Summary;
  std::vector<BDSOutputROOTEventCollimator*> collimators;
  /// @}
  BDSOutputROOTEventInfo*       Info;    ///< For backwards compatibility

  BDSOutputROOTEventAperture*   ApertureImpacts;

  std::vector<std::string> samplerNames;
  std::vector<std::string> samplerCNames;
  std::vector<std::string> samplerSNames;
#ifdef __ROOTDOUBLE__
  std::map<std::string, BDSOutputROOTEventSampler<double>* > samplerMap;
#else
  std::map<std::string, BDSOutputROOTEventSampler<float>* >  samplerMap;
#endif
  std::map<std::string, BDSOutputROOTEventSamplerC*> samplerCMap;
  std::map<std::string, BDSOutputROOTEventSamplerS*> samplerSMap;

  std::vector<std::string> collimatorNames;
  std::map<std::string, BDSOutputROOTEventCollimator*> collimatorMap;

  /// Relink samplers - i.e. set branch address again in case they might
  /// have changed in memory location by adding another.
  void RelinkSamplers();

  /// Utility method.
  RBDS::VectorString RemoveDuplicates(const RBDS::VectorString& namesIn) const;
  
  /// @{ Utility method for interface building events.
  void RegisterCollimator(const std::string& collimatorName);
  void RegisterSampler(const std::string& samplerName);
  /// @}

  /// @{ Flushing functions.
  void Flush();
  void FlushSamplers();
  void FlushCollimators();
  /// @}
private:
  /// @{ Utility function to avoid repetition of code.
  void SetBranchAddressCollimators(TTree* t,
                                   const RBDS::VectorString* collimatorNames);
  Int_t SetBranchAddressCollimatorSingle(TTree* t,
                                         const std::string& name,
                                         int i);
  /// @}
  
  TTree* tree;
  bool debug;
  bool processSamplers;
  int  dataVersion;
  bool usePrimaries;

  ClassDef(Event, 3);
};

#endif
