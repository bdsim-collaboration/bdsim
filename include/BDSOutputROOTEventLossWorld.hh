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
#ifndef BDSOUTPUTROOTEVENTLOSSWORLD_H
#define BDSOUTPUTROOTEVENTLOSSWORLD_H

#ifndef __ROOTBUILD__
class BDSHitEnergyDepositionGlobal;
#endif

#include "TObject.h"

#include <vector>

/**
 * @brief Data stored for world hits per event.
 *
 * These are for loss hits that have no concept of S or curvilinear
 * coordinates.
 * 
 * @author Laurie Nevay
 */

class BDSOutputROOTEventLossWorld: public TObject
{
public:
  int                n = 0;                 ///< Number of entries.
  std::vector<float> totalEnergy;           ///< Energy deposited in step.
  std::vector<float> preStepKineticEnergy;  ///< Kinetic energy in GeV at pre step point.
  std::vector<float> postStepKineticEnergy; ///< Kinetic energy in GeV at post step point.
  std::vector<float> stepLength;            ///< Step length.

  //// @{ Global coordinates
  std::vector<float> X;
  std::vector<float> Y;
  std::vector<float> Z;
  /// @}
  
  std::vector<float> T;       ///< Global time (time since beginning of event).
  std::vector<int>   partID;  ///< ParticleID that create the deposit
  std::vector<int>   trackID; ///< TrackID that created the deposit
  std::vector<int>   parentID;///< ParentID that created the deposit
  std::vector<float> weight;  ///< Weight
  std::vector<int>   turn;    ///< Turn number
  
  BDSOutputROOTEventLossWorld();
  virtual ~BDSOutputROOTEventLossWorld();
#ifndef __ROOTBUILD__
  void Fill(const BDSHitEnergyDepositionGlobal* hit);
#endif
  /// Fill from another instance.
  void Fill(const BDSOutputROOTEventLossWorld* other);
  virtual void Flush();

  ClassDef(BDSOutputROOTEventLossWorld,1);
};


#endif
