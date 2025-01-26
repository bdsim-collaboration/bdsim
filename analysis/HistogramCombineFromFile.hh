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
#ifndef HISTOGRAMCOMBINEFROMFILE_H
#define HISTOGRAMCOMBINEFROMFILE_H

#include <vector>

#include "Rtypes.h" // for classdef

class HistogramAccumulatorMerge;

class BDSOutputROOTEventHistograms;

class TDirectory;

/**
 * @brief Accumulator to merge pre-made run histograms, i.e. combine samples
 *
 * Operate on a stored series of histograms to merge them. Single
 * use only. HistogramMeanFromFile uses the HistogramAccumulator for accumulating
 * per-entry histograms. However, this class uses the HistogramAccumulatorMerge to
 * accumulate run-level samples.
 * 
 * @author Laurie Nevay
 */

class HistogramCombineFromFile
{
public:
  /// Public constructor only for compatibility with ROOT - not intended for use.
  HistogramCombineFromFile();

  /// Use this constructor.
  HistogramCombineFromFile(BDSOutputROOTEventHistograms* h);

  virtual ~HistogramCombineFromFile();

  /// Add a new set of histograms to the running total. Assume
  /// exact same structure in BDSOutputROOTEventHistogams input.
  void Accumulate(BDSOutputROOTEventHistograms* hNew);

  /// Finish calculation.
  void Terminate();

  /// Write to file.
  void Write(TDirectory* dir = nullptr);

private:
  
  std::vector<HistogramAccumulatorMerge*> histograms1d;
  std::vector<HistogramAccumulatorMerge*> histograms2d;
  std::vector<HistogramAccumulatorMerge*> histograms3d;
  std::vector<HistogramAccumulatorMerge*> histograms4d;

  ClassDef(HistogramCombineFromFile, 1);
};

#endif
