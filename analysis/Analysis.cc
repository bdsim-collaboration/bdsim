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
#include "Analysis.hh"
#include "BinGeneration.hh"
#include "Config.hh"
#include "HistogramDef.hh"
#include "HistogramDef1D.hh"
#include "HistogramDef2D.hh"
#include "HistogramDef3D.hh"
#include "HistogramDef4D.hh"
#include "HistogramFactory.hh"
#include "HistogramMeanFromFile.hh"
#include "PerEntryHistogram.hh"
#include "rebdsim.hh"

#include "TChain.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "BDSBH4DBase.hh"

#include <iostream>
#include <string>
#include <vector>

Analysis::Analysis(const std::string& treeNameIn,
                   TChain*            chainIn,
                   const std::string& mergedHistogramNameIn,
                   bool               perEntryAnalysis,
                   bool               debugIn):
  treeName(treeNameIn),
  chain(chainIn),
  mergedHistogramName(mergedHistogramNameIn),
  histoSum(nullptr),
  debug(debugIn),
  entries(chain->GetEntries()),
  perEntry(perEntryAnalysis),
  nPerEntryHistoDefinitions(0)
{;}

Analysis::~Analysis()
{
  delete histoSum;
  for (auto pe : perEntryHistograms)
    {delete pe;}
}

void Analysis::Execute()
{
  std::cout << "Analysis on \"" << treeName << "\" beginning" << std::endl;
  if (perEntry)
    {
      // ensure new histograms are added to file
      // crucial for draw command to work as it identifies the histograms by name
      TH1::AddDirectory(kTRUE);
      TH2::AddDirectory(kTRUE);
      TH3::AddDirectory(kTRUE);
      BDSBH4DBase::AddDirectory(kTRUE);
      PreparePerEntryHistograms();
      if (nPerEntryHistoDefinitions > 0) // avoid a useless data-loading loop
        {Process();}
    }
  SimpleHistograms();
  Terminate();
  std::cout << "Analysis on \"" << treeName << "\" complete" << std::endl;
}

void Analysis::UserProcess()
{;}

void Analysis::SimpleHistograms()
{
  // loop over histogram specifications and fill
  // TODO - in future we should avoid the singleton accessor as rebdsimOptics
  // doesn't use it but uses the event analysis.
  auto c = Config::Instance();
  if (c)
    {
      auto definitions = Config::Instance()->HistogramDefinitionsSimple(treeName);
      for (auto definition : definitions)
            {FillHistogram(definition);}
    }
}

void Analysis::PreparePerEntryHistograms()
{
  auto c = Config::Instance();
  if (c)
    {
      const auto& definitions = c->HistogramDefinitionsPerEntry(treeName);
      nPerEntryHistoDefinitions += (int)definitions.size();
      for (const auto& def : definitions)
        {perEntryHistograms.push_back(new PerEntryHistogram(def, chain));}
    }
}

void Analysis::AccumulatePerEntryHistograms(long int entryNumber)
{
  for (auto& peHist : perEntryHistograms)
    {peHist->AccumulateCurrentEntry(entryNumber);}
}

void Analysis::TerminatePerEntryHistograms()
{
  for (auto& peHist : perEntryHistograms)
    {peHist->Terminate();}
}

void Analysis::Terminate()
{
  if (histoSum)
    {histoSum->Terminate();}
  if (perEntry)
    {TerminatePerEntryHistograms();}
}

void Analysis::Write(TFile* outputFile)
{
  // treeName typically has a "." at the end, deleting it here:
  std::string cleanedName     = treeName.erase(treeName.size() - 1);
  std::string perEntryDirName = "PerEntryHistograms";
  std::string simpleDirName   = "SimpleHistograms";
  std::string mergedDirName   = "MergedHistograms";
  TDirectory* rebdsimDir  = outputFile->mkdir(cleanedName.c_str());
  TDirectory* perEntryDir = rebdsimDir->mkdir(perEntryDirName.c_str());
  TDirectory* simpleDir   = rebdsimDir->mkdir(simpleDirName.c_str());
  TDirectory* mergedDir   = rebdsimDir->mkdir(mergedDirName.c_str());

  // per entry histograms
  perEntryDir->cd();
  for (auto h : perEntryHistograms)
    {h->Write(perEntryDir);}

  // simple histograms
  simpleDir->cd();
  for (auto& h : simpleHistograms)
    {simpleDir->Add(h);}
  for (auto& h : simpleHistograms)
    {h->Write();}

  // merged histograms
  if (histoSum)
    {
      mergedDir->cd();
      std::cout << "Merging histograms from \"" << treeName << "\" analysis" << std::endl;
      histoSum->Write(mergedDir);
    }

  outputFile->cd("/");  // return to root of the file
}

void Analysis::FillHistogram(HistogramDef* definition,
                             std::vector<TH1*>* outputHistograms)
{
  // ensure new histograms are added to file.
  // this is crucial for the draw command to work as it finds the histograms by name
  TH1::AddDirectory(kTRUE);
  TH2::AddDirectory(kTRUE);
  TH3::AddDirectory(kTRUE);
  BDSBH4DBase::AddDirectory(kTRUE);

  
  // pull out communal information in base class
  std::string name      = definition->histName;
  std::string command   = definition->variable + " >> " + definition->histName;
  std::string selection = definition->selection;

  HistogramFactory factory;
  TH1* h = factory.CreateHistogram(definition);
  chain->Draw(command.c_str(), selection.c_str(),"goff");

  if (outputHistograms)
    {outputHistograms->push_back(h);}
  else
    {simpleHistograms.push_back(h);}

}
