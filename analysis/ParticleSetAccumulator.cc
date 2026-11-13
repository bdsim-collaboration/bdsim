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
#include "ParticleSet.hh"
#include "ParticleSetAccumulator.hh"

#include <cmath>
#include <string>

ClassImp(ParticleSetAccumulator)

ParticleSetAccumulator::ParticleSetAccumulator():
  n(0),
  terminated(false),
  resultHistName(""),
  resultHistTitle(""),
  mean(nullptr),
  variance(nullptr),
  result(nullptr)
{;}

ParticleSetAccumulator::ParticleSetAccumulator(ParticleSet*   baseParticleSet,
					       const std::string& resultHistNameIn,
					       const std::string& resultHistTitleIn):
  n(0),
  terminated(false),
  resultHistName(resultHistNameIn),
  resultHistTitle(resultHistTitleIn),
  mean(nullptr),
  variance(nullptr),
  result(nullptr)
{
  std::string meanName = resultHistName + "_Mean";
  std::string variName = resultHistName + "_Vari";
  mean     = new ParticleSet(*baseParticleSet);
  variance = new ParticleSet(*baseParticleSet);
  result   = new ParticleSet(*baseParticleSet);
}

ParticleSetAccumulator::~ParticleSetAccumulator()
{
  delete mean;
  delete variance;
  delete result;
}

void ParticleSetAccumulator::Accumulate(ParticleSet* newValue)
{
  // temporary variables
  double newMean = 0;
  double newVari = 0;
  const double error   = 0; // needed to pass reference to unused parameter
  const unsigned long nEntriesToAccumulate = 1;
  
  n++;
  for (auto& keyBin : *mean)
    {
      AccumulateSingleValue(keyBin.second.value,
			    ((*variance)[keyBin.first]).value,
			    ((*newValue)[keyBin.first]).value,
			    error, n, nEntriesToAccumulate,
			    newMean, newVari);
      (*mean)[keyBin.first].value = newMean;
      (*variance)[keyBin.first].sumOfWeightsSquared = newVari;
    }
}

ParticleSet* ParticleSetAccumulator::Terminate()
{
  // error on mean is sqrt(1/n) * std = sqrt(1/n) * sqrt(1/(n-1)) * sqrt(variance)
  // the only variable is the variance, so take the rest out as a factor.
  const double nD = (double)n; // cast only once
  const double factor = std::sqrt(1./(nD * (nD - 1))); // nan if n = 1 -> won't be used
  double mn     = 0; // temporary variable for mean
  double err    = 0; // temporary variable for standard error on mean
  double var    = 0; // temporary variable for variance
  
  // note here we set the std to 0 if there's only one entry (ie n = 1) to avoid
  // division by zero and nans
  for (auto& keyBin : *mean)
    {
      mn  = keyBin.second.value;
      var = keyBin.second.sumOfWeightsSquared;
      err = n > 1 ? factor*std::sqrt(var) : 0;
      (*result)[keyBin.first].value = mn;
      (*result)[keyBin.first].sumOfWeightsSquared = err;
    }
  result->nEntries = n;
  return result;
}

void ParticleSetAccumulator::AccumulateSingleValue(double         oldMean,
						   double         oldVari,
						   double         x,
						   double       /*xVari*/,
						   unsigned long  nEntriesAccumulated,
						   unsigned long/*nEntriesToAccumulate*/,
						   double&        newMean,
						   double&        newVari) const
{
  newMean = oldMean + ((x - oldMean) / (double)nEntriesAccumulated);
  newVari = oldVari + ((x - oldMean) * (x - newMean));
}
