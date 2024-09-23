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
#ifndef HISTOGRAMACCUMULATOR_H
#define HISTOGRAMACCUMULATOR_H

#include <string>

#include "Rtypes.h" // for classdef

class TH1;

/**
 * @brief Class to accumulate and merge histograms in different ways.
 * 
 * This acts as a base class to accumulate a single histogram from many.
 * The default implementation is to calculate the mean and the standard
 * error on the mean, however, the AccumulateSingleValue() function is virtual
 * and may be overridden to provide different functionality.
 * 
 * TH1 is the base class of all ROOT histograms so even 2 and 3D histograms
 * can be supplied through a TH1 pointer. These are statically cast to TH1D,
 * TH2D and TH3D based on the number of dimensions (1,2 or 3 only).
 *
 * A result histogram name can be specified. Note ROOT uses this to create
 * objects in the currently open file (lots of static stuff) and therefore
 * this should be unique from the supplied base histogram name!
 * 
 * Each instance is single use. Once terminated, the accumulation should
 * not be used.
 * 
 * The algorithm used to calculate the mean and variance is one that supports
 * online calculation and is numerically stable. 
 * 
 * Normally, at least 2 entries should be accumulated to calculate the variance
 * and to avoid nans from 1/(n-1), however, in this special case, the bin error
 * is set to 0.
 *
 * @author Laurie Nevay
 */

class HistogramAccumulator
{
public:
  /// Default constructor only for ROOT reflexivity - not intended for use.
  HistogramAccumulator();
  
  /// Construct Accumulator based on another histogram of 1,2, or 3 dimensions.
  /// Inspects the TH1 (ie base class) instance to get the number of dimensions
  /// then uses the next constructor.
  HistogramAccumulator(TH1* baseHistogram,
		       const std::string& resultHistNameIn,
		       const std::string& resultHistTitleIn);
  
  /// Construct accumulator based on another histogram of 1,2 or 3 dimensions.
  /// Note, the result name must be different from that of baseHistogram if
  /// baseHistogram exists in the currently open file.
  HistogramAccumulator(TH1*               baseHistogram,
		       int                nDimensionsIn,
		       const std::string& resultHistName,
		       const std::string& resultHistTitle);

  /// Destructor deletes mean and variance temporary histograms but leaks
  /// result as deleting it would remove it from the open file in ROOT.
  virtual ~HistogramAccumulator();

  /// Loop over the bins in a histogram and accumulate that bin from a new
  /// histogram ("newValue"). newValue is assumed to be the exact same as
  /// the baseHistogram the instance of this class was constructed with.
  virtual void Accumulate(TH1* newValue);

  /// Write the result to the result histogram. Calculate the standard error
  /// on the mean from the variance for the error in each bin.
  virtual TH1* Terminate();

  /// Accessor.
  inline TH1* Result() const {return result;}

  /// Permit the number of recorded entries to be incremented with zero values,
  /// ie just increment n.
  inline void AddNEmptyEntries(unsigned long i){n += i;}
  
  /// Access currently accumulated number of entries.
  inline unsigned long N() const {return n;}

  /// Empty all contents.
  virtual void Flush();

protected:
  /// Accumulate a single value into the online mean and variance histograms.
  /// This by default accumulates the mean and variance with a new value x.
  /// The xVari argument is not used in the implementation provided with this
  /// class but is used in derived classes with different AccumulateSingleValue
  /// implementations.
  virtual void AccumulateSingleValue(double        oldMean,
				     double        oldVari,
				     double        x,
				     double        xVari,
				     unsigned long nEntriesAccumulated,
				     unsigned long nEntriesToAccumulate,
				     double&       newMean,
				     double&       newVari) const;

  int               nDimensions;     ///< Number of dimensions
  unsigned long     n;               ///< Counter.
  bool              terminated;      ///< Whether this instance has been finished.
  const std::string resultHistName;  ///< Name for resultant histogram.
  const std::string resultHistTitle; ///< Title for resultant histogram.
  TH1*              mean;
  TH1*              variance;
  TH1*              result;

  ClassDef(HistogramAccumulator,1);
};

#endif
