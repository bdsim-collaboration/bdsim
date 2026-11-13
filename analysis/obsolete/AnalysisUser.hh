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
#ifndef BDSIM_ANALYSISUSER_H
#define BDSIM_ANALYSISUSER_H

#include <memory>
#include <string>
#include <vector>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#include "Options.hh"
#include "Model.hh"
#include "Run.hh"
#include "Event.hh"

/**
 * @brief User analysis class
 *
 * @author Stewart Boogert
 */

class AnalysisUser
{
public:
  AnalysisUser();
  AnalysisUser(std::string filename);
  AnalysisUser(std::vector<std::string> filenames);
  void MakeBranches(TTree* outputTree);
  void SetBranchAddresses(TTree* options, TTree* model, TTree* run, TTree* event);
  unsigned int GetNumSamplers();
  void GetEntry(int iEntry);
  void Analysis();
  virtual ~AnalysisUser();

  TFile* f;
  TTree* optionsTree;
  TTree* modelTree;
  TTree* runTree;
  TTree* eventTree;
  TFile* foutput;
  std::vector< TTree* > outputTree_Samplers;

  Options *options;
  Model   *model;
  Event   *event;
  Run     *run;

  char vertexmodel[11]{'0','0','0','0','0','0','0','0','0','0','\0'};
  float vertexx{999};
  float vertexy{999};
  float vertexz{999};
  int vertexprocess{999};
  int vertexsubprocess{999};
  int trackID{999};
  int PDG{999};
  int parentID{999};
private:
  ClassDef(AnalysisUser,1);
  unsigned int number_samplers{999};
};


#endif
