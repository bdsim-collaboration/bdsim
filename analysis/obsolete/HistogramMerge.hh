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
#ifndef HISTOGRAMMERGE_H
#define HISTOGRAMMERGE_H

#include <vector>

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TFile.h"

#include "BDSOutputROOTEventHistograms.hh"

class TDirectory;

/**
 * @brief Merge a set of histgrams.
 * 
 * @author Stewart Boogert
 */

class HistogramMerge
{
public:
  /// Default constructor with no action.
  HistogramMerge();

  /// Construct w.r.t. a set of event histograms. Each histogram
  /// is cloned in preparation for accumulation.
  HistogramMerge(BDSOutputROOTEventHistograms *h,
		 bool debugIn = false);
  
  virtual ~HistogramMerge();

  /// Loop over histograms used in non-default constructor and accumulate
  /// those from the input set of histograms.
  void Add(BDSOutputROOTEventHistograms *h);

  /// Loop over (1D only just now) histograms and calculate mean and std of
  /// each bin.
  void Terminate();

  /// Write the resultant histograms to an output file - ignores the argument for now.
  void Write(TFile* outputFile,
	     TDirectory* dir = nullptr);

private:
  bool debug;
  std::vector<TH1D*> histograms1D;
  std::vector<int>   histograms1DN;
  std::vector<TH1D*> histograms1DError;
  std::vector<TH2D*> histograms2D;
  std::vector<int>   histograms2DN;
  std::vector<TH2D*> histograms2DError;
  std::vector<TH3D*> histograms3D;
  std::vector<int>   histograms3DN;
  std::vector<TH3D*> histograms3DError;

  ClassDef(HistogramMerge,1);
};

#endif
