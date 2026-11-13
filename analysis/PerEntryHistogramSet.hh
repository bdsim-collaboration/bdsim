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
#ifndef PERENTRYHISTOGRAMSET_H
#define PERENTRYHISTOGRAMSET_H
#include "HistogramDefSet.hh"
#include "PerEntryHistogram.hh"
#include "SpectraParticles.hh"

#include "BDSOutputROOTEventSampler.hh"

#include <map>
#include <set>
#include <string>
#include <vector>

class Event;
class HistogramDef;
class TChain;
class TDirectory;
class TH1;

namespace BDS
{
  /// Nice flip pair and map from https://stackoverflow.com/questions/5056645/sorting-stdmap-using-value
  /// to order the map of <pdg ID to integral> so we can get the top N histograms.
  template<typename A, typename B>
  std::pair<B, A> flip_pair(const std::pair<A, B> &p)
  {return std::pair<B, A>(p.second, p.first);}

  template<typename A, typename B>
  std::multimap<B, A> flip_map(const std::map<A, B> &src)
  {
    std::multimap<B, A> dst;
    std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()), flip_pair<A, B>);
    return dst;
  }
}

/**
 * @brief Histogram over a set of integers not number line.
 *
 * @author L. Nevay
 */

class PerEntryHistogramSet: public PerEntryHistogram
{
public:
  PerEntryHistogramSet(const HistogramDefSet* definitionIn,
		       Event*                 eventIn,
		       TChain*                chainIn);  
  virtual ~PerEntryHistogramSet();

  virtual void AccumulateCurrentEntry(long int entryNumber);
  virtual void Terminate();
  virtual void Write(TDirectory* dir = nullptr);
  
  /// Ensure sampler is setup even if it wasn't on at the beginning when
  /// we inspected the model tree. We need this to build up unique PDG IDs
  /// by manually looping over the data. A derived class must implement this
  /// for each type of sampler data we have.
  virtual void CheckSampler() = 0;

protected:
  /// Derived class should get the partID member and form a set from the specific
  /// type of sampler it is.
  virtual void GetPDGIDSetFromSampler(std::set<long long int>& setIn) const = 0;
  
  inline bool IsIon(long long int pdgID) const {return pdgID > 100000000;}

  void CreatePerEntryHistogram(long long int pdgID);

  /// Utility function to find top N in set s. Sorted in descending order of integral.
  std::vector<long long int> TopUtility(const std::set<long long int>& s,
				     size_t n) const;

  /// @{ Get top part of set. Sorted in descending order of integral.
  std::vector<long long int> TopNNonIons(int n) const;
  std::vector<long long int> TopNIons(int n) const;
  std::vector<long long int> TopN(int n) const;
  /// @}
  
  HistogramDef* baseDefinition;
  Event*        event;
  TChain*       chain;
  std::string   branchName;
  bool          dynamicallyStoreParticles;
  bool          dynamicallyStoreIons;
  long long int nEntries;
  HistogramDefSet::writewhat what;
  int           topN;

#ifdef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<double>* sampler;
#else
  BDSOutputROOTEventSampler<float>*  sampler;
#endif

  std::set<long long int>                     allPDGIDs;
  std::set<long long int>                     ions;
  std::set<long long int>                     nonIons;
  std::map<ParticleSpec, PerEntryHistogram*>  histograms;
  std::map<long long int, PerEntryHistogram*> histogramsByPDGID;
  std::vector<PerEntryHistogram*>             allPerEntryHistograms;

  //ClassDef(PerEntryHistogramSet, 1);
};

#endif
