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
#ifndef MODEL_H
#define MODEL_H

#include "Rtypes.h" // for classdef

#include "RebdsimTypes.hh"

class BDSOutputROOTEventModel;
class TTree;

/** 
 * @brief Model loader.
 *
 * @author Stuart Walker.
 */

class Model
{
public:
  Model();
  Model(bool debugIn,
	int  dataVersionIn = 0);
  virtual ~Model();

  /// Access all the unique sampler names from the model.
  std::vector<std::string> SamplerNames() const;
  std::vector<std::string> SamplerCNames() const;
  std::vector<std::string> SamplerSNames() const;

  /// Access all the sampler branch names from the model.
  std::vector<std::string> CollimatorNames() const;

  /// Accessor.
  int DataVersion() const {return dataVersion;}
  
  /// Allow setting of data version if default constructor is used. If the default
  /// constructor was used, this function should be used before SetBranchAddress().
  inline void SetDataVersion(int dataVersionIn) {dataVersion = dataVersionIn;}

  /// Set the branch addresses to address the contents of the file.
  void SetBranchAddress(TTree* t,
			bool                      allBranchesOn    = true,
			const RBDS::VectorString* branchesToTurnOn = nullptr);

  /// @{ Member that ROOT can map file data to locally.
  BDSOutputROOTEventModel* model;
  /// @}

private:
  bool debug;
  int  dataVersion;
  
  ClassDef(Model, 2);
};

#endif
