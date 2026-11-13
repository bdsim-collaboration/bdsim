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
#include <iostream>

#include "AnalysisUser.hh"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#include "BDSOutputROOTEventModel.hh"
#include "BDSOutputROOTEventTrajectory.hh"

void StringToCharArray(std::string const component_name,
		       char * vertexmodel)
{
  std::vector< char > tempvec(component_name.begin(),component_name.end());

  //for(int i = 0; i < tempvec.size(); ++i){
  //  std::cout << tempvec.at(i);
  //}
  for(int i = tempvec.size(); i < 10; ++i){
    tempvec.push_back(' ');
  }
  //std::cout << "tempvecsize = " << tempvec.size() << std::endl;
  tempvec.push_back('\0');
  strcpy(vertexmodel,&tempvec[0]);
}

ClassImp(AnalysisUser)

AnalysisUser::AnalysisUser(){}

AnalysisUser::AnalysisUser(std::string const filename)
{
  gROOT->Reset();
  f = new TFile(filename.data());
  optionsTree = (TTree*)f->Get("Options");
  modelTree = (TTree*)f->Get("Model");
  runTree = (TTree*)f->Get("Run");
  eventTree = (TTree*)f->Get("Event");

  options  = new Options();
  model    = new Model();
  run      = new Run();
  event    = new Event(false,true);

  this->SetBranchAddresses(optionsTree,modelTree,runTree,eventTree);
  number_samplers = GetNumSamplers();

  std::string const outputfilename = "AnalysisUserOutput_" + filename ;
  TFile* tmpoutputfile = new TFile(outputfilename.data(),"CREATE","Output ROOT file from BDSIM AnalysisUser"); 
  foutput = tmpoutputfile;
  
  for(size_t s = 0; s < number_samplers; ++s)                       // Loop over samplers
  {
    std::stringstream ss;
    ss << "Sampler" << s;
    TTree* TreeTemp( new TTree(("AnalysisUser_"+ss.str()).c_str(),("AnalysisUser TTree containing vertex information of the particles recorded in "+ss.str()).c_str()) ) ;
    outputTree_Samplers.push_back( TreeTemp );
    MakeBranches(outputTree_Samplers.at(s));
  }
}

AnalysisUser::AnalysisUser(std::vector<std::string> /*filenames*/){}

//AnalysisUser::~AnalysisUser(){}

//void AnalysisUser::SetBranchAddresses(TTree* optionsTree, TTree* modelTree, TTree* runTree, TTree* eventTree)

AnalysisUser::~AnalysisUser()
{
  delete f;
  delete optionsTree;
  delete modelTree;
  delete runTree;
  delete eventTree;

  delete options;
  delete model;
  delete run;
  delete event;
}

void AnalysisUser::SetBranchAddresses(TTree* optionsTreeIn,
				      TTree* modelTreeIn,
				      TTree* runTreeIn,
				      TTree* /*eventTreeIn*/)
{
  options->SetBranchAddress(optionsTreeIn);
  model->SetBranchAddress(modelTreeIn);
  run->SetBranchAddress(runTreeIn);

  optionsTree->GetEntry(0);
  modelTree->GetEntry(0);
  runTree->GetEntry(0);
  eventTree->GetEntry(0);

  event->SetBranchAddress(eventTree,&(model->model->samplerNamesUnique));
}

void AnalysisUser::MakeBranches(TTree* outputTree)
{
  //outputTree->Branch("S",&s,"S/F");
  outputTree->Branch("VertexModel",(void*)&vertexmodel[0],"VertexModel[11]/C");
  outputTree->Branch("VertexX",&vertexx,"VertexX/F");
  outputTree->Branch("VertexY",&vertexy,"VertexY/F");
  outputTree->Branch("VertexZ",&vertexz,"VertexZ/F");
  outputTree->Branch("VertexProcess",&vertexprocess,"VertexProcess/I");
  outputTree->Branch("VertexSubProcess",&vertexsubprocess,"VertexSubProcess/I");
  outputTree->Branch("TrackID",&trackID,"TrackID/I");
  outputTree->Branch("PDGID",&PDG,"PDGID/I");
  outputTree->Branch("ParentID",&parentID,"ParentID/I");
}

unsigned int AnalysisUser::GetNumSamplers()
{
  eventTree->GetEntry(0);
  return this->event->Samplers.size();
}

void AnalysisUser::GetEntry(int iEntry)
{
  eventTree->GetEntry(iEntry);
}

void AnalysisUser::Analysis()
{
  std::cout << "Saving vertex information to ROOT output file." << std::endl; 
  for(long long int i = 0; i < eventTree->GetEntries(); ++i) {      // Loop over file entries
    this->GetEntry(i);                                              // Get entry
    for(size_t s = 0; s < number_samplers; ++s)                      // Loop over samplers
    {
      for(int j = 0; j < this->event->Samplers[s]->n; ++j)            // Loop over sampler hits in sampler 0
      {
        int trackid = this->event->Samplers[s]->trackID[j];         // track ID for sampler info
        if(trackid != 1) {                                          // does not work for the primary
          BDSOutputROOTEventTrajectoryPoint point =
            event->Trajectory->primaryProcessPoint(trackid);        // get initial process point
          //std::cout << "Point model is " << point.model << std::endl;
          //std::cout << "This is a vector componmentname of size " << model->model->componentName.size() << std::endl;
          //std::cout << "The vector contains: " << std::endl;
          //  for(int n = 0; n < model->model->componentName.size(); ++n){
          //    std::cout << n << ": " << model->model->componentName.at(n) << std::endl;
          //  }
          try{
            StringToCharArray(model->model->componentName.at(point.model),vertexmodel);
          } catch(...){
            //std::cerr << "Warning, trying to access elements of vectors that don't exist" << std::endl;
            continue;
          }
          //s = point.position.S();
          vertexx = point.position.X();
          vertexy = point.position.Y();
          vertexz = point.position.Z();
          vertexprocess = point.processType;
          vertexsubprocess = point.processSubType;
        }else
        {
          StringToCharArray("Primary",vertexmodel);
          vertexx = 0;
          vertexy = 0;
          vertexz = 0;
          vertexprocess = -1;
          vertexsubprocess = -1;
        }
        trackID = trackid;
        PDG = event->Samplers[s]->partID[j]; 
        parentID = event->Samplers[s]->parentID[j]; 
        outputTree_Samplers.at(s)->Fill();
      }      
    }//End of samplers loop
  }
  foutput->Write();
  foutput->Close();
}
