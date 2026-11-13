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
#ifndef PERENTRYHISTOGRAM_H
#define PERENTRYHISTOGRAM_H
#include "HistogramAccumulator.hh"

#include <string>

#include "Rtypes.h" // for classdef

class HistogramDef;

class TChain;
class TDirectory;
class TH1;

/**
 * @brief Holder for information to calculate per entry histograms.
 *
 * This creates a histogram per event and accumulates the mean and variance
 * for every bin in a cloned histogram. In the case of 1 event, the bin error
 * is 0. 
 * 
 * This uses a HistogramAccumulator object rather than inheritance as this
 * class has to prepare the base histogram in the constructor first.
 * 
 * @author Laurie Nevay
 */

class PerEntryHistogram
{
public:
  /// Public constructor only for compatibility with ROOT - not intended for use.
  PerEntryHistogram();
  
  /// Constructor with a histogram definition and the chain to operate on.
  PerEntryHistogram(const HistogramDef* definition,
		    TChain*             chain);
  virtual ~PerEntryHistogram();

  /// Create a histogram of the appropriate dimensions for the currently loaded
  /// event then add it to the online (ie running) means and variances.
  virtual void AccumulateCurrentEntry(long int entryNumber);

  /// Terminate the accumulator and save the result to the result member variable.
  virtual void Terminate();

  /// Forwarding function - call Write on result histograms on the currently
  /// open file. Optional directory to specify where the histogram should be moved to.
  virtual void Write(TDirectory* dir = nullptr);

  /// Permit the number of recorded entries to be incremented with zero values,
  /// ie just increment n.
  inline void AddNEmptyEntries(unsigned long i){accumulator->AddNEmptyEntries(i);}

  /// Get the Integral() from the result member histogram if it exists, otherwise 0.
  double Integral() const;

protected:
  HistogramAccumulator* accumulator;
  TChain*       chain;        ///< Cache of chain pointer that provides data.
  std::string   selection;    ///< Selection command.
  TH1*          temp;         ///< Histogram for temporary 1 event data.
  TH1*          result;       ///< Final result with errors as the error on the mean.
  std::string   command;      ///< Draw command.
  
  ClassDef(PerEntryHistogram, 1);
};

#endif
