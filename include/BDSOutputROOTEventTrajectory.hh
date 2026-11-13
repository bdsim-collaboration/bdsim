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
#ifndef BDSOUTPUTROOTEVENTTRAJECTORY_H
#define BDSOUTPUTROOTEVENTTRAJECTORY_H
#include "Rtypes.h"
#include "TVector3.h"

#include "BDSOutputROOTEventTrajectoryPoint.hh"
#include "BDSTrajectoryFilter.hh"
#include "BDSTrajectoriesToStore.hh"

#include <bitset>
#include <map>
#include <vector>

#ifndef __ROOTBUILD__
class BDSHitEnergyDeposition;
class BDSTrajectory;
template <class T> class G4THitsCollection;
typedef G4THitsCollection<BDSHitEnergyDeposition> BDSHitsCollectionEnergyDeposition;
class G4Material;
namespace BDS
{
  struct TrajectoryOptions;
}
#endif

class BDSAuxiliaryNavigator;

#if 0
0  fNotDefined,
  1  fTransportation,
    91 TRANSPORTATION,
    92 COUPLED_TRANSPORTATION
  2  fElectromagnetic,
    1  fCoulombScattering,
    2  fIonisation,
    3  fBremsstrahlung,
    4  fPairProdByCharged,
    5  fAnnihilation,
    6  fAnnihilationToMuMu,
    7  fAnnihilationToHadrons,
    8  fNuclearStopping,
    10 fMultipleScattering,
    11 fRayleigh,
    12 fPhotoElectricEffect,
    13 fComptonScattering,
    14 fGammaConversion,
    15 fGammaConversionToMuMu,
    21 fCerenkov,
    22 fScintillation,
    23 fSynchrotronRadiation,
    24 fTransitionRadiation
  3  fOptical,
    0 kCerenkov,
    1 kScintillation,
    2 kAbsorption,
    3 kRayleigh,
    4 kMieHG,
    5 kBoundary,
    6 kWLS,
    7 kNoProcess
  4  fHadronic,
    111 fHadronElastic,
    121 fHadronInelastic,
    131 fCapture,
    141 fFission,
    151 fHadronAtRest,
    152 fLeptonAtRest,
    161 fChargeExchange,
    210 fRadioactiveDecay
  5  fPhotolepton_hadron,
  6  fDecay,
    201 DECAY = 201,
    202 DECAY_WithSpin,
    203 DECAY_PionMakeSpin,
    210 DECAY_Radioactive,
    211 DECAY_Unknown,
    231 DECAY_External = 231
  7  fGeneral,
    401 STEP_LIMITER,
    402 USER_SPECIAL_CUTS,
    403 NEUTRON_KILLER
  8  fParameterisation,
  9  fUserDefined,
  10 fParallel,
    491 SBLN_UNKNOWN!
  11 fPhonon,
  12 fUCN
#endif

/**
 * @brief Structure to record a trajectory.
 * 
 * @author Stewart Boogert
 */

class BDSOutputROOTEventTrajectory: public TObject
{
public:
  BDSOutputROOTEventTrajectory();
  virtual ~BDSOutputROOTEventTrajectory();
#ifndef __ROOTBUILD__
  void Fill(const BDSTrajectoriesToStore* t,
            int  storeStepPointsN,
            bool storeStepPointLast,
            const BDS::TrajectoryOptions& storageOptions,
            const std::map<G4Material*, short int>& materialToID);
  void Fill(const BDSHitsCollectionEnergyDeposition* phc);

  /// Temporary structure for an individual trajectory used to convert types.
  struct IndividualTrajectory
  {
    std::vector<int>      preProcessType;
    std::vector<int>      preProcessSubType;
    std::vector<int>      postProcessType;
    std::vector<int>      postProcessSubType;
    std::vector<double>   preWeight;
    std::vector<double>   postWeight;
    std::vector<double>   energyDeposit;
    std::vector<TVector3> XYZ;
    std::vector<TVector3> PXPYPZ;
    std::vector<double>   S;
    std::vector<double>   T;    
    std::vector<TVector3> xyz;
    std::vector<TVector3> pxpypz;  
    std::vector<int>      charge;
    std::vector<double>   kineticEnergy;
    std::vector<int>      turn;
    std::vector<double>   mass;
    std::vector<double>   rigidity;    
    std::vector<bool>     isIon;
    std::vector<int>      ionA;
    std::vector<int>      ionZ;
    std::vector<int>      nElectrons;
    std::vector<int>      modelIndex;
    std::vector<short int> materialID;
  };

  /// Fill an trajectory point with index 'i' into the IndividualTrajectory struct
  /// (basic C++ / ROOT types) from Geant4 types from 'traj' trajectory for 1 track.
  void FillIndividualTrajectory(IndividualTrajectory& itj,
				BDSTrajectory*        traj,
				int                   i,
				const std::map<G4Material*, short int>& materialToID) const;
#endif

  /// Required to find beamline index careful including in streamer.
  BDSAuxiliaryNavigator* auxNavigator; //! add comment to avoid warning (no need to make persistent, see issue #191)
  
  virtual void Flush();
  void FlushLocal(); ///< Non-virtual version for initialising members.
  void Fill(const BDSOutputROOTEventTrajectory* other);

    
  int n;
  std::vector<std::bitset<BDS::NTrajectoryFilters> > filters;
  std::vector<int>                   partID;
  std::vector<unsigned int>          trackID;
  std::vector<unsigned int>          parentID;
  std::vector<unsigned int>          parentIndex;
  std::vector<unsigned int>          parentStepIndex;
  std::vector<int>                   primaryStepIndex;
  std::vector<int>                   depth;

  std::vector<std::vector<int>>      preProcessTypes;
  std::vector<std::vector<int>>      preProcessSubTypes;
  std::vector<std::vector<int>>      postProcessTypes;
  std::vector<std::vector<int>>      postProcessSubTypes;

  std::vector<std::vector<double>>   preWeights;
  std::vector<std::vector<double>>   postWeights;
  std::vector<std::vector<double>>   energyDeposit;

  std::vector<std::vector<TVector3>> XYZ;
  std::vector<std::vector<double>>   S;
  std::vector<std::vector<TVector3>> PXPYPZ;
  std::vector<std::vector<double>>   T;

  /// @{ Local coordinates.
  std::vector<std::vector<TVector3>> xyz;
  std::vector<std::vector<TVector3>> pxpypz;
  /// @}

  /// @{ Link trajectory information.
  std::vector<std::vector<int>>      charge;
  std::vector<std::vector<double>>   kineticEnergy;
  std::vector<std::vector<int>>      turnsTaken;
  std::vector<std::vector<double>>   mass;
  std::vector<std::vector<double>>   rigidity;
  /// @}

  /// @{ Ion trajectory information.
  std::vector<std::vector<bool>>     isIon;
  std::vector<std::vector<int>>      ionA;
  std::vector<std::vector<int>>      ionZ;
  std::vector<std::vector<int>>      nElectrons;
  /// @}
  
  std::vector<std::vector<short int>> materialID;

  std::vector<std::vector<int>>      modelIndicies;

  std::map<int, int>                 trackID_trackIndex;// trackID to storage index
  
  //  std::map<int, std::pair<int,int>>  trackIndex_trackProcess;     // trackProcess pair<trackIndex,trackProcessIndex>
  //  std::map<int, int>                 trackIndex_modelIndex;       // trackIndex to model index map
  //  std::map<int, std::vector<int>>    modelIndex_trackIndex;       // modelIndex to track index map
  //  std::pair<int,int>                 findParentProcess(int trackIndex);

  std::vector<BDSOutputROOTEventTrajectoryPoint> trackInteractions(int trackIDIn) const;
  BDSOutputROOTEventTrajectoryPoint              primaryProcessPoint(int trackIDIn) const;
  BDSOutputROOTEventTrajectoryPoint              parentProcessPoint(int trackIDIn) const;
  std::vector<BDSOutputROOTEventTrajectoryPoint> processHistory(int trackIDIn) const;
  void                                           printTrajectoryInfoByTrackID(int trackIDIn) const;
  void                                           printTrajectoryInfo(int storageIndex) const;
  bool                                           parentIsPrimary(int trackIDIn) const;

  friend std::ostream& operator<< (std::ostream& out, BDSOutputROOTEventTrajectory const &p);
  
  ClassDef(BDSOutputROOTEventTrajectory,5);
};

#endif
