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
#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "BDSBH4DBase.hh"

#include <map>
#include <string>
#include <vector>

class HistogramDef;
class HistogramMeanFromFile;
class PerEntryHistogram;
class TChain;
class TFile;

/**
 * @brief Base class for any TTree analysis.
 *
 * @author Stuart Walker.
 */

class Analysis
{
public:
  /// Requires the name of the tree to analyse, the chain of files to operate on,
  /// the name of the directory to create in the output root file for the combined
  /// existing histograms from that tree, whether to operate on each entry in the
  /// tree and whether or not we're in debug mode.
  Analysis(const std::string& treeNameIn,
	   TChain*     chainIn,
	   const std::string& mergedHistogramNameIn,
	   bool        perEntryAnalysis = true,
	   bool        debugIn          = false);
  virtual ~Analysis();

  /// Method which calls all other methods in order.
  virtual void Execute();

  /// Operate on each entry in the tree. Pure virtual as it is not known what
  /// analysis will be formed in any derived class. This is only called if
  /// perEntry is true (by default it is).
  virtual void Process() = 0;

  /// Virtual function for user to overload and use. Does nothing by default.
  virtual void UserProcess();
  
  /// Process histogram definitions from configuration instance.
  virtual void SimpleHistograms();

  /// Create structures necessary for per entry histograms.
  void PreparePerEntryHistograms();

  /// Accumulate means and variances for per entry histograms.
  void AccumulatePerEntryHistograms(long int entryNumber);

  /// Prepare result of per entry histogram accumulation.
  void TerminatePerEntryHistograms();

  /// Optional final action after Process() and SimpleHistograms(). The version
  /// in this base class terminates the histogram merges if there are any in histoSum.
  virtual void Terminate();

  /// Write rebdsim histograms.
  virtual void Write(TFile* outputFile);

protected:
  /// Create an individual histogram based on a definition.
  void FillHistogram(HistogramDef* definition,
                     std::vector<TH1*>* outputHistograms = nullptr);

  std::string treeName;
  TChain*     chain;
  std::string                 mergedHistogramName; ///< Name of directory for merged histograms.
  std::vector<TH1*>           simpleHistograms;
  std::vector<PerEntryHistogram*> perEntryHistograms;
  HistogramMeanFromFile*      histoSum; ///< Merge of per event stored histograms.
  bool                        debug;    ///< Whether debug print out is used or not.
  long int                    entries;  ///< Number of entries in the chain.
  bool                        perEntry; ///< Whether to analyse each entry in the tree in a for loop or not.
  int nPerEntryHistoDefinitions;
  
private:
  /// No default constructor for this base class.
  Analysis() = delete;
};

#endif
