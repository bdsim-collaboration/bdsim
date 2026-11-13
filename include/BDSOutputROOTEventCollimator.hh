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
#ifndef BDSOUTPUTROOTEVENTCOLLIMATOR_H
#define BDSOUTPUTROOTEVENTCOLLIMATOR_H

#ifndef __ROOTBUILD__
#include "BDSOutputROOTEventCollimatorInfo.hh"
#include "G4Types.hh"
class BDSHitCollimator;
#endif

#include "TObject.h"

#include <set>
#include <utility>
#include <vector>

class BDSOutputROOTParticleData;

/**
 * @brief Data stored for each collimator per event.
 * 
 * @author Laurie Nevay
 */

class BDSOutputROOTEventCollimator: public TObject
{
public:
  /// Default constructor for ROOT
  BDSOutputROOTEventCollimator();  

  bool               primaryInteracted;
  bool               primaryStopped;
  int                n;
  std::vector<float> energy; ///< Total energy of particle for each hit.
  std::vector<float> energyDeposited;
  std::vector<float> xIn;
  std::vector<float> yIn;
  std::vector<float> zIn;
  std::vector<float> xpIn;
  std::vector<float> ypIn;
  std::vector<float> zpIn;
  std::vector<float> T;
  std::vector<float> weight;
  std::vector<int>   partID;
  std::vector<int>   parentID;
  std::vector<int>   turn;
  /// A primary can generate multiple hits on a single passage through a collimator,
  /// but we're most interested in the first one where it impacted, so this is a flag
  /// for that hit. The same primary may hit on subsequent turns (however unlikely) in
  /// a ring so we must do this each turn.
  std::vector<bool>  firstPrimaryHitThisTurn;

  std::vector<float> impactParameterX;
  std::vector<float> impactParameterY;

  std::vector<bool>  isIon;
  std::vector<int>   ionA;
  std::vector<int>   ionZ;

  /// Different length set of turn number.
  std::set<int>      turnSet;

  /// @{ These are not filled by default.
  std::vector<int>   charge;
  std::vector<float> kineticEnergy;
  std::vector<float> mass;
  std::vector<float> rigidity;
  /// @}

  double totalEnergyDeposited; ///< Sum of energy deposits including weights.
  
  virtual ~BDSOutputROOTEventCollimator();
#ifndef __ROOTBUILD__
  void Fill(const BDSHitCollimator*                 hit,
	    const BDSOutputROOTEventCollimatorInfo& info,
	    const std::pair<G4double, G4double>&    differences,
	    G4bool                                  storeHits);
  void FillExtras(G4bool fillIonInfo,
		  G4bool fillLinks);

  /// Setter for one off flag per event.
  inline void SetPrimaryStopped(G4bool primaryStoppedIn) {primaryStopped = primaryStoppedIn;}
  
#endif
  /// Copy data from another instance into this one.
  void Fill(const BDSOutputROOTEventCollimator* other);

  /// Flush this instance.
  virtual void Flush();

  static BDSOutputROOTParticleData* particleTable;

  ClassDef(BDSOutputROOTEventCollimator, 1);
};


#endif
