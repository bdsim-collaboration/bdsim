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
#ifndef BDSOUTPUTROOTEVENTAPERTURE_H
#define BDSOUTPUTROOTEVENTAPERTURE_H

#ifndef __ROOTBUILD__
#include "G4Types.hh"
class BDSHitApertureImpact;
#endif

#include "TObject.h"

#include <vector>

class BDSOutputROOTParticleData;

/**
 * @brief Data stored for energy deposition hits per event.
 * 
 * @author Laurie Nevay
 */

class BDSOutputROOTEventAperture: public TObject
{
public:
  int                       n = 0;   ///< Number of entries
  std::vector<float>        energy;  ///< Total energy of particle
  std::vector<double>       S;       ///< Global curvilinear S coordinate
  std::vector<float>        weight;  ///< Weight associated with loss
  std::vector<bool>         isPrimary;
  std::vector<bool>         firstPrimaryImpact; ///< Whether the first time the primary is passing through.
  std::vector<int>          partID;  ///< ParticleID that create the deposit
  std::vector<int>          turn;    ///< Turn number
  std::vector<float>        x;
  std::vector<float>        y;
  std::vector<float>        xp;
  std::vector<float>        yp;
  std::vector<float>        T; ///< Global time (time since beginning of event).
  std::vector<float>        kineticEnergy; ///< Kinetic energy in GeV at pre step point.
  std::vector<bool>         isIon;
  std::vector<int>          ionA;
  std::vector<int>          ionZ;
  std::vector<int>          nElectrons;
  
  std::vector<int>          trackID; ///< TrackID that created the deposit
  std::vector<int>          parentID;///< ParentID that created the deposit
  std::vector<int>          modelID; ///< Geometry model index
  
  BDSOutputROOTEventAperture();
  virtual ~BDSOutputROOTEventAperture();

#ifndef __ROOTBUILD__
  void Fill(const BDSHitApertureImpact* hit,
	    G4bool isPrimaryFirstImpact);
#endif
  void Fill(const BDSOutputROOTEventAperture* other);
  virtual void Flush(){FlushLocal();}

  /// Shouldn't call a virtual function in the constructor so we have a simple
  /// version here that doesn't override the one from TObject.
  void FlushLocal();

  static BDSOutputROOTParticleData* particleTable;

  ClassDef(BDSOutputROOTEventAperture,1);
};

#endif
