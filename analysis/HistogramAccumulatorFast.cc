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
#include "HistogramAccumulatorFast.hh"

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "BDSBH4DBase.hh"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

ClassImp(HistogramAccumulatorFast)

HistogramAccumulatorFast::HistogramAccumulatorFast():
  HistogramAccumulator()
{;}

HistogramAccumulatorFast::HistogramAccumulatorFast(TH1* baseHistogramIn,
                                           const std::string& resultHistNameIn,
                                           const std::string& resultHistTitleIn):
  HistogramAccumulatorFast(baseHistogramIn, (int)baseHistogramIn->GetDimension(), resultHistNameIn, resultHistTitleIn)
{;}

HistogramAccumulatorFast::HistogramAccumulatorFast(TH1*               baseHistogramIn,
                                                   int                nDimensionsIn,
                                                   const std::string& resultHistNameIn,
                                                   const std::string& resultHistTitleIn):
  HistogramAccumulator(baseHistogramIn, nDimensionsIn, resultHistNameIn, resultHistTitleIn)
{
  std::string binsFilledName = resultHistName + "_Bins_Filled";
  // we work entirely with root's global bin index which means we can do this indexing
  // in 1D and can just use a 1D vector with the same number of total bins or 'cells'.
  binEventCount.resize(baseHistogramIn->GetNcells()); // initialises values to 0
}

HistogramAccumulatorFast::~HistogramAccumulatorFast()
{;}

void HistogramAccumulatorFast::AccumulateBinsThatWereFilledOnly(TH1* newValue, const std::set<Int_t>& binsFilled)
{
  // temporary variables
  double newMean = 0;
  double newVari = 0;
  const double error = 0; // needed to pass reference to unused parameter
  const unsigned long nEntriesToAccumulate = 1;

  n++; // must always count even if nothing to add up

  std::cout << newValue->GetTitle() << " bins filled ";
  for (auto f : binsFilled)
  {std::cout << f << " ";}
  std::cout << std::endl;

  switch (nDimensions)
  {
    case 1:
    case 2:
    case 3:
    {
      for (auto j : binsFilled)
        {
          binEventCount[j] += 1;
          AccumulateSingleValue(mean->GetBinContent(j),
                                variance->GetBinContent(j),
                                newValue->GetBinContent(j),
                                error, binEventCount[j], nEntriesToAccumulate,
                                newMean, newVari);
          mean->SetBinContent(j, newMean);
          variance->SetBinContent(j, newVari);
        }
      break;
      }
    case 4:
      {
#ifdef USE_BOOST
        BDSBH4DBase* h1  = dynamic_cast<BDSBH4DBase*>(mean);
        BDSBH4DBase* h1e = dynamic_cast<BDSBH4DBase*>(variance);
        BDSBH4DBase* ht  = dynamic_cast<BDSBH4DBase*>(newValue);
        for (auto j : binsFilled)
          {
            binEventCount[j] += 1;
            AccumulateSingleValue(h1->At(j),
                                  h1e->At(j),
                                  ht->At(j),
                                  error, binEventCount[j], nEntriesToAccumulate,
                                  newMean, newVari);
            h1->Set_BDSBH4D(j, newMean);
            h1e->Set_BDSBH4D(j, newVari);
          }
#endif
        break;
      }
    default:
      {break;}
    }
}

TH1* HistogramAccumulatorFast::Terminate()
{
  // error on mean is sqrt(1/n) * std = sqrt(1/n) * sqrt(1/(n-1)) * sqrt(variance)
  // the only variable is the variance, so take the rest out as a factor.
  const double nD = (double)n; // cast only once
  const double factor = std::sqrt(1./(nD * (nD - 1))); // nan if n = 1 -> won't be used
  double mn     = 0; // temporary variable for mean
  double var    = 0; // temporary variable for variance
  double mn2 = 0;
  double var2 = 0;
  double err2 = 0; // temporary variable for standard error on mean

  for (int j = 0; j < (int)binEventCount.size(); j++)
  {std::cout << binEventCount[j] << " ";}
  std::cout << std::endl;
  for (int j = 0; j < (int)binEventCount.size(); j++)
  {
    mn = mean->GetBinContent(j);
    std::cout << mn << " ";
  }
  std::cout << std::endl;
  for (int j = 0; j < (int)binEventCount.size(); j++)
  {
    var = variance->GetBinContent(j);
    std::cout << var << " ";
  }
  std::cout << std::endl;
  // note here we set the std to 0 if there's only one entry (ie n = 1) to avoid
  // division by zero and nans
  switch (nDimensions)
    {
    case 1:
    case 2:
    case 3:
      {
        for (int j = 0; j < (int)binEventCount.size(); j++)
          {
            mn  = mean->GetBinContent(j);
            var = variance->GetBinContent(j);
            // accumulate N x 0 for missed events all at once
            AccumulateNZeros(mn, var, binEventCount[j], n-binEventCount[j], mn2, var2);
            err2 = n > 1 ? factor*std::sqrt(var2) : 0;
            result->SetBinContent(j, mn2);
            result->SetBinError(j, err2);
          }
        break;
      }
    case 4:
      {
#ifdef USE_BOOST
        auto mnCast = dynamic_cast<BDSBH4DBase*>(mean);
        auto varCast = dynamic_cast<BDSBH4DBase*>(variance);
        auto resCast = dynamic_cast<BDSBH4DBase*>(result);
        for (int j = 0; j < (int)binEventCount.size(); j++)
          {
            mn  = mnCast->At(j);
            var = varCast->At(j);
            // accumulate N x 0 for missed events all at once - FIX FOR J TO BE GLOBAL HERE
            AccumulateNZeros(mn, var, binEventCount[j], n-binEventCount[j], mn2, var2);
            err2 = n > 1 ? factor*std::sqrt(var) : 0;
            resCast->Set_BDSBH4D(j, mn);
            resCast->SetError_BDSBH4D(j, err2);
          }
#endif
        break;
      }
    default:
      {break;}
    }
  if(nDimensions==4)
    {dynamic_cast<BDSBH4DBase*>(result)->SetEntries_BDSBH4D((double)n);}
  else
    {result->SetEntries((double)n);}

  return result;
}

void HistogramAccumulatorFast::AccumulateNZeros(double        oldMean,
                                                double        oldVari,
                                                unsigned long nEntriesAccumulated,
                                                unsigned long nZerosToAccumulate,
                                                double&       newMean,
                                                double&       newVari) const
{
  double dMean  = 0 - oldMean;
  double dMean2 = std::pow(dMean, 2);
  double nA = (double)nEntriesAccumulated;
  double nB = (double)nZerosToAccumulate;
  double nT = nA + nB;

  newMean = oldMean + nB * (dMean / nT);

  double q = (nA * nB) * (dMean2 / nT);
  newVari = oldVari + 0 + q;
}

void HistogramAccumulatorFast::Flush()
{
  HistogramAccumulator::Flush();
  std::fill(binEventCount.begin(), binEventCount.end(), 0);
}