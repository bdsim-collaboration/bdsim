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
#include "Event.hh"
#include "HistogramDef.hh"
#include "HistogramDefSet.hh"
#include "PerEntryHistogram.hh"
#include "PerEntryHistogramSet.hh"
#include "SpectraParticles.hh"

#include "BDSOutputROOTEventSampler.hh"
#include "RBDSException.hh"

#include <algorithm>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <vector>

//ClassImp(PerEntryHistogramSet)

PerEntryHistogramSet::PerEntryHistogramSet(const HistogramDefSet* definitionIn,
                                           Event*                 eventIn,
                                           TChain*                chainIn):
  baseDefinition(definitionIn->baseDefinition),
  event(eventIn),
  chain(chainIn),
  branchName(definitionIn->branchName),
  dynamicallyStoreParticles(definitionIn->dynamicallyStoreParticles),
  dynamicallyStoreIons(definitionIn->dynamicallyStoreIons),
  nEntries(0),
  what(definitionIn->what),
  topN(definitionIn->topN),
  sampler(nullptr)
{
  for (const auto& pSpecDef : definitionIn->definitions)
    {
      auto pSpec = pSpecDef.first;
      auto def   = pSpecDef.second;
      PerEntryHistogram* hist = new PerEntryHistogram(def, chainIn);
      histograms[pSpec] = hist;
      histogramsByPDGID[pSpec.first] = hist;
      allPerEntryHistograms.push_back(hist); // keep vector for quick iteration each Accumulate() call
      if (pSpec.second == RBDS::SpectraParticles::all)
        {
          if (IsIon(pSpec.first))
            {ions.insert(pSpec.first);}
          else
            {nonIons.insert(pSpec.first);}
        }
    }
}

void PerEntryHistogramSet::CreatePerEntryHistogram(long long int pdgID)
{
  allPDGIDs.insert(pdgID);
  if (IsIon(pdgID))
    {ions.insert(pdgID);}
  else
    {nonIons.insert(pdgID);}
  // copy base definition
  HistogramDef* def = baseDefinition->Clone();
  def->histName  = "Top" + std::to_string(topN) + "_Spectra_" + def->histName + "_" + std::to_string(pdgID);
  def->selection = HistogramDefSet::AddPDGFilterToSelection(ParticleSpec(pdgID,RBDS::SpectraParticles::all),
                                                            def->selection,
                                                            branchName);

  PerEntryHistogram* hist = new PerEntryHistogram(def, chain);
  hist->AddNEmptyEntries(nEntries); // update to current number of events
  histograms[ParticleSpec(pdgID, RBDS::SpectraParticles::all)] = hist;
  histogramsByPDGID[pdgID] = hist;
  allPerEntryHistograms.push_back(hist);
}

PerEntryHistogramSet::~PerEntryHistogramSet()
{
  delete baseDefinition;
  for (auto kv : allPerEntryHistograms)
    {delete kv;}
}

void PerEntryHistogramSet::AccumulateCurrentEntry(long int entryNumber)
{
  CheckSampler();

  if (dynamicallyStoreParticles || dynamicallyStoreIons)
    {
      // for this event, form a set of pdgIDs and
      // then ensure we have all prepare histograms
      std::set<long long int> pdgIDSet;
      GetPDGIDSetFromSampler(pdgIDSet);

      // use vector because set cannot be appended to for set_difference
      std::vector<long long int> missing;
      std::set_difference(pdgIDSet.begin(), pdgIDSet.end(),
                          allPDGIDs.begin(), allPDGIDs.end(),
                          std::back_inserter(missing));
      if (!missing.empty())
        {
          for (auto pdgID : missing)
            {
              bool isIon = IsIon(pdgID);
              if ((isIon && dynamicallyStoreIons) || (!isIon && dynamicallyStoreParticles))
                {CreatePerEntryHistogram(pdgID);}
            }
        }
    }
  nEntries += 1;
  for (auto hist : allPerEntryHistograms)
    {hist->AccumulateCurrentEntry(entryNumber);}
}

void PerEntryHistogramSet::Terminate()
{
  for (auto hist : allPerEntryHistograms)
    {hist->Terminate();}
}

void PerEntryHistogramSet::Write(TDirectory* dir)
{
  if (what == HistogramDefSet::writewhat::all)
    {
      for (auto hist : allPerEntryHistograms)
        {hist->Write(dir);}
    }
  else
    {
      // form a (sorted) vector of desired pdgIDs
      std::vector<long long int> desiredPDGIDs;
      switch (what)
        {
        case HistogramDefSet::writewhat::all:
          {std::copy(allPDGIDs.begin(), allPDGIDs.end(), std::back_inserter(desiredPDGIDs)); break;}
        case HistogramDefSet::writewhat::topN:
          {desiredPDGIDs = TopN(topN);        break;}
        case HistogramDefSet::writewhat::ions:
          {std::copy(ions.begin(), ions.end(), std::back_inserter(desiredPDGIDs)); break;}
        case HistogramDefSet::writewhat::topNIons:
          {desiredPDGIDs = TopNIons(topN);    break;}
        case HistogramDefSet::writewhat::particles:
          {std::copy(nonIons.begin(), nonIons.end(), std::back_inserter(desiredPDGIDs)); break;}
        case HistogramDefSet::writewhat::topNParticles:
          {desiredPDGIDs = TopNNonIons(topN); break;}
        } 

      for (auto pdgID : desiredPDGIDs)
        {histogramsByPDGID.at(pdgID)->Write(dir);}
    }
}

std::vector<long long int> PerEntryHistogramSet::TopUtility(const std::set<long long int>& s,
                                                            size_t n) const
{
  // map the pdg id to the total number (inc weight) of that particle histogrammed (per event)
  std::map<long long int, double> integrals;
  for (auto id : s)
    {integrals[id] = histogramsByPDGID.at(id)->Integral();}

  // reverse the mapping so the pdg ids are sorted by the integral, whilst tolerating
  // multiple integrals of the same value (ie same rate)
  std::multimap<double, long long int> sorted = BDS::flip_map(integrals);

  // take advantage of a multimap being (by definition) ordered - therefore, the integrals
  // are ordered low to high, so reverse iterate to get in decreasing order.
  std::vector<long long int> topResult;
  int i = 0;
  int nInt = (int)n;
  // reverse iterate up to the end of the result or n, whichever is smaller
  for (auto it = sorted.rbegin(); it != sorted.rend() && i < nInt; it++, i++)
    {topResult.push_back(it->second);}
  return topResult;
}

std::vector<long long int> PerEntryHistogramSet::TopNNonIons(int n) const
{
  return TopUtility(nonIons, (size_t)n);
}

std::vector<long long int> PerEntryHistogramSet::TopNIons(int n) const
{
  return TopUtility(ions, (size_t)n);
}
  
std::vector<long long int> PerEntryHistogramSet::TopN(int n) const
{
  return TopUtility(allPDGIDs, (size_t)n);
}
