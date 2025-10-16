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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSOutputROOT.hh"
#include "BDSOutputROOTEventAperture.hh"
#include "BDSOutputROOTEventBeam.hh"
#include "BDSOutputROOTEventCollimator.hh"
#include "BDSOutputROOTEventCoords.hh"
#include "BDSOutputROOTEventLossWorld.hh"
#include "BDSOutputROOTEventHeader.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventModel.hh"
#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventRunInfo.hh"
#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTEventSamplerC.hh"
#include "BDSOutputROOTEventSamplerS.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSOutputROOTParticleData.hh"

#include "parser/options.h"

#include "TFile.h"
#include "TObject.h"
#include "TTree.h"

BDSOutputROOT::BDSOutputROOT(const G4String& fileName,
			     G4int           fileNumberOffset,
			     G4int           compressionLevelIn):
  BDSOutput(fileName, ".root", fileNumberOffset),
  compressionLevel(compressionLevelIn),
  theRootOutputFile(nullptr),
  theHeaderOutputTree(nullptr),
  theParticleDataTree(nullptr),
  theBeamOutputTree(nullptr),
  theOptionsOutputTree(nullptr),
  theModelOutputTree(nullptr),
  theEventOutputTree(nullptr),
  theRunOutputTree(nullptr)
{;}

BDSOutputROOT::~BDSOutputROOT()
{
  Close();
}

void BDSOutputROOT::NewFile() 
{
  G4String newFileName = GetNextFileName();
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();

  theRootOutputFile = new TFile(newFileName,"RECREATE", "BDS output file");
  if (theRootOutputFile->IsZombie())
    {throw BDSException(__METHOD_NAME__, "Unable to open output file: \"" + newFileName +"\"");}
 
  if (compressionLevel > 9 || compressionLevel < -1)
    {throw BDSException(__METHOD_NAME__, "invalid ROOT compression level (" + std::to_string(compressionLevel) + ") must be 0 - 9.");}
  if (compressionLevel > -1)
    {theRootOutputFile->SetCompressionLevel(compressionLevel);}
  
  // root file - note this sets the current 'directory' to this file!
  theRootOutputFile->cd();
  
  theHeaderOutputTree  = new TTree("Header", "BDSIM Header");           // header
  theParticleDataTree  = new TTree("ParticleData", "Particle Data");    // particle data
  theBeamOutputTree    = new TTree("Beam", "BDSIM beam");               // beam data tree
  theOptionsOutputTree = new TTree("Options","BDSIM options");          // options data tree
  theModelOutputTree   = new TTree("Model","BDSIM model");              // model data tree
  theRunOutputTree     = new TTree("Run","BDSIM run histograms/information"); // run info tree
  theEventOutputTree   = new TTree("Event","BDSIM event");              // event data tree

  // Build branches for each object
  theHeaderOutputTree->Branch("Header.",       "BDSOutputROOTEventHeader",    headerOutput,     32000, 1);
  theParticleDataTree->Branch("ParticleData.", "BDSOutputROOTParticleData",   particleDataOutput,32000, 1);
  theBeamOutputTree->Branch("Beam.",           "BDSOutputROOTEventBeam",      beamOutput,       32000, 2);
  theOptionsOutputTree->Branch("Options.",     "BDSOutputROOTEventOptions",   optionsOutput,    32000, 2);
  theModelOutputTree->Branch("Model.",         "BDSOutputROOTEventModel",     modelOutput,      32000, globals->ModelSplitLevel());
  theRunOutputTree->Branch("Histos.",          "BDSOutputROOTEventHistograms",runHistos,        32000, 1);
  theRunOutputTree->Branch("Summary.",         "BDSOutputROOTEventRunInfo",   runInfo,          32000, 1);

  // Branches for event...
  // Event info output
  theEventOutputTree->Branch("Summary.",   "BDSOutputROOTEventInfo",evtInfo,32000,1);

  // Build primary structures
  if (storePrimaries)
    {
      theEventOutputTree->Branch("Primary.",       "BDSOutputROOTEventSampler",primary,       32000, 1);
      theEventOutputTree->Branch("PrimaryGlobal.", "BDSOutputROOTEventCoords", primaryGlobal, 3200,  1);
    }

  // Build loss and hit structures
  if (storeELoss)
    {theEventOutputTree->Branch("Eloss.",          "BDSOutputROOTEventLoss",   eLoss,          4000, 1);}
  if (storeELossVacuum)
    {theEventOutputTree->Branch("ElossVacuum.",    "BDSOutputROOTEventLoss",   eLossVacuum,    4000, 1);}
  if (storeELossTunnel)
    {theEventOutputTree->Branch("ElossTunnel.",    "BDSOutputROOTEventLoss",   eLossTunnel,    4000, 1);}
  if (storeELossWorld)
    {
      theEventOutputTree->Branch("ElossWorld.",     "BDSOutputROOTEventLossWorld", eLossWorld,     4000, 1);
      theEventOutputTree->Branch("ElossWorldExit.", "BDSOutputROOTEventLossWorld", eLossWorldExit, 4000, 1);
    }
  if (storeELossWorldContents)
    {theEventOutputTree->Branch("ElossWorldContents.", "BDSOutputROOTEventLossWorld", eLossWorldContents, 4000, 1);}
  theEventOutputTree->Branch("PrimaryFirstHit.","BDSOutputROOTEventLoss",      pFirstHit,      4000, 2);
  theEventOutputTree->Branch("PrimaryLastHit.", "BDSOutputROOTEventLoss",      pLastHit,       4000, 2);
  if (storeApertureImpacts)
    {theEventOutputTree->Branch("ApertureImpacts.", "BDSOutputROOTEventAperture", apertureImpacts, 4000, 1);}

  // Build trajectory structures
  if (storeTrajectory)
    {theEventOutputTree->Branch("Trajectory.", "BDSOutputROOTEventTrajectory", traj, 4000,  2);}

  // Build event histograms
  G4cout << "Store per event histos " << storePerEventHistos << G4endl;
  if (storePerEventHistos)
    {theEventOutputTree->Branch("Histos.",     "BDSOutputROOTEventHistograms", evtHistos, 32000, 1);}

  // build sampler structures
  for (G4int i = 0; i < (G4int)samplerTrees.size(); ++i)
    {
      auto samplerTreeLocal = samplerTrees.at(i);
      auto samplerName      = samplerNames.at(i);
      theEventOutputTree->Branch((samplerName+".").c_str(),
                                 "BDSOutputROOTEventSampler",
                                 samplerTreeLocal, 32000, globals->SamplersSplitLevel());
    }
  for (G4int i = 0; i < (G4int)samplerCTrees.size(); ++i)
    {
      auto samplerTreeLocal = samplerCTrees.at(i);
      auto samplerName      = samplerCNames.at(i);
      theEventOutputTree->Branch((samplerName+".").c_str(),
				 "BDSOutputROOTEventSamplerC",
				 samplerTreeLocal, 32000, globals->SamplersSplitLevel());
    }
  for (G4int i = 0; i < (G4int)samplerSTrees.size(); ++i)
    {
      auto samplerTreeLocal = samplerSTrees.at(i);
      auto samplerName      = samplerSNames.at(i);
      theEventOutputTree->Branch((samplerName+".").c_str(),
				 "BDSOutputROOTEventSamplerS",
				 samplerTreeLocal, 32000, globals->SamplersSplitLevel());
    }
  
  // build collimator structures
  if (CreateCollimatorOutputStructures())
    {
      for (G4int i = 0; i < (G4int) collimators.size(); ++i)
        {
          auto collimatorLocal = collimators.at(i);
          auto collimatorName  = collimatorNames.at(i);
          // set the tree branches
          theEventOutputTree->Branch((collimatorName + ".").c_str(),
                                     "BDSOutputROOTEventCollimator",
                                     collimatorLocal, 32000, globals->SamplersSplitLevel());
        }
    }

  FillHeader(); // this fills and then calls WriteHeader() pure virtual implemented here
}

void BDSOutputROOT::WriteHeader()
{
  theHeaderOutputTree->Fill();
}

void BDSOutputROOT::WriteHeaderEndOfFile()
{
  // there's no way to overwrite an entry in a ttree so we just add another entry with updated information
  theHeaderOutputTree->Fill();
}

void BDSOutputROOT::WriteParticleData()
{
  theParticleDataTree->Fill();
}

void BDSOutputROOT::WriteBeam()
{
  theBeamOutputTree->Fill();
}

void BDSOutputROOT::WriteOptions()
{
  theOptionsOutputTree->Fill();
}

void BDSOutputROOT::WriteModel()
{
  theModelOutputTree->Fill();
}

void BDSOutputROOT::WriteFileEventLevel()
{
  if (theRootOutputFile)
    {theRootOutputFile->cd();}
  theEventOutputTree->Fill();
}

void BDSOutputROOT::WriteFileRunLevel()
{
  if (theRootOutputFile)
    {theRootOutputFile->cd();}
  theRunOutputTree->Fill();
  
  if (theRootOutputFile)
    {
      if (theRootOutputFile->IsOpen())
	{theRootOutputFile->Write(nullptr,TObject::kOverwrite);}
    }
}

void BDSOutputROOT::CloseFile()
{
  Close();
}

void BDSOutputROOT::Close()
{
  if (theRootOutputFile)
    {
      if (theRootOutputFile->IsOpen())
	{
	  theRootOutputFile->cd();
	  theRootOutputFile->Write(0,TObject::kOverwrite);
	  G4cout << __METHOD_NAME__ << "Data written to file: " << theRootOutputFile->GetName() << G4endl;
	  theRootOutputFile->Close();
	  delete theRootOutputFile;
	  theRootOutputFile = nullptr;
	}
    }
}

void BDSOutputROOT::UpdateSamplers()
{
  G4int nNewSamplers = BDSOutputStructures::UpdateSamplerStructures();
  G4int nSamplers = (G4int)samplerTrees.size();
  for (G4int i = nSamplers - nNewSamplers; i < nSamplers; ++i)
    {
      auto samplerTreeLocal = samplerTrees.at(i);
      auto samplerName      = samplerNames.at(i);
      // set tree branches
      theEventOutputTree->Branch((samplerName+".").c_str(),
				 "BDSOutputROOTEventSampler",
				 samplerTreeLocal,32000,0);
    }
}
