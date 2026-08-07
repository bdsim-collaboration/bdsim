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
#ifndef BDSOUTPUTROOTEVENTMODEL_H
#define BDSOUTPUTROOTEVENTMODEL_H

#include "BDSOutputROOTEventCavityInfo.hh"
#include "BDSOutputROOTEventCollimatorInfo.hh"

#include <map>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "TObject.h"
#include "TVector3.h"
#include "TRotation.h"

#ifndef __ROOTBUILD__
#include "G4RotationMatrix.hh"
#include "G4String.hh"
#include "G4Transform3D.hh"
#include "G4Types.hh"
#endif

/**
 * @brief Information stored per model representing accelerator.
 * 
 * @author Stewart Boogert
 */

class BDSOutputROOTEventModel: public TObject
{
public:
  /// Default constructor
  BDSOutputROOTEventModel();
  /// Destructor
  virtual ~BDSOutputROOTEventModel();
  
  /// Initialise all members.
  void Flush();
  
  /// Find element index closest to point.
  int findNearestElement(const TVector3& point) const;

#ifndef __ROOTBUILD__
  /// Constructor for whether to store collimator and cavity information or not.
  explicit BDSOutputROOTEventModel(G4bool storeCollimatorInfoIn,
                                   G4bool storeCavityInfoIn);
  
  /// Utility function.
  TRotation ConvertToROOT(const G4RotationMatrix* rm) const;
  TRotation ConvertToROOT(const G4RotationMatrix& rm) const;
  TVector3  ConvertToROOT(const G4ThreeVector& v) const;
  
  /// Fill root output.
  virtual void Fill(const std::vector<G4int>& collimatorIndicesIn = {},
                    const std::map<G4String, G4int>& collimatorIndicesByNameIn = {},
                    const std::vector<BDSOutputROOTEventCollimatorInfo>& collimatorInfoIn = {},
                    const std::vector<G4String>& collimatorBranchNamesIn = {},
                    const std::vector<G4int>& cavityIndicesIn = {},
                    const std::map<G4String, G4int>& cavityIndicesByNameIn = {},
                    const std::vector<BDSOutputROOTEventCavityInfo>& cavityInfoIn = {},
                    const std::vector<G4String>& cavityBranchNamesIn = {},
                    const std::map<G4String, G4Transform3D>* scorerMeshPlacements = nullptr,
		    const std::map<short int, G4String>* materialIDToNameUnique = nullptr,
		    G4bool storeTrajectory = false);
#endif
  
  int n;
  std::vector<std::string> componentName;
  std::vector<std::string> placementName;
  std::vector<std::string> componentType;
  std::vector<float>       length;
  std::vector<float>       angle;
  std::vector<TVector3>    staPos;
  std::vector<TVector3>    midPos;
  std::vector<TVector3>    endPos;
  std::vector<TRotation>   staRot;
  std::vector<TRotation>   midRot;
  std::vector<TRotation>   endRot;
  std::vector<TVector3>    staRefPos;
  std::vector<TVector3>    midRefPos;
  std::vector<TVector3>    endRefPos;
  std::vector<TRotation>   staRefRot;
  std::vector<TRotation>   midRefRot;
  std::vector<TRotation>   endRefRot;
  std::vector<float>       tilt;
  std::vector<float>       offsetX;
  std::vector<float>       offsetY;
  std::vector<float>       staS;
  std::vector<float>       midS;
  std::vector<float>       endS;
  std::vector<std::string> beamPipeType;
  std::vector<double>      beamPipeAper1;
  std::vector<double>      beamPipeAper2;
  std::vector<double>      beamPipeAper3;
  std::vector<double>      beamPipeAper4;
  std::vector<std::string> material;       ///< Material associated with element if any.
  std::vector<float>       k1;
  std::vector<float>       k2;
  std::vector<float>       k3;
  std::vector<float>       k4;
  std::vector<float>       k5;
  std::vector<float>       k6;
  std::vector<float>       k7;
  std::vector<float>       k8;
  std::vector<float>       k9;
  std::vector<float>       k10;
  std::vector<float>       k11;
  std::vector<float>       k12;
  std::vector<float>       k1s;
  std::vector<float>       k2s;
  std::vector<float>       k3s;
  std::vector<float>       k4s;
  std::vector<float>       k5s;
  std::vector<float>       k6s;
  std::vector<float>       k7s;
  std::vector<float>       k8s;
  std::vector<float>       k9s;
  std::vector<float>       k10s;
  std::vector<float>       k11s;
  std::vector<float>       k12s;
  std::vector<float>       ks;     ///< Solenoid strength.
  std::vector<float>       hkick;  ///< Horizontal fractional momentum kick.
  std::vector<float>       vkick;  ///< Vertical fractional momentum kick.
  std::vector<float>       bField; ///< B field in T.
  std::vector<float>       eField; ///< E field in V/m.
  std::vector<float>       e1;
  std::vector<float>       e2;
  std::vector<float>       hgap;
  std::vector<float>       fint;
  std::vector<float>       fintx;
  std::vector<float>       fintk2;
  std::vector<float>       fintxk2;
  std::vector<std::vector<std::string>> pvName;
  std::vector<std::vector<std::string>> pvNameWPointer;
  std::vector<float>       midT;
  std::vector<float>       staP;
  std::vector<float>       staEk;

  /// Whether optional collimator information was stored.
  bool storeCollimatorInfo;

  /// Whether optional cavity information was stored.
  bool storeCavityInfo;

  /// Optional cache of indices in beam line of cavities used to extract
  /// cavity information.
  std::vector<int> cavityIndices;

  /// Similar cache but by name of cavity as built by BDSIM.
  std::map<std::string, int> cavityIndicesByName;

  int nCavities; ///< Number of cavities in beam line.
  std::vector<BDSOutputROOTEventCavityInfo> cavityInfo; ///< cavity information explicitly.

  /// Vector of all cavity branch names in event tree used to load data.
  std::vector<std::string> cavityBranchNamesUnique;

  /// Optional cache of indices in beam line of collimators used to extract
  /// collimator information.
  std::vector<int> collimatorIndices;

  /// Similar cache but by name of collimator as built by BDSIM.
  std::map<std::string, int> collimatorIndicesByName;
  
  int nCollimators; ///< Number of collimators in beam line.
  std::vector<BDSOutputROOTEventCollimatorInfo> collimatorInfo; ///< Collimator information explicitly.

  /// Vector of all collimator branch names in event tree used to load data.
  std::vector<std::string> collimatorBranchNamesUnique;

  std::map<std::string, TVector3>  scoringMeshTranslation;
  std::map<std::string, TRotation> scoringMeshRotation;
  std::vector<std::string>         scoringMeshName;

  std::map<short int, std::string> materialIDToName;
  std::map<std::string, short int> materialNameToID;

	std::vector<int>         subElementIDs;
	std::vector<std::string> subElementNames;
  
  std::vector<std::string> samplerNamesUnique;
  std::vector<double>      samplerSPosition;
  std::vector<std::string> samplerCNamesUnique;
  std::vector<std::string> samplerSNamesUnique;
  std::map<std::string, double> samplerCRadius;
  std::map<std::string, double> samplerSRadius;

  ClassDef(BDSOutputROOTEventModel, 7);
};

#endif
