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
/**
 * @file ptc2Bdsim.cc
 */

#include "Particles.hh"
#include "TfsFile.hh"

#include "BDSOutputROOTEventHeader.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventModel.hh"
#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTEventTrajectory.hh"

#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"

int main(int argc, char *argv[])
{
  // check input
  if (argc < 5 || argc > 6)
    {
      std::cout << "usage: ptc2bdsim <ptcOutput> <outputFile> <particleName> <nominalMomentum>" << std::endl;
      std::cout << " <ptcOutput>    - output file of PTC (Tfs format)" << std::endl;
      std::cout << " <outputFile>   - desired output file name for BDSIM format file" << std::endl;
      std::cout << " <particleName> - one of e- e+ proton" << std::endl;
      std::cout << " <nominalMomentum> - nominal momentum of the beam in GeV" << std::endl;
      std::cout << " <samplersSplitLevel> - (optional) ROOT split-level of sampler branches, must be non-negative integer" << std::endl;
      exit(1);
    }
  
  std::string inputFileName  = std::string(argv[1]);
  std::string outputFileName = std::string(argv[2]);
  std::string particleName   = std::string(argv[3]);
  std::string nomMom         = std::string(argv[4]);

  int sampSplitLevel = 0;
  if (argc == 6)
    {
      std::string ss = argv[5];
      // check argument is a number. Decimal points and minus signs should be caught here
      // so this should also catch non-numeric & negative values
      for (std::string::size_type i = 0; i < ss.size(); i++)
        {
          if (ss[i] < '0' || ss[i] > '9')
            {
              std::cout << "optional argument samplersSplitLevel isn't a non-negative integer" << std::endl;
              exit(1);
            }
        }
      sampSplitLevel = std::stoi(ss);
    }

  double nominalMomentum = std::stod(nomMom);
  double mass  = 0;
  int    pdgID = 0;
  try
    {
      auto pInfo = BDSC::GetParticleInfo(particleName);
      mass  = pInfo.mass;
      pdgID = pInfo.pdgID;
    }
  catch (std::string& error)
    {
      std::cout << error << std::endl;
      exit(1);
    }
  
  // load ptc file
  PTC::TfsFile* input = new PTC::TfsFile(inputFileName);
  try
    {input->Load();}
  catch (std::string& error)
    {
      std::cout << error << std::endl;
      exit(1);
    }

  TFile* outputFile = new TFile(outputFileName.c_str(),"RECREATE");
  outputFile->cd();

  // add header for file type and version details
  BDSOutputROOTEventHeader* headerOut = new BDSOutputROOTEventHeader();
  TTree* headerTree = new TTree("Header", "BDSIM Header");
  headerTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);
  headerOut->Fill(); // updates time stamp
  headerOut->SetFileType("BDSIM");
  headerTree->Fill();
  headerOut->Flush();

  TTree* modelTree = new TTree("Model","BDSIM model");
  BDSOutputROOTEventModel* model = new BDSOutputROOTEventModel();
  modelTree->Branch("Model.", "BDSOutputROOTEventModel", model, 32000, 1);
  std::vector<std::string> samplerNamesUnique;
  for (const auto& sampler : *input)
    {samplerNamesUnique.push_back(sampler.name + ".");}
  model->samplerNamesUnique = samplerNamesUnique;
  modelTree->Fill();
  model->Flush();
  
  TTree* eventOutputTree = new TTree("Event","BDSIM event");
  
  // we have to put a few things in the file so the analysis DataLoader will work.
  auto infoLocal   = new BDSOutputROOTEventInfo();
  auto eLossLocal  = new BDSOutputROOTEventLoss();
  auto primaryFHit = new BDSOutputROOTEventLoss();
  auto primaryLHit = new BDSOutputROOTEventLoss();
  auto tunnelLocal = new BDSOutputROOTEventLoss();
  auto trajLocal   = new BDSOutputROOTEventTrajectory();
  auto histsLocal  = new BDSOutputROOTEventHistograms();
  eventOutputTree->Branch("Info.",           "BDSOutputROOTEventInfo",      infoLocal,   32000, 1);
  eventOutputTree->Branch("Eloss.",          "BDSOutputROOTEventLoss",      eLossLocal,  4000,  1);
  eventOutputTree->Branch("PrimaryFirstHit.","BDSOutputROOTEventLoss",      primaryFHit, 4000,  2);
  eventOutputTree->Branch("PrimaryLastHit.", "BDSOutputROOTEventLoss",      primaryLHit, 4000,  2);
  eventOutputTree->Branch("TunnelHit.",      "BDSOutputROOTEventLoss",      tunnelLocal, 4000,  2);
  eventOutputTree->Branch("Trajectory.",     "BDSOutputROOTEventTrajectory",trajLocal,   4000,  2);
  eventOutputTree->Branch("Histos.",         "BDSOutputROOTEventHistograms",histsLocal,  32000, 1);
  
  // shortcut for handiness
#ifndef __ROOTDOUBLE__
  typedef BDSOutputROOTEventSampler<float> samplerd;
  typedef float SamplerDataType;
#else
  typedef BDSOutputROOTEventSampler<double> samplerd;
  typedef double SamplerDataType;
#endif

  // setup local objects, branches and link to output file
  std::vector<samplerd*> localSamplers;
  for (const auto& sampler : *input)
    {
      samplerd* outputSampler = new samplerd();
      std::string sampName = sampler.name;
      outputSampler->samplerName = sampName;
      eventOutputTree->Branch((sampName+".").c_str(),
			      "BDSOutputROOTEventSampler",
			      outputSampler,32000,sampSplitLevel);
      localSamplers.push_back(outputSampler);
    }

  std::cout << "Writing to BDSIM file event by event" << std::endl;
  // we can only lose particles so the number of entries in the first
  // is the number of 'events' or entries we'll use
  int nEvents   = (int)input->segments[0].size();
  int nSamplers = (int)input->size(); // we know our localSamplers has the same size

  // loop over events and fill for all samplers
  for (int eventIndex = 0; eventIndex < nEvents; ++eventIndex)
    {
      std::cout << "\rEvent #" << std::setw(6) << eventIndex+1 << " of " << nEvents;
      std::cout.flush();
      for (int samplerIndex = 0; samplerIndex < nSamplers; ++samplerIndex)
	{
	  const auto& inputSampler = input->segments[samplerIndex];
	  if (eventIndex+1 > (int)inputSampler.size())
	    {continue;} // no hit on this sampler
	  const auto& data = inputSampler.observations[eventIndex];

	  auto& lSampler = localSamplers[samplerIndex];
	  lSampler->n = 1;

	  double p = nominalMomentum*(1. + data.pt);
	  double E = std::sqrt(std::pow(p,2) + std::pow(mass,2));
	  lSampler->energy.push_back((SamplerDataType)E);
	  lSampler->p.push_back((SamplerDataType)p);
	  lSampler->x.push_back((SamplerDataType)data.x);
	  lSampler->y.push_back((SamplerDataType)data.y);
	  lSampler->z = 0;                   // local z always 0

	  lSampler->xp.push_back((SamplerDataType)data.px);
	  lSampler->yp.push_back((SamplerDataType)data.py);
	  lSampler->zp.push_back((SamplerDataType)(std::sqrt(1 - std::pow(data.px,2) - std::pow(data.py,2))));
	  lSampler->T.push_back((SamplerDataType)data.T);

	  lSampler->weight.push_back(1);
	  lSampler->partID.push_back(pdgID);
	  lSampler->parentID.push_back(0);
	  lSampler->trackID.push_back(0);
	  lSampler->modelID = 0;
	  lSampler->turnNumber.push_back(0);
	  lSampler->S = (SamplerDataType)data.s;
	}
      outputFile->cd();
      eventOutputTree->Fill();
      // clear local sampler of any previous data
      for (auto& s : localSamplers)
        {s->Flush();}
    }
  std::cout << std::endl;
  outputFile->Write(0,TObject::kOverwrite);
  delete outputFile;
  for (auto& s : localSamplers)
    {delete s;}
  delete input;

  return 0;
}
