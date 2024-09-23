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
#ifndef HISTOGRAMACCUMULATORFAST_H
#define HISTOGRAMACCUMULATORFAST_H

#include "HistogramAccumulator.hh"

#include <set>
#include <string>
#include <vector>

#include "Rtypes.h" // for classdef

class TH1;

/**
 * @brief Class to accumulate only bins that were filled for histograms.
 * 
 * This works in a similar way to HistogramAccumulator which it inherits.
 * The key difference is the function AccumulateBinsThatWereFilledOnly
 * that will accumulate only a given set of bins (by global index). Internally,
 * this class keeps track of which bins were accumulated how many times. In
 * the termination stage, the appropriate number of zeros is added once to
 * each given bin. This gives the exact same numerical result as if zeros
 * had been accumulated each time, i.e. by looping over every bin of every
 * accumulated histogram.
 *
 * This should be significantly faster by the ratio of the typical number of
 * bins filled per event to the total number of bins in a histogram. This can
 * become a large factor for histograms with overly fine binning.
 *
 * @author Laurie Nevay
 */

class HistogramAccumulatorFast: public HistogramAccumulator
{
public:
  /// Default constructor only for ROOT reflexivity - not intended for use.
  HistogramAccumulatorFast();
  
  /// Construct Accumulator based on another histogram of 1,2, or 3 dimensions.
  /// Inspects the TH1 (ie base class) instance to get the number of dimensions
  /// then uses the next constructor.
  HistogramAccumulatorFast(TH1* baseHistogram,
                           const std::string& resultHistNameIn,
                           const std::string& resultHistTitleIn);
  
  /// Construct accumulator based on another histogram of 1,2 or 3 dimensions.
  /// Note, the result name must be different from that of baseHistogram if
  /// baseHistogram exists in the currently open file.
  HistogramAccumulatorFast(TH1*               baseHistogram,
                           int                nDimensionsIn,
                           const std::string& resultHistName,
                           const std::string& resultHistTitle);

  /// Destructor deletes mean and variance temporary histograms but leaks
  /// result as deleting it would remove it from the open file in ROOT.
  virtual ~HistogramAccumulatorFast();

  /// Accumulate from the newValue histogram bins specified by the binsFilled
  /// set of global bin indices.
  virtual void AccumulateBinsThatWereFilledOnly(TH1* newValue, const std::set<Int_t>& binsFilled);

  /// Write the result to the result histogram. Calculate the standard error
  /// on the mean from the variance for the error in each bin.
  virtual TH1* Terminate();

  /// Add to the mean and variance as if N entries of 0 were added. This is
  /// used to apply a single offset for bins that weren't accumulated but
  /// should therefore have had 0 added each time. This is based on combining
  /// sample means and variances from HistogramAccumulatorMerge where we assume
  /// the 'B' mean and variance are 0 and added to the 'A' (old) mean and variances.
  void AccumulateNZeros(double        oldMean,
                        double        oldVari,
                        unsigned long nEntriesAccumulated,
                        unsigned long nZerosToAccumulate,
                        double&       newMean,
                        double&       newVari) const;


  /// Reset the counting in the class. The original base histogram clones remain.
  virtual void Flush();

protected:
  std::vector<unsigned long int> binEventCount;

  ClassDef(HistogramAccumulatorFast,1);
};

#endif
