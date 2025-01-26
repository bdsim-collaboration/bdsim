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
#ifndef DATALOADER_H
#define DATALOADER_H

#include "Rtypes.h" // for classdef

#include "RebdsimTypes.hh"

#include <map>
#include <set>
#include <string>
#include <vector>

class Beam;
class Event;
class ParticleData;
class Header;
class Options;
class Model;
class Run;
class TChain;
class TFile;

/**
 * @brief Loader for a ROOT file using classes used to generate the file.
 *
 * @author Stewart Boogert
 */

class DataLoader
{
public:
  DataLoader() = default;
  DataLoader(const std::string& fileName,
	     bool        debugIn           = false,
	     bool        processSamplersIn = true,
	     bool        allBranchesOn     = true,
	     const RBDS::BranchMap* branchesToTurnOn = nullptr,
	     bool        backwardsCompatibleIn = true);
  virtual ~DataLoader();

  /// Create an instance of each class in the file to be overlaid by loading
  /// the ROOT file.
  void CommonCtor(const std::string& fileName);

  /// Build up the input file list.
  void BuildInputFileList(std::string inputPath);

  /// Open the first file in the file list and map the trees in it.
  void BuildTreeNameList();

  /// Inspect the first file (leaving it open...) and build a list of samplers
  /// by checking whether they have 'Sampler' in the name.
  void BuildEventBranchNameList();

  /// Create a tree for each sampler and add all the files to it.
  void ChainTrees();

  /// Map each chain to the member instance of each storage class in this class.
  void SetBranchAddress(bool allOn = true,
                        const RBDS::BranchMap* bToTurnOn = nullptr);

  inline int DataVersion() const {return dataVersion;}

  /// Combine multiple run histograms and copy the combined average to
  /// the output file under "Event/MergedHistograms".
  void CombineRunHistogramsAndCopyToEventMerged(TFile* outputFile);

  /// @{ Accessor
  std::vector<std::string>   GetFileNames()      {return fileNames;}
  std::vector<std::string>   GetTreeNames()      {return treeNames;};
  std::vector<std::string>   GetBranchNames()    {return branchNames;}
  std::vector<std::string>   GetSamplerNames()   {return samplerNames;}
  std::vector<std::string>   GetSamplerCNames()  {return samplerCNames;}
  std::vector<std::string>   GetSamplerSNames()  {return samplerSNames;}
  std::vector<std::string>   GetCollimatorNames(){return collimatorNames;}
  Header*                    GetHeader()         {return hea;}
  ParticleData*              GetParticleData()   {return par;}
  Beam*                      GetBeam()           {return bea;}
  Options*                   GetOptions()        {return opt;}
  Model*                     GetModel()          {return mod;}
  Event*                     GetEvent()          {return evt;}
  Run*                       GetRun()            {return run;}
  TChain*                    GetHeaderTree()     {return heaChain;}
  TChain*                    GetParticleDataTree() {return parChain;}
  TChain*                    GetBeamTree()       {return beaChain;}
  TChain*                    GetOptionsTree()    {return optChain;}
  TChain*                    GetModelTree()      {return modChain;}
  TChain*                    GetEventTree()      {return evtChain;}
  TChain*                    GetRunTree()        {return runChain;}
  /// @}

  const std::set<std::string>& GetAllCylindricalAndSphericalSamplerNames() const {return allSamplerCAndSNames;}
  const std::set<std::string>& GetAllCylindricalSamplerNames() const {return allSamplerCNamesSet;}
  const std::set<std::string>& GetAllSphericalSamplerNames() const {return allSamplerSNamesSet;}

private:
  bool debug;
  bool processSamplers;
  bool allBranchesOn;
  const RBDS::BranchMap* branchesToTurnOn;
  bool backwardsCompatible;

  Header*     hea;
  ParticleData* par;
  Beam*       bea;
  Options*    opt;
  Model*      mod;
  Event*      evt;
  Run*        run;

  std::vector<std::string> fileNames;
  std::vector<std::string> treeNames;
  std::vector<std::string> branchNames;  // non-sampler branch names
  std::vector<std::string> samplerNames; // sampler branch names
  std::vector<std::string> samplerCNames;
  std::vector<std::string> samplerSNames;
  std::vector<std::string> allSamplerNames; // used to know what branches are samplers
  std::vector<std::string> allCSamplerNames;
  std::vector<std::string> allSSamplerNames;
  std::vector<std::string> collimatorNames;

  /// We need to know if a sampler is a C or S type sampler
  /// for different variable names. Build a set of them together.
  std::set<std::string> allSamplerCAndSNames;
  std::set<std::string> allSamplerCNamesSet;
  std::set<std::string> allSamplerSNamesSet;

  TChain* heaChain;
  TChain* parChain;
  TChain* beaChain;
  TChain* optChain;
  TChain* modChain;
  TChain* evtChain;
  TChain* runChain;

  int dataVersion; ///< Integer version of data loaded.

  ClassDef(DataLoader, 2);
};

#endif
