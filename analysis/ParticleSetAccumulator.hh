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
#ifndef PARTICLESETACCUMULATOR_H
#define PARTICLESETACCUMULATOR_H

#include <string>

#include "Rtypes.h" // for classdef

class ParticleSet;

/**
 * @brief Class to accumulate and merge ParticleSets in different ways.
 *
 * @author Laurie Nevay
 */

class ParticleSetAccumulator
{
public:
  /// Default constructor only for ROOT reflexivity - not intended for use.
  ParticleSetAccumulator();
  
  /// Construct accumulator based on another histogram of 1,2 or 3 dimensions.
  /// Note, the result name must be different from that of baseHistogram if
  /// baseHistogram exists in the currently open file.
  ParticleSetAccumulator(ParticleSet*               baseHistogram,
			 const std::string& resultHistName,
			 const std::string& resultHistTitle);
  
  /// Destructor deletes mean and variance temporary histograms but leaks
  /// result as deleting it would remove it from the open file in ROOT.
  virtual ~ParticleSetAccumulator();

  /// Loop over the bins in a histogram and accumulate that bin from a new
  /// histogram ("newValue"). newValue is assumed to be the exact same as
  /// the baseHistogram the instance of this class was constructed with.
  virtual void Accumulate(ParticleSet* newValue);

  /// Write the result to the result histogram. Calculate the standard error
  /// on the mean from the variance for the error in each bin.
  virtual ParticleSet* Terminate();

  /// Accessor.
  inline ParticleSet* Result() const {return result;}

  /// Permit the number of recorded entries to be incremented with zero values,
  /// ie just increment n.
  inline void AddNEmptyEntries(unsigned long i){n += i;}

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
  
  unsigned long     n;               ///< Counter.
  bool              terminated;      ///< Whether this instance has been finished.
  const std::string resultHistName;  ///< Name for resultant histogram.
  const std::string resultHistTitle; ///< Title for resultant histogram.
  ParticleSet*      mean;
  ParticleSet*      variance;
  ParticleSet*      result;

  ClassDef(ParticleSetAccumulator,1);
};

#endif
