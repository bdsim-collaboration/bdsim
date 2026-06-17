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
#include "BDSOutputROOTEventBeam.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "Config.hh"
#include "Event.hh"
#include "EventAnalysis.hh"
#include "HistogramMeanFromFile.hh"
#include "PerEntryHistogramSet.hh"
#include "PerEntryHistogramSetPlane.hh"
#include "PerEntryHistogramSetC.hh"
#include "PerEntryHistogramSetS.hh"
#include "RBDSException.hh"
#include "SamplerAnalysis.hh"
#include "rebdsim.hh"

#include "TChain.h"
#include "TDirectory.h"
#include "TFile.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

ClassImp(EventAnalysis)

EventAnalysis::EventAnalysis():
  Analysis("Event.", nullptr, "EventHistogramsMerged"),
  event(nullptr),
  printOut(false),
  printModulo(1),
  processSamplers(false),
  emittanceOnTheFly(false),
  calculateEventMeanHistos(false),
  eventStart(0),
  eventEnd(-1),
  nEventsToProcess(0)
{;}

EventAnalysis::EventAnalysis(Event*   eventIn,
                             TChain*  chainIn,
                             bool     perEntryAnalysis,
                             bool     processSamplersIn,
                             bool     calculateEventMeanHistosIn,
                             bool     debugIn,
                             bool     printOutIn,
                             double   printModuloFraction,
                             bool     emittanceOnTheFlyIn,
                             long int eventStartIn,
                             long int eventEndIn,
                             const std::string& primaryParticleName):
  Analysis("Event.", chainIn, "EventHistogramsMerged", perEntryAnalysis, debugIn),
  event(eventIn),
  printOut(printOutIn),
  printModulo(1),
  processSamplers(processSamplersIn),
  emittanceOnTheFly(emittanceOnTheFlyIn),
  calculateEventMeanHistos(calculateEventMeanHistosIn),
  eventStart(eventStartIn),
  eventEnd(eventEndIn),
  nEventsToProcess(eventEndIn - eventStartIn)
{
  // check we get this right for print out normalisation
  if (eventEndIn == -1)
    {nEventsToProcess = (long int)chainIn->GetEntries();}

  if (processSamplers)
    {// Create sampler analyses if needed
      // Analyse the primary sampler in the optics too.
      SamplerAnalysis* sa = nullptr;
      SamplerAnalysis* pa = nullptr;
      if (event->UsePrimaries())
        {
          sa = new SamplerAnalysis(event->GetPrimaries());
          samplerAnalyses.push_back(sa);
          pa = sa;
        }
      
      for (const auto& sampler : event->Samplers)
        {
          sa = new SamplerAnalysis(sampler, debug);
          samplerAnalyses.push_back(sa);
        }
      if (!event->UsePrimaries())
        {
          if (!samplerAnalyses.empty())
            {pa = samplerAnalyses[0];}
        }
      
      chain->GetEntry(0);
      if (!primaryParticleName.empty())
        {SamplerAnalysis::UpdateMass(primaryParticleName);}
      else if (pa)
        {SamplerAnalysis::UpdateMass(pa);}
      else
        {throw RBDSException("No samplers and no particle name - unable to calculate optics without mass of particle");}
    }
  
  SetPrintModuloFraction(printModuloFraction);
}

void EventAnalysis::Execute()
{
  std::cout << "Analysis on \"" << treeName << "\" beginning" << std::endl;
  if (perEntry || processSamplers)
    {
      // ensure new histograms are added to file
      // crucial for draw command to work as it identifies the histograms by name
      TH1::AddDirectory(kTRUE);
      TH2::AddDirectory(kTRUE);
      TH3::AddDirectory(kTRUE);
      BDSBH4DBase::AddDirectory(kTRUE);
      PreparePerEntryHistograms();
      PreparePerEntryHistogramSets();
      if (nPerEntryHistoDefinitions > 0 || processSamplers) // avoid a useless data-loading loop
        {Process();}
    }
  SimpleHistograms();
  Terminate();
  std::cout << "Analysis on \"" << treeName << "\" complete" << std::endl;
}

void EventAnalysis::SetPrintModuloFraction(double fraction)
{
  printModulo = (int)std::ceil((double)nEventsToProcess * fraction);
  if (printModulo <= 0)
    {printModulo = 1;}
}

EventAnalysis::~EventAnalysis() noexcept
{
  for (auto& sa : samplerAnalyses)
    {delete sa;}
  for (auto& hs : perEntryHistogramSets)
    {delete hs;}
}

void EventAnalysis::Process()
{
  Initialise();

  if (debug)
    {std::cout << __METHOD_NAME__ << "Entries: " << chain->GetEntries() << " " << std::endl;}

  // loop over events
  if (eventEnd < 0)
    {eventEnd = entries;}
  if (eventEnd > entries)
    {
      std::cerr << "EventEnd " << eventEnd << " > entries (" << entries
                << ") in file(s) -> curtailing to # of entries!" << std::endl;
      eventEnd = entries;
    }
  bool firstLoop = true;
  for (auto i = (Long64_t)eventStart; i < (Long64_t)eventEnd; ++i)
    {
      if (firstLoop) // ensure samplers setup for spectra before we load data
        {CheckSpectraBranches();}

      event->Flush();
      Int_t bytesLoaded = chain->GetEntry(i);
      if (debug)
        {std::cout << __METHOD_NAME__ << i << ": " << bytesLoaded << " bytes loaded" << std::endl;}
      // event analysis feedback
      if (i % printModulo == 0 && printOut)
        {
          std::cout << "\rEvent #" << std::setw(8) << i << " of " << entries;
          if (!debug)
            {std::cout.flush();}
          else
            {std::cout << std::endl;}
        }

      // merge histograms stored per event in the output
      if (calculateEventMeanHistos)
        {
          if (firstLoop)
            {histoSum = new HistogramMeanFromFile(event->Histos);}
          else
            {histoSum->Accumulate(event->Histos);}
        }

      // per event histograms
      AccumulatePerEntryHistograms(i);
      AccumulatePerEntryHistogramSets(i);

      UserProcess();

      if (debug && processSamplers)
        {
          std::cout << __METHOD_NAME__ << "Vector lengths" << std::endl;
          std::cout << __METHOD_NAME__ << "primaries=" << event->Primary->n << std::endl;
          std::cout << __METHOD_NAME__ << "eloss="     << event->Eloss->n << std::endl;
          std::cout << __METHOD_NAME__ << "nprimary="  << event->PrimaryFirstHit->n << std::endl;
          std::cout << __METHOD_NAME__ << "nlast="     << event->PrimaryLastHit->n << std::endl;
          std::cout << __METHOD_NAME__ << "ntunnel="   << event->TunnelHit->n << std::endl;
          std::cout << __METHOD_NAME__ << "ntrajectory=" << event->Trajectory->n << std::endl;
        }
      
      if (processSamplers)
        {ProcessSamplers(firstLoop);}
      if (firstLoop)
        {firstLoop = false;} // set to false on first pass of loop
    }
  std::cout << "\rSampler analysis complete                           " << std::endl;
}

void EventAnalysis::CheckSpectraBranches()
{
  for (auto s : perEntryHistogramSets)
    {s->CheckSampler();}
}

void EventAnalysis::Terminate()
{
  Analysis::Terminate();
  TerminatePerEntryHistogramSets();

  if (processSamplers)
    {
      //vector of emittance values and errors: emitt_x, emitt_y, err_emitt_x, err_emitt_y
      std::vector<double> emittance = {0,0,0,0};
      for (auto& samplerAnalysis : samplerAnalyses)
        {
          emittance = samplerAnalysis->Terminate(emittance, !emittanceOnTheFly);
          opticalFunctions.push_back(samplerAnalysis->GetOpticalFunctions());
        }
    }
}

void EventAnalysis::SimpleHistograms()
{
  Analysis::SimpleHistograms();

  auto setDefinitions = Config::Instance()->EventHistogramSetDefinitionsSimple();
  for (auto definition : setDefinitions)
    {FillHistogram(definition);}
}

void EventAnalysis::Write(TFile* outputFile)
{
  // Write rebdsim histograms:
  Analysis::Write(outputFile);

  // histogram sets done in this derived class because they only apply to the Event tree
  std::string peSetsDirName = "PerEntryHistogramSets";
  std::string siSetsDirName = "SimpleHistogramSets";
  std::string cleanedName   = treeName;
  TDirectory* treeDir       = outputFile->GetDirectory(cleanedName.c_str());
  TDirectory* peSetsDir     = treeDir->mkdir(peSetsDirName.c_str());
  TDirectory* siSetsDir     = treeDir->mkdir(siSetsDirName.c_str());
  
  // per entry histogram sets
  peSetsDir->cd();
  for (auto s : perEntryHistogramSets)
    {s->Write(peSetsDir);}
  outputFile->cd("/");

  // simple histogram sets
  siSetsDir->cd();
  for (const auto& set : simpleSetHistogramOutputs)
    {
      for (auto h : set.second)
        {
          siSetsDir->Add(h);
          h->Write();
        }
    }
  outputFile->cd("/");

  // We don't need to write out the optics tree if we didn't process samplers
  // as there's no possibility of optical data.
  if (!processSamplers)
    {return;}

  outputFile->cd("/");

  std::vector<double> xOpticsPoint;
  std::vector<double> yOpticsPoint;
  std::vector<double> lOpticsPoint;
  xOpticsPoint.resize(25);
  yOpticsPoint.resize(25);
  lOpticsPoint.resize(25);

  // write optical functions
  TTree* opticsTree = new TTree("Optics","Optics");
  opticsTree->Branch("Emitt_x", &(xOpticsPoint[0]), "Emitt_x/D");
  opticsTree->Branch("Emitt_y", &(yOpticsPoint[0]), "Emitt_y/D");
  opticsTree->Branch("Alpha_x", &(xOpticsPoint[1]), "Alpha_x/D");
  opticsTree->Branch("Alpha_y", &(yOpticsPoint[1]), "Alpha_y/D");
  opticsTree->Branch("Beta_x",  &(xOpticsPoint[2]), "Beta_x/D");
  opticsTree->Branch("Beta_y",  &(yOpticsPoint[2]), "Beta_y/D");
  opticsTree->Branch("Gamma_x", &(xOpticsPoint[3]), "Gamma_x/D");
  opticsTree->Branch("Gamma_y", &(yOpticsPoint[3]), "Gamma_y/D");
  opticsTree->Branch("Disp_x",  &(xOpticsPoint[4]), "Disp_x/D");
  opticsTree->Branch("Disp_y",  &(yOpticsPoint[4]), "Disp_y/D");
  opticsTree->Branch("Disp_xp", &(xOpticsPoint[5]), "Disp_xp/D");
  opticsTree->Branch("Disp_yp", &(yOpticsPoint[5]), "Disp_yp/D");
  opticsTree->Branch("Mean_x",  &(xOpticsPoint[6]), "Mean_x/D");
  opticsTree->Branch("Mean_y",  &(yOpticsPoint[6]), "Mean_y/D");
  opticsTree->Branch("Mean_xp", &(xOpticsPoint[7]), "Mean_xp/D");
  opticsTree->Branch("Mean_yp", &(yOpticsPoint[7]), "Mean_yp/D");
  opticsTree->Branch("Sigma_x", &(xOpticsPoint[8]), "Sigma_x/D");
  opticsTree->Branch("Sigma_y", &(yOpticsPoint[8]), "Sigma_y/D");
  opticsTree->Branch("Sigma_xp",&(xOpticsPoint[9]), "Sigma_xp/D");
  opticsTree->Branch("Sigma_yp",&(yOpticsPoint[9]), "Sigma_yp/D");
  opticsTree->Branch("S"       ,&(xOpticsPoint[10]),"S/D");
  opticsTree->Branch("Npart"   ,&(xOpticsPoint[11]),"Npart/D");

  opticsTree->Branch("Sigma_Emitt_x", &(xOpticsPoint[12]), "Sigma_Emitt_x/D");
  opticsTree->Branch("Sigma_Emitt_y", &(yOpticsPoint[12]), "Sigma_Emitt_y/D");
  opticsTree->Branch("Sigma_Alpha_x", &(xOpticsPoint[13]), "Sigma_Alpha_x/D");
  opticsTree->Branch("Sigma_Alpha_y", &(yOpticsPoint[13]), "Sigma_Alpha_y/D");
  opticsTree->Branch("Sigma_Beta_x",  &(xOpticsPoint[14]), "Sigma_Beta_x/D");
  opticsTree->Branch("Sigma_Beta_y",  &(yOpticsPoint[14]), "Sigma_Beta_y/D");
  opticsTree->Branch("Sigma_Gamma_x", &(xOpticsPoint[15]), "Sigma_Gamma_x/D");
  opticsTree->Branch("Sigma_Gamma_y", &(yOpticsPoint[15]), "Sigma_Gamma_y/D");
  opticsTree->Branch("Sigma_Disp_x",  &(xOpticsPoint[16]), "Sigma_Disp_x/D");
  opticsTree->Branch("Sigma_Disp_y",  &(yOpticsPoint[16]), "Sigma_Disp_y/D");
  opticsTree->Branch("Sigma_Disp_xp", &(xOpticsPoint[17]), "Sigma_Disp_xp/D");
  opticsTree->Branch("Sigma_Disp_yp", &(yOpticsPoint[17]), "Sigma_Disp_yp/D");
  opticsTree->Branch("Sigma_Mean_x",  &(xOpticsPoint[18]), "Sigma_Mean_x/D");
  opticsTree->Branch("Sigma_Mean_y",  &(yOpticsPoint[18]), "Sigma_Mean_y/D");
  opticsTree->Branch("Sigma_Mean_xp", &(xOpticsPoint[19]), "Sigma_Mean_xp/D");
  opticsTree->Branch("Sigma_Mean_yp", &(yOpticsPoint[19]), "Sigma_Mean_yp/D");
  opticsTree->Branch("Sigma_Sigma_x", &(xOpticsPoint[20]), "Sigma_Sigma_x/D");
  opticsTree->Branch("Sigma_Sigma_y", &(yOpticsPoint[20]), "Sigma_Sigma_y/D");
  opticsTree->Branch("Sigma_Sigma_xp",&(xOpticsPoint[21]), "Sigma_Sigma_xp/D");
  opticsTree->Branch("Sigma_Sigma_yp",&(yOpticsPoint[21]), "Sigma_Sigma_yp/D");

  opticsTree->Branch("Mean_E",        &(lOpticsPoint[6]),  "Mean_E/D");
  opticsTree->Branch("Mean_t",        &(lOpticsPoint[7]),  "Mean_t/D");
  opticsTree->Branch("Sigma_E",       &(lOpticsPoint[8]),  "Sigma_E/D");
  opticsTree->Branch("Sigma_t",       &(lOpticsPoint[9]),  "Sigma_t/D");
  opticsTree->Branch("Sigma_Mean_E",  &(lOpticsPoint[18]), "Sigma_Mean_E/D");
  opticsTree->Branch("Sigma_Mean_t",  &(lOpticsPoint[19]), "Sigma_Mean_t/D");
  opticsTree->Branch("Sigma_Sigma_E", &(lOpticsPoint[20]), "Sigma_Sigma_E/D");
  opticsTree->Branch("Sigma_Sigma_t", &(lOpticsPoint[21]), "Sigma_Sigma_t/D");

  opticsTree->Branch("xyCorrelationCoefficent", &(xOpticsPoint[24]), "xyCorrelationCoefficent/D");

  for(const auto& entry : opticalFunctions)
    {
      xOpticsPoint = entry[0];
      yOpticsPoint = entry[1];
      lOpticsPoint = entry[2];
      opticsTree->Fill();
    }
  opticsTree->Write();
}

void EventAnalysis::ProcessSamplers(bool firstTime)
{
  if (processSamplers)
    {
      for (auto s : samplerAnalyses)
        {s->Process(firstTime);}
    }
}

void EventAnalysis::Initialise()
{
  if (processSamplers)
    {
      for (auto s : samplerAnalyses)
        {s->Initialise();}
    }
}

void EventAnalysis::PreparePerEntryHistogramSets()
{
  auto c = Config::Instance();
  if (c)
    {
      auto setDefinitions  = c->EventHistogramSetDefinitionsPerEntry();
      nPerEntryHistoDefinitions += (int)setDefinitions.size();
      for (const auto& def : setDefinitions)
        {perEntryHistogramSets.push_back(ConstructPerEntryHistogramSet(def, event, chain));}
    }
}

PerEntryHistogramSet* EventAnalysis::ConstructPerEntryHistogramSet(const HistogramDefSet* definitionIn,
                                                                   Event*                 eventIn,
                                                                   TChain*                chainIn) const
{
  if (!definitionIn)
    {return nullptr;}

  PerEntryHistogramSet* result = nullptr;
  switch (definitionIn->samplerType)
  {
    case HistogramDefSet::samplertype::plane:
      {result = new PerEntryHistogramSetPlane(definitionIn, eventIn, chainIn); break;}
    case HistogramDefSet::samplertype::cylindrical:
      {result = new PerEntryHistogramSetC(definitionIn, eventIn, chainIn); break;}
    case HistogramDefSet::samplertype::spherical:
      {result = new PerEntryHistogramSetS(definitionIn, eventIn, chainIn); break;}
    default:
      {break;}
  }
  return result;
}

void EventAnalysis::AccumulatePerEntryHistogramSets(long int entryNumber)
{
  for (auto& peSet : perEntryHistogramSets)
    {peSet->AccumulateCurrentEntry(entryNumber);}
}

void EventAnalysis::TerminatePerEntryHistogramSets()
{
  for (auto& peSet : perEntryHistogramSets)
    {peSet->Terminate();}
}

void EventAnalysis::FillHistogram(HistogramDefSet* definition)
{
  std::vector<TH1*> outputHistograms;
  for (auto def : definition->definitionsV)
    {Analysis::FillHistogram(def, &outputHistograms);}
  simpleSetHistogramOutputs[definition] = outputHistograms;
}
