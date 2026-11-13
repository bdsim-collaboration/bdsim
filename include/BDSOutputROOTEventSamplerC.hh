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
#ifndef BDSOUTPUTROOTEVENTSAMPLERC_H
#define BDSOUTPUTROOTEVENTSAMPLERC_H

#include "Rtypes.h"
#include "TObject.h"

#include <string>
#include <vector>

class BDSOutputROOTParticleData;
class TTree;

#ifndef __ROOTBUILD__ 
#include "globals.hh"
class BDSParticleCoordsCylindrical;
class BDSHitSamplerCylinder;
class BDSPrimaryVertexInformationV;
#endif

/**
 * @brief Information stored per cylindrical sampler per event.
 * 
 * @author Laurie Nevay
 */

class BDSOutputROOTEventSamplerC: public TObject
{
public:
  std::string samplerName; //|| Don't split the header
  
  int                n;
  std::vector<float> totalEnergy;
  std::vector<float> z;
  std::vector<float> phi;
  std::vector<float> rp;
  std::vector<float> zp;
  std::vector<float> phip;
  std::vector<float> p;
  std::vector<float> T;

  std::vector<float> weight;
  std::vector<int>   partID;
  std::vector<int>   parentID;
  std::vector<int>   trackID;
  int                modelID;
  std::vector<int>   turnNumber;

  float              S;

  /// @{ These are not filled by default.
  std::vector<int>     charge;
  std::vector<float>   kineticEnergy;
  std::vector<float>   mass;
  std::vector<float>   rigidity;
  std::vector<bool>    isIon;
  std::vector<int>     ionA;
  std::vector<int>     ionZ;
  std::vector<int>     nElectrons;
  /// @}

  /// @{ Function to calculate on the fly the parameters.
  std::vector<float>   getKineticEnergy();
  std::vector<float>   getMass();
  std::vector<float>   getRigidity();
  std::vector<bool>    getIsIon();
  std::vector<int>     getIonA();
  std::vector<int>     getIonZ();
  /// @}  
  
  BDSOutputROOTEventSamplerC();
  explicit BDSOutputROOTEventSamplerC(const std::string& samplerNameIn);
  virtual ~BDSOutputROOTEventSamplerC();
#ifndef __ROOTBUILD__
  void Fill(const BDSHitSamplerCylinder* hit,
	    G4bool storeMass          = false,
	    G4bool storeCharge        = false,
	    G4bool storeElectrons     = false,
	    G4bool storeRigidity      = false,
	    G4bool storeKineticEnergy = false);
#endif
  void Fill(const BDSOutputROOTEventSamplerC* other);

  /// @{ Calculate and fill calculated variables.
  inline void FillIon() {isIon = getIsIon(); ionA = getIonA(); ionZ = getIonZ();}
  /// @}
  
  void SetBranchAddress(TTree*);
  virtual void Flush();  ///< Clean Sampler

  static BDSOutputROOTParticleData* particleTable;

  ClassDef(BDSOutputROOTEventSamplerC,1);
};

#endif
