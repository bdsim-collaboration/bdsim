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
#ifndef HISTOGRAMACCUMULATORSUM_H
#define HISTOGRAMACCUMULATORSUM_H

#include "HistogramAccumulator.hh"

#include <string>

#include "Rtypes.h" // for classdef

class TH1;

/**
 * @brief Class to Sum histogram.
 *
 * This overrides two methods for the accumulation to do the simple sum using TH1::Add.
 * 
 * @author Laurie Nevay
 */

class HistogramAccumulatorSum: public HistogramAccumulator
{
public:
  /// Public constructor only for compatibility with ROOT - not intended for use.
  HistogramAccumulatorSum();
  
  /// Constructor simply passes down to base class.
  HistogramAccumulatorSum(TH1*               baseHistogram,
			  int                nDimensionsIn,
			  const std::string& resultHistName,
			  const std::string& resultHistTitle);
  
  virtual ~HistogramAccumulatorSum(){;}

  /// Use TH1::Add which works on 1,2 and 3D histograms.
  virtual void Accumulate(TH1* newValue);

  /// Simply return the result as it's already the correct result.
  virtual TH1* Terminate();

  ClassDef(HistogramAccumulatorSum, 1);
};

#endif
