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
#ifndef BDSOUTPUTROOTEVENTLOSS_H
#define BDSOUTPUTROOTEVENTLOSS_H

#ifndef __ROOTBUILD__
class BDSHitEnergyDeposition;
class BDSTrajectoryPoint;
class BDSTrajectoryPointHit;
#endif

#include "TObject.h"

#include <vector>

/**
 * @brief Data stored for energy deposition hits per event.
 * 
 * @author Stewart Boogert
 */

class BDSOutputROOTEventLoss: public TObject
{
public:
  BDSOutputROOTEventLoss();
  virtual ~BDSOutputROOTEventLoss();
  
  int                       n;       ///< Number of entries
  std::vector<float>        energy;  ///< Energy deposited in step
  std::vector<float>        S;       ///< Global curvilinear S coordinate
  std::vector<float>        weight;  ///< Weight associated with loss

  std::vector<int>          partID;  ///< ParticleID that create the deposit
  std::vector<int>          trackID; ///< TrackID that created the deposit
  std::vector<int>          parentID;///< ParentID that created the deposit
  
  std::vector<int>          modelID; ///< Geometry model index
	std::vector<int> 					subElementID;
  std::vector<int>          turn;    ///< Turn number

  /// @{ Local coordinate
  std::vector<float>        x;
  std::vector<float>        y;
  std::vector<float>        z;
  /// @}
  
  //// @{ Global coordinates
  std::vector<float>        X;
  std::vector<float>        Y;
  std::vector<float>        Z;
  /// @}

  std::vector<float>        T; ///< Global time (time since beginning of event).

  std::vector<float>        stepLength; ///< Step length taken for hit.
  std::vector<float>        preStepKineticEnergy; ///< Kinetic energy in GeV at pre step point.

  std::vector<int>          postStepProcessType;
  std::vector<int>          postStepProcessSubType;
  
  /// Fill from another instance.
  void Fill(const BDSOutputROOTEventLoss* other);
  virtual void Flush();

  void FlushLocal(); ///< Non-virtual function for initialising / clearing variables.
  
#ifndef __ROOTBUILD__
  BDSOutputROOTEventLoss(bool storeTurnIn,
			 bool storeLinksIn,
			 bool storeModleIDIn,
			 bool storeLocalIn,
			 bool storeGobalIn,
                         bool storeTimeIn,
			 bool storeStepLengthIn,
			 bool storePreStepKineticEnergyIn,
			 bool storePhysicsProcessesIn);
  void Fill(const BDSTrajectoryPointHit* hit);
  void Fill(const BDSHitEnergyDeposition* hit);

  bool storeTurn       = false; ///< Store turn number.
  bool storeLinks      = false; ///< Whether to store links between Eloss and model and trajectors.
  bool storeModelID    = false; ///< Whether to store the beam line index.
  bool storeLocal      = false; ///< Whether to store local coordinates.
  bool storeGlobal     = false; ///< Whether to store global coordinates.
  bool storeTime       = false; ///< Whether to store global time.
  bool storeStepLength = false; ///< Whether to store step length.
  bool storePreStepKineticEnergy = false; ///< Whether to store pre step kinetic energy.
  bool storePhysicsProcesses = false;
#endif

  ClassDef(BDSOutputROOTEventLoss,5);
};

#endif
