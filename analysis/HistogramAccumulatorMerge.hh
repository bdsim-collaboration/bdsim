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
#ifndef HISTOGRAMACCUMULATORMERGE_H
#define HISTOGRAMACCUMULATORMERGE_H

#include "HistogramAccumulator.hh"

#include <string>

#include "Rtypes.h" // for classdef

class TH1;

/**
 * @brief Class to combine histograms of mean with error on mean.
 * 
 * This uses the functionality of HistogramAccumulator to accumulate
 * a combined mean and error on the mean from histograms already containing
 * means and error the mean. i.e. The incoming histograms have already
 * averaged many (at least 2) events.
 *
 * This overrides two methods for the accumulation but the inheritance
 * saves on repetition in the constructor and structures.
 * 
 * @author Laurie Nevay
 */

class HistogramAccumulatorMerge: public HistogramAccumulator
{
public:
  /// Default constructor only for ROOT reflexivity - not intended for use.
  HistogramAccumulatorMerge();
  
  /// Constructor simply passes down to base class.
  HistogramAccumulatorMerge(TH1*               baseHistogram,
			    int                nDimensionsIn,
			    const std::string& resultHistName,
			    const std::string& resultHistTitle);

  virtual ~HistogramAccumulatorMerge(){;}

  /// This method must be overridden as the information passed to the
  /// AccumualteSingleValue is different.
  virtual void Accumulate(TH1* newValue);

protected:

  /// This implements a different method from the base class that is used
  /// to combine means and variances from different samples of the same underlying
  /// experiment / data.
  virtual void AccumulateSingleValue(double        oldMean,
				     double        oldVari,
				     double        x,
				     double        xVari,
				     unsigned long nEntriesAccumulated,
				     unsigned long nEntriesToAccumulate,
				     double&       newMean,
				     double&       newVari) const;

  ClassDef(HistogramAccumulatorMerge,1);
};

#endif
