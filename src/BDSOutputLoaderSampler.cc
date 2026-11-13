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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSOutputLoaderSampler.hh"
#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventSampler.hh"
#include "BDSUtilities.hh"

#include "globals.hh"

#include "RtypesCore.h"
#include "TFile.h"
#include "TTree.h"

#include <string>

BDSOutputLoaderSampler::BDSOutputLoaderSampler(const G4String& filePath,
					       const G4String& samplerName):
  BDSOutputLoader(filePath),
  doublePrecision(false),
  nEvents(0),
  localSamplerFloat(nullptr),
  localSamplerDouble(nullptr)
{
  G4String samplerNameLocal = samplerName;
  if (!BDS::EndsWith(samplerNameLocal, "."))
    {samplerNameLocal += ".";}
  
  TBranch* search = eventTree->GetBranch(samplerNameLocal);
  if (!search)
    {throw BDSException(__METHOD_NAME__, "no such sampler name \"" + samplerName + "\"");}
  
  localSamplerDouble = new BDSOutputROOTEventSampler<double>();
  localSamplerFloat = new BDSOutputROOTEventSampler<float>();
  doublePrecision = localOptions->outputDoublePrecision;
  if (localOptions->outputDoublePrecision)
    {eventTree->SetBranchAddress(samplerNameLocal, &localSamplerDouble);}
  else
    {eventTree->SetBranchAddress(samplerNameLocal, &localSamplerFloat);}
  nEvents = (G4long)eventTree->GetEntries();
}

BDSOutputLoaderSampler::~BDSOutputLoaderSampler()
{
  delete localSamplerFloat;
  delete localSamplerDouble;
}

const BDSOutputROOTEventSampler<float>* BDSOutputLoaderSampler::SamplerDataFloat(G4long eventNumber)
{
  localSamplerFloat->Flush();
  Common(eventNumber);
  return localSamplerFloat;
}

const BDSOutputROOTEventSampler<double>* BDSOutputLoaderSampler::SamplerDataDouble(G4long eventNumber)
{
  localSamplerDouble->Flush();
  Common(eventNumber);
  return localSamplerDouble;
}

void BDSOutputLoaderSampler::Common(G4long eventNumber)
{
  if (eventNumber > nEvents)
    {
      G4String msg = "requested event number " + std::to_string(eventNumber);
      msg += " is beyond number of entries in the file (" + std::to_string(nEvents) + ")";
      throw BDSException(__METHOD_NAME__, msg);
    }
  // always change back to this file - assuming other root files could be open
  file->cd();
  
  // cannot retrieve a seed state beyond that in the file - protection here to
  // make life simpler elsewhere
  if (eventNumber > eventTree->GetEntries())
    {
      G4cout << __METHOD_NAME__ << "event index beyond number stored in file - no seed state loaded" << G4endl;
      return;
    }
  eventTree->GetEntry((Long64_t)eventNumber);
}
